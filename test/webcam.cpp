#include <fcntl.h>           // open(), O_RDWR
#include <unistd.h>          // close()
#include <sys/ioctl.h>       // ioctl() für Geräte-IOCTLs (Treiber-Kommandos)
#include <sys/mman.h>        // mmap() / munmap() für Memory-Mapping (MMAP)
#include <linux/videodev2.h> // V4L2-API: Strukturen, Konstanten, IOCTL-IDs

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

/*
 * GPT5: AI generated.
 * Required multiple prompts, but starting from a helloworld example for controlling a webcam in linux.
 * Will be used for implementation in the devicehandler.
 * Isolated different functions for easy implementation.
 *
 * Überblick:
 * Dieses Programm zeigt den klassischen Capture-Flow mit V4L2 (Video4Linux2, Linux-Kamera-API):
 *   1) Gerät öffnen (open)
 *   2) Aktuelles Format holen/setzen (VIDIOC_G_FMT / VIDIOC_S_FMT)
 *   3) Puffer (Buffers) beim Treiber anfordern (VIDIOC_REQBUFS)
 *   4) Puffer-Infos abfragen und per MMAP (Memory Mapping) in den Userspace einblenden (VIDIOC_QUERYBUF + mmap)
 *   5) Puffer in die Eingangsschlange (Queue) geben (VIDIOC_QBUF)
 *   6) Stream starten (VIDIOC_STREAMON)
 *   7) Frame aus der Ausgangsschlange holen (VIDIOC_DQBUF), speichern, Puffer zurückgeben (VIDIOC_QBUF)
 *   8) Stream stoppen und aufräumen (VIDIOC_STREAMOFF, munmap, close)
 *
 * Wichtige Begriffe:
 * - FourCC (Four-Character Code): 4-Zeichen-Kennung des Pixelformats (z. B. 'YUYV', 'MJPG').
 * - MMAP (Memory Mapping): Kernelpuffer werden direkt in den Adressraum des Programms eingeblendet (Zero-Copy).
 * - Buffer Queue (Puffer-Warteschlange): Der Treiber füllt Puffer (Frames), die Anwendung dequeuet verarbeitet und queued sie zurück.
 */

struct Buffer {
    void*  start = nullptr; // Startadresse des via mmap() gemappten Kernel-Puffers (Userspace-Sicht)
    size_t length = 0;      // Länge des Puffers in Bytes (vom Treiber vorgegeben)
};

struct Webcam {
    int fd = -1;            // Datei-Deskriptor des V4L2-Geräts (z. B. /dev/video0)
    v4l2_format fmt{};      // Aktives Videoformat: Auflösung, FourCC, Strides etc.
    std::vector<Buffer> bufs; // Liste aller gemappten MMAP-Puffer
};

/**
 * @brief Hilfsfunktion: konvertiert einen FourCC (uint32_t) in eine lesbare 4-Zeichen-Zeichenkette.
 * Beispiel: 0x56595559 -> "YUYV"
 */
static std::string fourcc_to_str(uint32_t f) {
    char s[5] = {
        char(f & 0xFF),            // Byte 0
        char((f >> 8) & 0xFF),     // Byte 1
        char((f >> 16) & 0xFF),    // Byte 2
        char((f >> 24) & 0xFF),    // Byte 3
        0                           // Terminator
    };
    return std::string(s);
}

/**
 * @brief Farbraumkonvertierung: YUYV (4:2:2) -> RGB24.
 *
 * Fachbegriffe:
 * - Y (Luminanz): Helligkeit pro Pixel
 * - U/V (Chrominanz): Farbdifferenzwerte, bei 4:2:2 für je 2 Pixel geteilt
 *
 * Speicherlayout YUYV (pro 2 Pixel = 4 Bytes): Y0 U Y1 V
 * -> Wir berechnen daraus zwei RGB-Pixel mit gemeinsamem U/V.
 *
 * @param in  Eingabezeiger auf YUYV-bytes (2 Bytes pro Pixel)
 * @param out Ausgabezeiger auf RGB24 (3 Bytes pro Pixel)
 * @param w   Bildbreite
 * @param h   Bildhöhe
 */
static void yuyv_to_rgb(const uint8_t* in, uint8_t* out, int w, int h) {
    // clamp: Werte in [0,255] begrenzen (Vermeidung von Overflow/Underflow nach der Matrixrechnung)
    auto clamp = [](int x) { return static_cast<uint8_t>(std::min(255, std::max(0, x))); };

    // Wir verarbeiten pro Schleifendurchlauf 2 Pixel (Y0,U,Y1,V)
    for (int i = 0; i < w * h; i += 2) {
        int y0 = in[0];            // Luminanz Pixel 0
        int u  = in[1] - 128;      // U um 128 zentriert -> signed
        int y1 = in[2];            // Luminanz Pixel 1
        int v  = in[3] - 128;      // V um 128 zentriert -> signed

        // Näherung der BT.601-Matrix für YUV -> RGB
        auto conv = [&](int y, int& r, int& g, int& b) {
            r = static_cast<int>(y + 1.402     * v);
            g = static_cast<int>(y - 0.344136  * u - 0.714136 * v);
            b = static_cast<int>(y + 1.772     * u);
        };

        int r,g,b;
        // Pixel 0 (Y0, U, V)
        conv(y0, r,g,b);
        out[0] = clamp(r);
        out[1] = clamp(g);
        out[2] = clamp(b);

        // Pixel 1 (Y1, U, V)
        conv(y1, r,g,b);
        out[3] = clamp(r);
        out[4] = clamp(g);
        out[5] = clamp(b);

        in  += 4;  // 4 YUYV-Bytes konsumiert -> 2 Pixel
        out += 6;  // 2 * 3 RGB-Bytes geschrieben
    }
}

/**
 * @brief Fehler-Helfer: zeigt errno-abhängige Fehlermeldung via perror() und gibt false zurück.
 */
static bool fail(const char* msg) { std::perror(msg); return false; }

/**
 * @brief Initialisiert das V4L2-Gerät, handelt Format aus, richtet MMAP-Puffer ein und startet den Stream.
 *
 * Parameter:
 * - dev: Gerätepfad (z. B. "/dev/video0")
 * - width/height: gewünschte Auflösung (Treiber darf anpassen)
 * - pixfmt_fourcc: gewünschtes Pixelformat als FourCC (0 = automatisch/beibehalten)
 * - bufferCount: Anzahl MMAP-Puffer (>=2 für Double-Buffering; 3–6 sind üblich)
 *
 * Rückgabe:
 * - true bei Erfolg, sonst false (Fehler wird via perror() erklärt)
 */
bool startupWebcam(Webcam& cam,
                   const char* dev        = "/dev/video0",
                   uint32_t width         = 640,
                   uint32_t height        = 480,
                   uint32_t pixfmt_fourcc = 0,
                   uint32_t bufferCount   = 4) {
    std::cout << "\n=== startupWebcam ===\n"
              << "device=" << dev << " width=" << width << " height=" << height
              << " request_pixfmt=" << (pixfmt_fourcc ? fourcc_to_str(pixfmt_fourcc) : std::string("auto"))
              << " buffers=" << bufferCount << "\n";

    // (1) Gerät öffnen (Lese-/Schreibmodus). Blockierend ist okay für Minimalbeispiel.
    cam.fd = open(dev, O_RDWR);
    if (cam.fd < 0) return fail("open");

    // (2) Aktuelles Format vom Treiber holen (G_FMT)
    cam.fmt = {};
    cam.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam.fd, VIDIOC_G_FMT, &cam.fmt) < 0) return fail("VIDIOC_G_FMT");

    // Wunschwerte setzen: Auflösung und optional FourCC (Treiber darf abrunden/anpassen)
    cam.fmt.fmt.pix.width  = width;
    cam.fmt.fmt.pix.height = height;
    if (pixfmt_fourcc) cam.fmt.fmt.pix.pixelformat = pixfmt_fourcc;

    // (3) Format setzen (S_FMT). Rückgabewerte nicht hart erzwingen:
    //     Der Treiber kann andere gültige Werte wählen (z. B. Line-Stride, Nähe statt exakt).
    (void)ioctl(cam.fd, VIDIOC_S_FMT, &cam.fmt);

    std::cout << "active_format: " << cam.fmt.fmt.pix.width << "x" << cam.fmt.fmt.pix.height
              << " fourcc=" << fourcc_to_str(cam.fmt.fmt.pix.pixelformat) << "\n";

    // (4) Puffer beim Treiber anfordern: Strategie = MMAP
    //     req.count ist ein Wunschwert – Treiber kann weniger zurückmelden.
    v4l2_requestbuffers req{};
    req.count  = bufferCount;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam.fd, VIDIOC_REQBUFS, &req) < 0) return fail("VIDIOC_REQBUFS");
    if (req.count < 2) { // mindestens Double-Buffering
        std::cerr << "Too few buffers.\n";
        return false;
    }

    // (5) Für jeden Puffer: Metadaten erfragen (QUERYBUF), via mmap() einblenden, initial queue'n (QBUF)
    cam.bufs.resize(req.count);
    for (uint32_t i = 0; i < req.count; ++i) {
        v4l2_buffer buf{};
        buf.type   = req.type;
        buf.memory = req.memory;
        buf.index  = i;

        // QUERYBUF: Offset/Länge dieses Puffers abrufen
        if (ioctl(cam.fd, VIDIOC_QUERYBUF, &buf) < 0) return fail("VIDIOC_QUERYBUF");

        // mmap(): Kernelpuffer in den Userspace abbilden (MMAP = Zero-Copy, sehr effizient)
        cam.bufs[i].length = buf.length;
        cam.bufs[i].start  = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam.fd, buf.m.offset);
        if (cam.bufs[i].start == MAP_FAILED) return fail("mmap");

        // QBUF: Puffer initial in die Eingangsqueue geben; der Treiber darf ihn füllen
        if (ioctl(cam.fd, VIDIOC_QBUF, &buf) < 0) return fail("VIDIOC_QBUF");
    }
    std::cout << "queued_buffers=" << cam.bufs.size() << "\n";

    // (6) STREAMON: Startet den Videostrom (ab jetzt füllt der Treiber die gequeueten Puffer)
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam.fd, VIDIOC_STREAMON, &type) < 0) return fail("VIDIOC_STREAMON");

    std::cout << "stream=ON\n";
    return true;
}

/**
 * @brief Stoppt den Stream, gibt alle MMAP-Puffer frei und schließt das Gerät.
 * Idempotent (mehrfacher Aufruf ist sicher).
 */
bool shutdownWebcam(Webcam& cam) {
    std::cout << "\n=== shutdownWebcam ===\n";
    if (cam.fd < 0) { // bereits geschlossen
        std::cout << "already closed\n";
        return true;
    }

    // STREAMOFF: Videostrom stoppen. Danach keine DQBUF/QBUF-Operationen mehr durchführen.
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    (void)ioctl(cam.fd, VIDIOC_STREAMOFF, &type);

    // MMAP-Puffer zurückgeben (munmap() je Buffer), Reihenfolge egal
    for (auto& b : cam.bufs) {
        if (b.start && b.length) munmap(b.start, b.length);
    }
    cam.bufs.clear();

    // Gerät schließen
    close(cam.fd);
    cam.fd = -1;

    std::cout << "stream=OFF, unmapped buffers, closed\n";
    return true;
}

/**
 * @brief Holt genau ein Frame (DQBUF), speichert es abhängig vom FourCC und queued den Puffer wieder (QBUF).
 *
 * Formate:
 * - MJPEG/JPEG: Rohstream liegt bereits als JPEG-Bytefolge vor -> 1:1 in Datei schreiben (*.jpg).
 * - YUYV: Planar Packed YUV 4:2:2 -> nach RGB24 konvertieren und als PPM (P6) schreiben (*.ppm).
 *
 * Wichtige IOCTLs:
 * - VIDIOC_DQBUF (DeQueue Buffer): Nimmt einen gefüllten Puffer aus der Ausgabewarteschlange.
 * - VIDIOC_QBUF  (Queue Buffer): Gibt denselben Puffer nach Verarbeitung zurück in die Eingangsqueue.
 */
bool takeSnapshot(Webcam& cam, const std::string& path) {
    std::cout << "\n=== takeSnapshot ===\n" << "path=" << path << "\n";
    if (cam.fd < 0) return false;

    // (1) DQBUF: Blockiert standardmäßig, bis ein Puffer gefüllt ist (je nach Treiber/Device).
    v4l2_buffer buf{};
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam.fd, VIDIOC_DQBUF, &buf) < 0) return fail("VIDIOC_DQBUF");

    const uint32_t fourcc = cam.fmt.fmt.pix.pixelformat;
    const int W = cam.fmt.fmt.pix.width, H = cam.fmt.fmt.pix.height;

    std::cout << "dequeued index=" << buf.index << " bytes=" << buf.bytesused
              << " fourcc=" << fourcc_to_str(fourcc) << " size=" << W << "x" << H << "\n";

    bool ok = false;

    // (2a) MJPEG/JPEG: Bytefolge ist bereits komprimiertes JPEG -> unverändert speichern
    if (fourcc == V4L2_PIX_FMT_MJPEG || fourcc == V4L2_PIX_FMT_JPEG) {
        if (FILE* f = std::fopen(path.c_str(), "wb")) {
            std::fwrite(cam.bufs[buf.index].start, 1, buf.bytesused, f);
            std::fclose(f);
            std::cout << "wrote JPEG\n";
            ok = true;
        }
    }
    // (2b) YUYV: Nach RGB24 konvertieren und als PPM (P6) speichern
    else if (fourcc == V4L2_PIX_FMT_YUYV) {
        std::vector<uint8_t> rgb(3 * W * H); // 3 Byte pro Pixel (R,G,B)
        yuyv_to_rgb(static_cast<uint8_t*>(cam.bufs[buf.index].start), rgb.data(), W, H);
        if (FILE* f = std::fopen(path.c_str(), "wb")) {
            // PPM P6: binär, Header "P6\n<breite> <höhe>\n<maxval>\n" gefolgt von RGB-Daten
            std::fprintf(f, "P6\n%d %d\n255\n", W, H);
            std::fwrite(rgb.data(), 1, rgb.size(), f);
            std::fclose(f);
            std::cout << "wrote PPM\n";
            ok = true;
        }
    }
    // (2c) Andere Formate: in diesem Minimalbeispiel nicht unterstützt
    else {
        std::cerr << "unsupported FourCC\n";
    }

    // (3) QBUF: Puffer wieder in die Eingangsqueue legen (sonst „verschwinden“ Puffer aus dem Ring)
    (void)ioctl(cam.fd, VIDIOC_QBUF, &buf);
    std::cout << "requeued index=" << buf.index << "\n";
    return ok;
}

/**
 * @brief Liest die Framerate aus (timeperframe = 1/fps) via VIDIOC_G_PARM.
 *
 * Fachbegriff:
 * - timeperframe (tpf): Bruch (numerator/denominator), z. B. 1/30 -> 30 fps.
 */
bool getFramerate(Webcam& cam, double& fps_out) {
    std::cout << "\n=== getFramerate ===\n";
    if (cam.fd < 0) return false;

    v4l2_streamparm parm{};
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    // G_PARM: liefert u. a. capture.timeperframe
    if (ioctl(cam.fd, VIDIOC_G_PARM, &parm) < 0) return fail("VIDIOC_G_PARM");
    const auto& tpf = parm.parm.capture.timeperframe;

    // defensiv: Nenner/Zähler dürfen nicht 0 sein
    if (!tpf.numerator || !tpf.denominator) {
        std::cout << "no timeperframe\n";
        return false;
    }

    fps_out = static_cast<double>(tpf.denominator) / static_cast<double>(tpf.numerator);
    std::cout << "fps=" << fps_out << "\n";
    return true;
}

/**
 * @brief Minimaler End-to-End-Test:
 *  - öffnet Device, startet Stream
 *  - liest optional Framerate
 *  - nimmt ein einzelnes Bild auf und speichert es (JPEG oder PPM)
 *  - fährt sauber herunter
 *
 * Aufruf:
 *   ./app [/dev/videoX]
 */
int main(int argc, char** argv) {
    std::cout << "=== main: webcam test ===\n";
    const char* dev = (argc > 1) ? argv[1] : "/dev/video0";
    std::cout << "device=" << dev << "\n";

    Webcam cam;

    // Initialisierung + Streamstart (Auflösung 640x480, FourCC automatisch, 4 Puffer)
    if (!startupWebcam(cam, dev, 640, 480, 0, 4)) {
        std::cerr << "Startup failed.\n";
        return 1;
    }

    // Framerate abfragen (falls Treiber timeperframe liefert)
    double fps;
    if (getFramerate(cam, fps)) std::cout << "driver_fps=" << fps << "\n";

    // Ausgabedatei abhängig vom aktiven FourCC wählen
    std::string out = (cam.fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG || cam.fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_JPEG)
                    ? "frame.jpg"
                    : "frame.ppm";

    // Ein einzelnes Bild aufnehmen und speichern
    (void)takeSnapshot(cam, out);

    // Sauber herunterfahren (STREAMOFF, munmap, close)
    (void)shutdownWebcam(cam);
    return 0;
}

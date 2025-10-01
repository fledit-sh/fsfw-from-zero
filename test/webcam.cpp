// file: test/webcam_mmap.cpp
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

struct Buffer {
    void*  start = nullptr;
    size_t length = 0;
};

static void yuyv_to_rgb(const uint8_t* in, uint8_t* out, int w, int h) {
    for (int i = 0; i < w * h; i += 2) {
        int y0 = in[0], u = in[1] - 128, y1 = in[2], v = in[3] - 128;
        auto clamp = [&](int x) { return static_cast<uint8_t>(std::min(255, std::max(0, x))); };
        auto conv  = [&](int y, int u, int v, int& r, int& g, int& b) {
            r = static_cast<int>(y + 1.402 * v);
            g = static_cast<int>(y - 0.344136 * u - 0.714136 * v);
            b = static_cast<int>(y + 1.772 * u);
        };
        int r,g,b;
        conv(y0,u,v,r,g,b);
        out[0]=clamp(r); out[1]=clamp(g); out[2]=clamp(b);
        conv(y1,u,v,r,g,b);
        out[3]=clamp(r); out[4]=clamp(g); out[5]=clamp(b);
        in += 4; out += 6;
    }
}

int main(int argc, char** argv) {
    const char* dev = (argc > 1) ? argv[1] : "/dev/video0";
    int fd = open(dev, O_RDWR);
    if (fd < 0) { std::cerr << "Kann " << dev << " nicht öffnen.\n"; return 1; }

    // Format ermitteln und (optional) Wunsch setzen
    v4l2_format fmt{}; fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) { std::cerr << "VIDIOC_G_FMT fehlgeschlagen.\n"; return 1; }

    // Beispiel: sanft auf 640x480 bleiben, FourCC nicht hart erzwingen.
    // Falls du MJPEG willst: fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    // Falls du YUYV willst:  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.width  = 640;
    fmt.fmt.pix.height = 480;
    ioctl(fd, VIDIOC_S_FMT, &fmt); // best-effort

    // Buffer anfordern (MMAP)
    v4l2_requestbuffers req{};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0 || req.count < 2) {
        std::cerr << "REQBUFS fehlgeschlagen oder zu wenige Buffer.\n"; return 1;
    }

    std::vector<Buffer> bufs(req.count);
    for (uint32_t i = 0; i < req.count; ++i) {
        v4l2_buffer buf{};
        buf.type = req.type; buf.memory = req.memory; buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) { std::cerr << "QUERYBUF fehlgeschlagen.\n"; return 1; }
        bufs[i].length = buf.length;
        bufs[i].start  = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (bufs[i].start == MAP_FAILED) { std::cerr << "mmap fehlgeschlagen.\n"; return 1; }

        // In die Queue geben
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { std::cerr << "QBUF fehlgeschlagen.\n"; return 1; }
    }

    // Stream starten
    v4l2_buf_type type = static_cast<v4l2_buf_type>(req.type);
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) { std::cerr << "STREAMON fehlgeschlagen.\n"; return 1; }

    // Einen Frame dequeue’n (blocking)
    v4l2_buffer buf{};
    buf.type = req.type; buf.memory = req.memory;
    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) { std::cerr << "DQBUF fehlgeschlagen.\n"; return 1; }

    // Bild speichern – abhängig vom Pixelformat
    uint32_t fourcc = fmt.fmt.pix.pixelformat;
    int W = fmt.fmt.pix.width, H = fmt.fmt.pix.height;

    if (fourcc == V4L2_PIX_FMT_MJPEG || fourcc == V4L2_PIX_FMT_JPEG) {
        // Viele Cams liefern gültige JPEG-Frames -> direkt dumpen
        FILE* f = std::fopen("frame.jpg", "wb");
        if (!f) { std::cerr << "Konnte frame.jpg nicht öffnen.\n"; return 1; }
        std::fwrite(bufs[buf.index].start, 1, buf.bytesused, f);
        std::fclose(f);
        std::cout << "Gespeichert: frame.jpg (" << W << "x" << H << ", " << buf.bytesused << " Bytes, MJPEG)\n";
    } else if (fourcc == V4L2_PIX_FMT_YUYV) {
        std::vector<uint8_t> rgb(3 * W * H);
        yuyv_to_rgb(static_cast<uint8_t*>(bufs[buf.index].start), rgb.data(), W, H);
        FILE* f = std::fopen("frame.ppm", "wb");
        if (!f) { std::cerr << "Konnte frame.ppm nicht öffnen.\n"; return 1; }
        std::fprintf(f, "P6\n%d %d\n255\n", W, H);
        std::fwrite(rgb.data(), 1, rgb.size(), f);
        std::fclose(f);
        std::cout << "Gespeichert: frame.ppm (" << W << "x" << H << ", YUYV)\n";
    } else {
        std::cerr << "Nicht unterstütztes Pixelformat: 0x" << std::hex << fourcc << std::dec << "\n";
    }

    // Buffer zurück in die Queue (optional, da wir gleich stoppen)
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) { std::cerr << "QBUF (requeue) fehlgeschlagen.\n"; }

    // Stream stoppen und aufräumen
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (auto& b : bufs) if (b.start) munmap(b.start, b.length);
    close(fd);
    return 0;
}

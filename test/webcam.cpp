#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

struct Buffer {
    void*  start = nullptr;
    size_t length = 0;
};

struct Webcam {
    int fd = -1;
    v4l2_format fmt{};
    std::vector<Buffer> bufs;
};

static std::string fourcc_to_str(uint32_t f) {
    char s[5] = {char(f & 0xFF), char((f >> 8) & 0xFF), char((f >> 16) & 0xFF), char((f >> 24) & 0xFF), 0};
    return std::string(s);
}

static void yuyv_to_rgb(const uint8_t* in, uint8_t* out, int w, int h) {
    auto clamp = [](int x) { return static_cast<uint8_t>(std::min(255, std::max(0, x))); };
    for (int i = 0; i < w * h; i += 2) {
        int y0 = in[0], u = in[1] - 128, y1 = in[2], v = in[3] - 128;
        auto conv = [&](int y, int& r, int& g, int& b) {
            r = static_cast<int>(y + 1.402 * v);
            g = static_cast<int>(y - 0.344136 * u - 0.714136 * v);
            b = static_cast<int>(y + 1.772 * u);
        };
        int r,g,b;
        conv(y0, r,g,b); out[0]=clamp(r); out[1]=clamp(g); out[2]=clamp(b);
        conv(y1, r,g,b); out[3]=clamp(r); out[4]=clamp(g); out[5]=clamp(b);
        in += 4; out += 6;
    }
}

static bool fail(const char* msg) { std::perror(msg); return false; }

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

    cam.fd = open(dev, O_RDWR);
    if (cam.fd < 0) return fail("open");

    cam.fmt = {};
    cam.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam.fd, VIDIOC_G_FMT, &cam.fmt) < 0) return fail("VIDIOC_G_FMT");

    cam.fmt.fmt.pix.width  = width;
    cam.fmt.fmt.pix.height = height;
    if (pixfmt_fourcc) cam.fmt.fmt.pix.pixelformat = pixfmt_fourcc;
    (void)ioctl(cam.fd, VIDIOC_S_FMT, &cam.fmt);

    std::cout << "active_format: " << cam.fmt.fmt.pix.width << "x" << cam.fmt.fmt.pix.height
              << " fourcc=" << fourcc_to_str(cam.fmt.fmt.pix.pixelformat) << "\n";

    v4l2_requestbuffers req{};
    req.count  = bufferCount;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam.fd, VIDIOC_REQBUFS, &req) < 0) return fail("VIDIOC_REQBUFS");
    if (req.count < 2) { std::cerr << "Too few buffers.\n"; return false; }

    cam.bufs.resize(req.count);
    for (uint32_t i = 0; i < req.count; ++i) {
        v4l2_buffer buf{};
        buf.type = req.type; buf.memory = req.memory; buf.index = i;
        if (ioctl(cam.fd, VIDIOC_QUERYBUF, &buf) < 0) return fail("VIDIOC_QUERYBUF");

        cam.bufs[i].length = buf.length;
        cam.bufs[i].start  = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, cam.fd, buf.m.offset);
        if (cam.bufs[i].start == MAP_FAILED) return fail("mmap");

        if (ioctl(cam.fd, VIDIOC_QBUF, &buf) < 0) return fail("VIDIOC_QBUF");
    }
    std::cout << "queued_buffers=" << cam.bufs.size() << "\n";

    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam.fd, VIDIOC_STREAMON, &type) < 0) return fail("VIDIOC_STREAMON");

    std::cout << "stream=ON\n";
    return true;
}

bool shutdownWebcam(Webcam& cam) {
    std::cout << "\n=== shutdownWebcam ===\n";
    if (cam.fd < 0) { std::cout << "already closed\n"; return true; }
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    (void)ioctl(cam.fd, VIDIOC_STREAMOFF, &type);
    for (auto& b : cam.bufs) {
        if (b.start && b.length) munmap(b.start, b.length);
    }
    cam.bufs.clear();
    close(cam.fd);
    cam.fd = -1;
    std::cout << "stream=OFF, unmapped buffers, closed\n";
    return true;
}

bool takeSnapshot(Webcam& cam, const std::string& path) {
    std::cout << "\n=== takeSnapshot ===\n" << "path=" << path << "\n";
    if (cam.fd < 0) return false;

    v4l2_buffer buf{};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (ioctl(cam.fd, VIDIOC_DQBUF, &buf) < 0) return fail("VIDIOC_DQBUF");

    const uint32_t fourcc = cam.fmt.fmt.pix.pixelformat;
    const int W = cam.fmt.fmt.pix.width, H = cam.fmt.fmt.pix.height;
    std::cout << "dequeued index=" << buf.index << " bytes=" << buf.bytesused
              << " fourcc=" << fourcc_to_str(fourcc) << " size=" << W << "x" << H << "\n";

    bool ok = false;
    if (fourcc == V4L2_PIX_FMT_MJPEG || fourcc == V4L2_PIX_FMT_JPEG) {
        if (FILE* f = std::fopen(path.c_str(), "wb")) {
            std::fwrite(cam.bufs[buf.index].start, 1, buf.bytesused, f);
            std::fclose(f);
            std::cout << "wrote JPEG\n";
            ok = true;
        }
    } else if (fourcc == V4L2_PIX_FMT_YUYV) {
        std::vector<uint8_t> rgb(3 * W * H);
        yuyv_to_rgb(static_cast<uint8_t*>(cam.bufs[buf.index].start), rgb.data(), W, H);
        if (FILE* f = std::fopen(path.c_str(), "wb")) {
            std::fprintf(f, "P6\n%d %d\n255\n", W, H);
            std::fwrite(rgb.data(), 1, rgb.size(), f);
            std::fclose(f);
            std::cout << "wrote PPM\n";
            ok = true;
        }
    } else {
        std::cerr << "unsupported FourCC\n";
    }

    (void)ioctl(cam.fd, VIDIOC_QBUF, &buf);
    std::cout << "requeued index=" << buf.index << "\n";
    return ok;
}

bool getFramerate(Webcam& cam, double& fps_out) {
    std::cout << "\n=== getFramerate ===\n";
    if (cam.fd < 0) return false;
    v4l2_streamparm parm{};
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(cam.fd, VIDIOC_G_PARM, &parm) < 0) return fail("VIDIOC_G_PARM");
    const auto& tpf = parm.parm.capture.timeperframe;
    if (!tpf.numerator || !tpf.denominator) { std::cout << "no timeperframe\n"; return false; }
    fps_out = static_cast<double>(tpf.denominator) / static_cast<double>(tpf.numerator);
    std::cout << "fps=" << fps_out << "\n";
    return true;
}

int main(int argc, char** argv) {
    std::cout << "=== main: webcam test ===\n";
    const char* dev = (argc > 1) ? argv[1] : "/dev/video0";
    std::cout << "device=" << dev << "\n";

    Webcam cam;
    if (!startupWebcam(cam, dev, 640, 480, 0, 4)) {
        std::cerr << "Startup failed.\n";
        return 1;
    }
    double fps;
    if (getFramerate(cam, fps)) std::cout << "driver_fps=" << fps << "\n";
    std::string out = (cam.fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG || cam.fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_JPEG)
                    ? "frame.jpg" : "frame.ppm";
    (void)takeSnapshot(cam, out);
    (void)shutdownWebcam(cam);
    return 0;
}

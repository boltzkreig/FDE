/*
 * BUILD: gcc live-wallpaper.c -o live-wallpaper -lX11 -lGL -lm
 * A Live Wallpaper that uses GPU for rendering with minimal resource load for X11.
 * NOTE: LLM Generated. Colorscheme, complexity &
   framerate etc manually set & tuned to be resource efficient.
 * An override-redirect a GLX window that cannot decorate, move, resize, or restack it.
 * Then run a GPU Shader on that window. The screen is 5 pixel oversize on all sides.
 * Run as a daemon from .xinitrc or equivalent of you WM to act as your wallpaper.
 * Written By: Boltzkreig
 */

#define GL_GLEXT_PROTOTYPES
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

// Oversized wallpaper
#define BLEED 5
// 1_000_000 us / 15 frames = 15 fps
#define NEW_WALLPAPER_DELAY 166667


// ---- Shaders --------------------------------------------------------

static const char *VS =
"#version 120\n"
"attribute vec2 a_position;\n"
"void main() { gl_Position = vec4(a_position, 0.0, 1.0); }\n";

static const char *FS =
"#version 120\n"
"uniform vec2  u_resolution;\n"
"uniform float u_time;\n"
"\n"
"float hash(vec2 p) {\n"
"    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);\n"
"}\n"
"float noise(vec2 p) {\n"
"    vec2 i = floor(p); vec2 f = fract(p);\n"
"    f = f * f * (3.0 - 2.0 * f);\n"
"    return mix(mix(hash(i),              hash(i + vec2(1, 0)), f.x),\n"
"               mix(hash(i + vec2(0, 1)), hash(i + vec2(1, 1)), f.x), f.y);\n"
"}\n"
"float fbm(vec2 p) {\n"
"    float v = 0.0; float a = 0.5;\n"
"    for (int i = 0; i < 4; i++) { v += a * noise(p); p *= 2.0; a *= 0.5; }\n"
"    return v;\n"
"}\n"
"\n"
"void main() {\n"
"    vec2 uv = gl_FragCoord.xy / u_resolution * 2.0;\n"
"    float t = u_time * 0.0075;\n"
"    vec2 q = vec2(fbm(uv + t),\n"
"                  fbm(uv + vec2(5.2, 1.3)));\n"
"    vec2 r = vec2(fbm(uv + 4.0 * q + vec2(1.7, 9.2) + t),\n"
"                  fbm(uv + 4.0 * q + vec2(8.3, 2.8) - t));\n"
"    float f = fbm(uv + 20.0 * r);\n"
"    vec3 col = mix(vec3(0.65, 0.3, 0.45), vec3(0.35,0.05,0.85), f);\n"
"    col = mix(col, vec3(0.22, 0.1, 0.37), length(q));\n"
"    col = mix(col, vec3(0.0, 0.0, 0.0), pow(f, 4.0));\n"
"    gl_FragColor = vec4(col, 1.0);\n"
"}\n";

// ---- Helpers --------------------------------------------------------

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), NULL, log);
        fprintf(stderr, "Shader compile error:\n%s\n", log);
        exit(EXIT_FAILURE);
    }
    return s;
}

static volatile sig_atomic_t g_quit = 0;
static void on_signal(int sig) { (void)sig; g_quit = 1; }

// ---- Main -----------------------------------------------------------

int main(void) {
    signal(SIGTERM, on_signal);
    signal(SIGINT,  on_signal);

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display ($DISPLAY=%s)\n", getenv("DISPLAY"));
        return EXIT_FAILURE;
    }

    int screen  = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);
    int sw      = DisplayWidth(dpy,  screen);
    int sh      = DisplayHeight(dpy, screen);

    // 5px bleed past every screen edge
    int x       = -BLEED;
    int y       = -BLEED;
    int W       = sw + 2 * BLEED;
    int H       = sh + 2 * BLEED;

    int glAttribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };
    XVisualInfo *vi = glXChooseVisual(dpy, screen, glAttribs);
    if (!vi) {
        fprintf(stderr, "No suitable GLX visual\n");
        return EXIT_FAILURE;
    }

    XSetWindowAttributes swa;
    swa.colormap          = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.background_pixmap = None;
    swa.border_pixel      = 0;                  // no X11 border
    swa.event_mask        = ExposureMask | StructureNotifyMask;
    swa.override_redirect = True;               // bypass the WM entirely

    Window win = XCreateWindow(
        dpy, root, x, y, W, H,
        0,                                      // border_width = 0
        vi->depth, InputOutput, vi->visual,
        CWColormap | CWBackPixmap | CWBorderPixel | CWEventMask | CWOverrideRedirect,
        &swa);

    XStoreName(dpy, win, "Live_Wallpaper");
    XMapWindow(dpy, win);
    XLowerWindow(dpy, win);                     // WM won't stack us; do it ourselves

    GLXContext ctx = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    if (!ctx) {
        fprintf(stderr, "Failed to create GLX context\n");
        return EXIT_FAILURE;
    }
    glXMakeCurrent(dpy, win, ctx);

    // -- Compile, link, set up quad --
    GLuint prog = glCreateProgram();
    glAttachShader(prog, compile_shader(GL_VERTEX_SHADER,   VS));
    glAttachShader(prog, compile_shader(GL_FRAGMENT_SHADER, FS));
    glLinkProgram(prog);
    GLint linked = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[2048];
        glGetProgramInfoLog(prog, sizeof(log), NULL, log);
        fprintf(stderr, "Link error:\n%s\n", log);
        return EXIT_FAILURE;
    }

    GLfloat verts[] = {-1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    GLint aPos  = glGetAttribLocation (prog, "a_position");
    GLint uRes  = glGetUniformLocation(prog, "u_resolution");
    GLint uTime = glGetUniformLocation(prog, "u_time");
    glEnableVertexAttribArray(aPos);
    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glUseProgram(prog);

    // -- Main loop --
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (!g_quit) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == ConfigureNotify) {
                W = ev.xconfigure.width;
                H = ev.xconfigure.height;
            }
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        float t = (now.tv_sec  - start.tv_sec)
                + (now.tv_nsec - start.tv_nsec) * 1e-9f;

        glViewport(0, 0, W, H);
        glUniform2f(uRes, (float)W, (float)H);
        glUniform1f(uTime, t);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glXSwapBuffers(dpy, win);

        usleep(NEW_WALLPAPER_DELAY);
    }

    glXMakeCurrent(dpy, None, NULL);
    glXDestroyContext(dpy, ctx);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return EXIT_SUCCESS;
}

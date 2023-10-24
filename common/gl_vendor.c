/*
  gl_vendor

  Gives the name of the vendor of the currently active display driver for GLX
  context purposes. This allows getting the vendor info without having to
  install all of `mesa-utils` just to grep `glxinfo` from a snap.

  This mostly just sets up a basic X context and queries the vendor via openGL,
  which is quite verbose but functionally not much is going on.

  Needs to be linked with `-lGL -lGLX -lX11`
*/

#include <stdbool.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

int main() {
  Display *display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "ERROR: unable to open display\n");
    return -1;
  }

  int attrib_list[] = {GLX_RGBA, GLX_RED_SIZE,  1, GLX_GREEN_SIZE,
                       1,        GLX_BLUE_SIZE, 1, None};

  XVisualInfo *vis_info = glXChooseVisual(display, 0, attrib_list);
  if (vis_info == NULL) {
    fprintf(stderr, "ERROR: can't get RGB GLX visual\n");
  }

  unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  Window root = RootWindow(display, 0);
  XSetWindowAttributes window_attribs;
  window_attribs.background_pixel = 0;
  window_attribs.colormap =
      XCreateColormap(display, root, vis_info->visual, AllocNone);
  window_attribs.event_mask = StructureNotifyMask | ExposureMask;
  Window window =
      XCreateWindow(display, root, 0, 0, 100, 100, 0, vis_info->depth,
                    InputOutput, vis_info->visual, mask, &window_attribs);

  GLXContext context = glXCreateContext(display, vis_info, NULL, GL_TRUE);
  if (context == NULL) {
    fprintf(stderr, "ERROR: Cannot create GLX Context\n");
    XDestroyWindow(display, window);
    return 1;
  }

  if (glXMakeCurrent(display, window, context)) {
    const char *vendor = (const char *)glGetString(GL_VENDOR);
    printf("%s\n", vendor);
  } else {
    fprintf(stderr, "ERROR: Cannot make GLX Context current\n");
    return 1;
  }
}
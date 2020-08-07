//
// Created by namdq2k on 06/08/2020.
//
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "opengldef.h"


static EGLConfig eglConf;
static EGLSurface eglSurface;
static EGLContext eglCtx;
static EGLDisplay eglDisp;
int     setupEOffScreenGL(int w, int h)
{
    // EGL config attributes
    const EGLint confAttr[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,    // very important!
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,          // we will create a pixelbuffer surface
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,     // if you need the alpha channel
            EGL_DEPTH_SIZE, 16,    // if you need the depth buffer
            EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };

    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,              // very important!
            EGL_NONE
    };

    // surface attributes
    // the surface size is set to the input frame size
    const EGLint surfaceAttr[] = {
            EGL_WIDTH, w,
            EGL_HEIGHT, h,
            EGL_NONE
    };

    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;

    eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisp, &eglMajVers, &eglMinVers);

    LOGI("EGL init with version %d.%d", eglMajVers, eglMinVers);

    // choose the first config, i.e. best config
    eglChooseConfig(eglDisp, confAttr, &eglConf, 1, &numConfigs);

    eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);

    // create a pixelbuffer surface
    eglSurface = eglCreatePbufferSurface(eglDisp, eglConf, surfaceAttr);
    int error = eglMakeCurrent(eglDisp, eglSurface, eglSurface, eglCtx);
}

void shutdownOffscreenEGL() {
    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglSurface);
    eglTerminate(eglDisp);

    eglDisp = EGL_NO_DISPLAY;
    eglSurface = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;
}
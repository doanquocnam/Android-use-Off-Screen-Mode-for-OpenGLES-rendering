//
// Created by namdq2k on 05/08/2020.
//

#ifndef ANDROIDOPENGL_OPENGLDEF_H
#define ANDROIDOPENGL_OPENGLDEF_H

#include <android/log.h>

#define  OPENGL_LOG_TAG "OpenGLJNI"
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,    OPENGL_LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,       OPENGL_LOG_TAG, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,      OPENGL_LOG_TAG, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,       OPENGL_LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,      OPENGL_LOG_TAG, __VA_ARGS__)
#endif //ANDROIDOPENGL_OPENGLDEF_H

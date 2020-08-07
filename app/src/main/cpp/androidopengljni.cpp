#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>
#include <unistd.h>
#include <android/bitmap.h>
#include "opengldef.h"
#include "TextureSample.h"
#include "GLOffscreen.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <opencv2/opencv.hpp>
#include <vector>
JavaVM * g_vm = NULL;

struct NativeThread { ;

    jobject bitmap_obj;
    jobject cb_obj;
    pthread_t thr;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int32_t nsig;
    bool pause;
    bool running;
    std::vector<cv::Mat> vec_mat;
};


NativeThread native_thread;// = {0};

jint JNI_OnLoad(JavaVM * vm, void * reserved)
{
    JNIEnv * jenv;
    g_vm = vm;

    if(g_vm->GetEnv((void **)&jenv, JNI_VERSION_1_6) != JNI_OK)
    {
        LOGE("Error load JNI version 1.6");
        return -1;
    }
    memset(&native_thread, 0, sizeof(native_thread));
    return JNI_VERSION_1_6;
}



static void checkGlError(const char* op)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static void create_time_wait(timespec * ts, int ms)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_nsec += ms * 1000000;
    if(ts->tv_nsec >= 1000000000)
    {
        ts->tv_sec = 1;
        ts->tv_nsec -= 1000000000;
    }
}

void get_asset(AAssetManager * assetMgr, std::vector<cv::Mat>& vec_mat)
{
    AAssetDir * dir = AAssetManager_openDir(assetMgr, "home");
    const char * filename = NULL;
    while((filename = AAssetDir_getNextFileName(dir)) != NULL)
    {
        cv::Mat m;
        std::string asset_file_path = std::string("home/") + filename;

        AAsset * asset = AAssetManager_open(assetMgr, asset_file_path.c_str(), AASSET_MODE_STREAMING);
        long size = AAsset_getLength(asset);
        char * buffer = (char *)malloc((size_t)size);
        AAsset_read(asset, buffer, (size_t)size);
        std::vector<char>data(buffer, buffer + size);

        m = cv::imdecode(cv::Mat(data), CV_LOAD_IMAGE_UNCHANGED);
        if (m.empty())
        {
            __android_log_print(ANDROID_LOG_INFO, "AAssetManager", "Error read image");
        }
        else
        {
            if(m.channels() == 3) {
                cv::Mat m_rgba;
                cv::cvtColor(m, m_rgba, CV_BGR2RGBA);
                vec_mat.push_back(m_rgba);
            }
            else
            {
                vec_mat.push_back(m);
            }

        }
        free(buffer);
        AAsset_close(asset);
    }
}

void render()
{
    timespec ts;
    JNIEnv* jenv;
    void * buffer;
    AndroidBitmapInfo bitmapInfo;
    int32_t render_time = 0;

    texture_renderFrame();
    if (g_vm->AttachCurrentThread(&jenv, NULL)==JNI_OK)
    {
        AndroidBitmap_getInfo(jenv, native_thread.bitmap_obj, &bitmapInfo);
        if(AndroidBitmap_lockPixels(jenv, native_thread.bitmap_obj, &buffer)==JNI_OK) {
            jclass jclz;
            jmethodID jmid;
            glReadPixels(0, 0, bitmapInfo.width, bitmapInfo.height, GL_RGBA,
                    GL_UNSIGNED_BYTE, buffer);
            AndroidBitmap_unlockPixels(jenv, native_thread.bitmap_obj);
            jclz = jenv->GetObjectClass(native_thread.cb_obj);
            jmid = jenv->GetMethodID(jclz, "onCallbackOpenGL", "()V");
            if (jmid) {
                jenv->CallVoidMethod(native_thread.cb_obj, jmid);
            }
            if (jenv->ExceptionCheck())
                jenv->ExceptionClear();
        }
        g_vm->DetachCurrentThread();
    }
}
#define NANO_TICK 1000000000
#define MICRO_TICK 1000000
#define COUNT_TIME(start,end) ((end.tv_sec - start.tv_sec) * NANO_TICK + (end.tv_nsec - start.tv_nsec))

static void * render_thread(void * param)
{
    JNIEnv * jenv;
    srand((uint32_t)time(NULL));
    if(g_vm->AttachCurrentThread(&jenv, NULL) == JNI_OK)
    {
        timespec last_time, current_time;
        int32_t frame_count =  0;
        float time_per_frame = 1000.f /_VIDEO_FRAME_RATE;

        AndroidBitmapInfo bitmapInfo;
        AndroidBitmap_getInfo(jenv, native_thread.bitmap_obj, &bitmapInfo);
        setupEOffScreenGL(bitmapInfo.width, bitmapInfo.height);
        texture_setupGraphics(bitmapInfo.width, bitmapInfo.height, native_thread.vec_mat);
        g_vm->DetachCurrentThread();

        native_thread.running = true;
        clock_gettime(CLOCK_REALTIME, &last_time);
        while(native_thread.running)
        {
            timespec start_time, end_time;
            long wait_time, render_time;
            double count_time;


            clock_gettime(CLOCK_REALTIME, &start_time);
            render();
            clock_gettime(CLOCK_REALTIME, &end_time);

            if(native_thread.pause)
            {
                pthread_mutex_lock(&native_thread.mutex);
                while(!native_thread.nsig)
                    pthread_cond_wait(&native_thread.cond, &native_thread.mutex);
                pthread_mutex_unlock(&native_thread.mutex);
            }
            else
            {
                current_time = end_time;
                render_time = COUNT_TIME(start_time, end_time);
                wait_time = (long)(time_per_frame * MICRO_TICK - render_time);
                if(wait_time > 0)
                {
                    timespec wait_ts;
                    wait_ts.tv_sec = 0;
                    wait_ts.tv_nsec = wait_time;
                    nanosleep(&wait_ts, NULL);
                }
            }
            frame_count++;
            count_time = (double)COUNT_TIME(last_time, current_time) / NANO_TICK;
            if(count_time >= 0.5)
            {
                char strFPS[30];
                double fps = frame_count / count_time;
                frame_count = 0;
                last_time = current_time;
                sprintf(strFPS, "Render speed: %0.2lf(fps)", fps);
                if(g_vm->AttachCurrentThread(&jenv, NULL) == JNI_OK)
                {
                    jclass jclz = jenv->GetObjectClass(native_thread.cb_obj);
                    if(jclz) {
                        jmethodID jmid = jenv->GetMethodID(jclz, "onCallbackFPS", "(Ljava/lang/String;)V");
                        if(jmid)
                            jenv->CallVoidMethod(native_thread.cb_obj, jmid, jenv->NewStringUTF(strFPS));

                    }
                    if(jenv->ExceptionCheck())
                        jenv->ExceptionClear();
                    g_vm->DetachCurrentThread();
                }

            }


        }
        shutdownOffscreenEGL();
    }
    pthread_mutex_destroy(&native_thread.mutex);
    pthread_cond_destroy(&native_thread.cond);
    LOGI("Stop OpenGL off screen render");
    return NULL;
}


extern "C" JNIEXPORT void JNICALL Java_com_namdq_glesoffscreenrender_MainActivity_startOrResumeRender(JNIEnv * jenv, jobject obj, jobject bitmap, jobject assset)
{
    if(!native_thread.running)
    {
        if(native_thread.bitmap_obj)
            jenv->DeleteGlobalRef(native_thread.bitmap_obj);
        if(native_thread.cb_obj)
            jenv->DeleteGlobalRef(native_thread.cb_obj);

        get_asset(AAssetManager_fromJava(jenv, assset), native_thread.vec_mat);
        native_thread.bitmap_obj        = jenv->NewGlobalRef(bitmap);
        native_thread.cb_obj            = jenv->NewGlobalRef(obj);
        pthread_mutex_init(&native_thread.mutex, NULL);
        pthread_cond_init(&native_thread.cond, NULL);
        native_thread.nsig = 0;
        if(pthread_create(&native_thread.thr, NULL, render_thread, NULL))
        {
            jenv->DeleteGlobalRef(native_thread.bitmap_obj);
            jenv->DeleteGlobalRef(native_thread.cb_obj);
            pthread_mutex_destroy(&native_thread.mutex);
            pthread_cond_destroy(&native_thread.cond);
        }
    }
    else
    {
        if(native_thread.pause)
        {
            pthread_mutex_lock(&native_thread.mutex);
            if(native_thread.bitmap_obj)
                jenv->DeleteGlobalRef(native_thread.bitmap_obj);
            if(native_thread.cb_obj)
                jenv->DeleteGlobalRef(native_thread.cb_obj);

            native_thread.bitmap_obj        = jenv->NewGlobalRef(bitmap);
            native_thread.cb_obj            = jenv->NewGlobalRef(obj);

            native_thread.nsig = 1;
            native_thread.pause = false;
            pthread_cond_signal(&native_thread.cond);
            pthread_mutex_unlock(&native_thread.mutex);
        }
    }
}
extern "C" JNIEXPORT void JNICALL Java_com_namdq_glesoffscreenrender_MainActivity_pauseRender(JNIEnv * jenv, jobject)
{
    if(native_thread.running)
    {
        native_thread.pause = true;
        native_thread.nsig = 0;
    }
}
extern "C" JNIEXPORT void JNICALL Java_com_namdq_glesoffscreenrender_MainActivity_stopRender(JNIEnv * jenv, jobject)
{
    if(native_thread.running) {
        native_thread.running = false;
        if(native_thread.pause)
        {
            native_thread.pause = false;
            pthread_mutex_lock(&native_thread.mutex);
            native_thread.nsig = 1;
            pthread_cond_signal(&native_thread.cond);
            pthread_mutex_unlock(&native_thread.mutex);
        }
    }
}


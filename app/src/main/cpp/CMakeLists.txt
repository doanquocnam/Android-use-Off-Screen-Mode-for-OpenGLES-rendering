# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
link_directories(../jni/lib/${ANDROID_ABI})
include_directories(../jni/include)
add_library( # Sets the name of the library.
        gl2jni

        # Sets the library as a shared library.
        SHARED
        Texture.cpp
        GLOffscreen.cpp
        Matrix.cpp
        TextureSample.cpp
        # Provides a relative path to your source file(s).
        androidopengljni.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


IF(${ANDROID_ABI} MATCHES "armeabi-v7a" OR ${ANDROID_ABI}  MATCHES "arm64-v8a")
    target_link_libraries( # Specifies the target library.
            gl2jni

            EGL
            GLESv2
            opencv_highgui
            opencv_imgcodecs
            opencv_imgproc
            opencv_core
            gnustl_static
            jnigraphics
            tegra_hal
            jpeg-turbo
            android
            png
            z
            m
            dl
            c
            # Links the target library to the log library
            # included in the NDK.
            ${log-lib})
ELSEIF(${ANDROID_ABI} MATCHES "x86" OR ${ANDROID_ABI} MATCHES "x86_64")
    target_link_libraries( # Specifies the target library.
            gl2jni

            opencv_highgui
            opencv_imgcodecs
            opencv_imgproc
            opencv_core
            EGL
            GLESv2
            gnustl_static
            jnigraphics
            jpeg-turbo
            png
            android
            z
            m
            dl
            c
            # Links the target library to the log library
            # included in the NDK.
            ${log-lib})
ENDIF()
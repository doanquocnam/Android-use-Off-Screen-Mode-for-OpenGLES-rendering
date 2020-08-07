//
// Created by namdq2k on 06/08/2020.
//

#ifndef ANDROIDOPENGL_TEXTURE_H
#define ANDROIDOPENGL_TEXTURE_H
#include <GLES2/gl2.h>
#include <opencv2/opencv.hpp>
/**
 * \brief Loads a simple 3 x 3 static texture into OpenGL ES.
 * \return Returns the handle to the texture object.
 */
GLuint * loadSimpleTexture(const std::vector<cv::Mat>& mat);
#endif //ANDROIDOPENGL_TEXTURE_H

//
// Created by namdq2k on 06/08/2020.
//

#ifndef ANDROIDOPENGL_TEXTURESAMPLE_H
#define ANDROIDOPENGL_TEXTURESAMPLE_H

#include <opencv2/opencv.hpp>

bool texture_setupGraphics(int width, int height, const std::vector<cv::Mat>& mat);
void texture_renderFrame();
#endif //ANDROIDOPENGL_TEXTURESAMPLE_H

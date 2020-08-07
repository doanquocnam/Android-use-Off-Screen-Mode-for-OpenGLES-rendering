//
// Created by namdq2k on 06/08/2020.
//

#include <GLES2/gl2.h>
#include <opencv2/opencv.hpp>

/**
 * \brief Loads a simple 3 x 3 static texture into OpenGL ES.
 * \return Returns the handle to the texture object.
 */
/* [includeTextureDefinition] */
#include "Texture.h"

#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

GLuint * loadSimpleTexture(const std::vector<cv::Mat>& vec_mat)
{
    /* Texture Object Handle. */
    static GLuint textureId[6];
    int i;

    /* [includeTextureDefinition] */

    /* [placeTextureInMemory] */
    /* Use tightly packed data. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Generate a texture object. */
    glGenTextures(6, textureId);

    /* Activate a texture. */
    glActiveTexture(GL_TEXTURE0);

    /* Bind the texture object. */
    for(i = 0; i < 6; i++) {
        int idx = i % vec_mat.size();
        glBindTexture(GL_TEXTURE_2D, textureId[i]);

        /* Load the texture. */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vec_mat[idx].cols, vec_mat[idx].rows, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, vec_mat[idx].data);

        /* Set the filtering mode. */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return textureId;
}
/* [placeTextureInMemory] */

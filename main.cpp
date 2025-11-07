/*
 *  CSCI 441, Computer Graphics, Fall 2025
 *
 *  Project: Mid-Term Project
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with GLSL shaders and
 *      implement diffuse lighting.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2025
 *
 */

#include "MPEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

///*****************************************************************************
//
// Our main function
int main() {
    const auto MpEngine = new MPEngine();
    MpEngine->initialize();
    if (MpEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        MpEngine->run();
    }
    MpEngine->shutdown();
    delete MpEngine;
	return EXIT_SUCCESS;
}

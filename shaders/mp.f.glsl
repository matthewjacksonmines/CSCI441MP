#version 410 core

/**
 * ********************* Fragment Shader *********************
 *
 * Computer Graphics
 * CSCI441 - Fall 2025
 * Colorado School of Mines
 *
 * @author Santiago Hevia
 * @hero Marcos Rogelio De la Hoz
 */


// ------------------------ Varying inputs ------------------------

// Interpolated color for this fragment
layout(location = 0) in vec3 color;

// ------------------------ Outputs ------------------------

// Color to apply to this fragment
out vec4 fragColorOut;


void main() {

    // Passing the interpolated color through as output.
    fragColorOut = vec4(color, 1.0);
}
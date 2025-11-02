#version 410 core

/**
 * ********************* Vertex Shader *********************
 *
 * Computer Graphics
 * CSCI441 - Fall 2025
 * Colorado School of Mines
 *
 * @author Santiago Hevia
 * @hero Marcos Rogelio De la Hoz
 */


// ------------------------ Uniform inputs ------------------------

// The precomputed Model-View-Projection Matrix.
uniform mat4 mvpMatrix;

uniform mat4 modelMatrix;

uniform mat3 normalMatrix;

uniform vec3 lightDirection;

uniform vec3 lightColor;

// The material color for our vertex (& whole object).
uniform vec3 materialColor;

// Camera position for the viewing vector.
uniform vec3 cameraPos;


// ------------------------ Attribute inputs ------------------------

// The position of this specific vertex in object space.
layout(location = 0) in vec3 vPos;

in vec3 vertexNormal;

// ------------------------ Varying outputs ------------------------

// Color to apply to this vertex
layout(location = 0) out vec3 color;

void main() {

    // Transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // Transforming the vector position to world space.
    vec3 fragmentPos = vec3(modelMatrix * vec4(vPos, 1.0));

    // Diffuse Illumination
    vec3 L = normalize(-lightDirection);
    vec3 N = normalize(normalMatrix * vertexNormal);
    vec3 diffuse = lightColor * materialColor * max(dot(L, N), 0.0);

    // Specular Illumination
    vec3 R = reflect(-L, N);
    vec3 V = normalize(cameraPos - fragmentPos);
    float shininess = 32.0;
    vec3 specular = lightColor * materialColor * pow( max(dot(R, V), 0.0), shininess);

    // Ambient Illumination
    float ambientLight = 0.2;
    vec3 ambient = ambientLight * materialColor;

    // Phong Illumination Model
    color = diffuse + specular + ambient;
}
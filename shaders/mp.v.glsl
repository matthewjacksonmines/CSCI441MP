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


// ------------------------ Uniform inputs ------------------------|

// ··············· Matrix uniforms ···············|

// Precomputed Model-View-Projection Matrix
uniform mat4 mvpMatrix;
// World model matrix
uniform mat4 modelMatrix;
// Normal matrix
uniform mat3 normalMatrix;

// ··············· Material ···············|

// The material color for our vertex (& whole object).
uniform vec3 materialColor;

// ··············· Directional light ···············|

// Light direction vector
uniform vec3 directional_lightDirection;
// Color of the light
uniform vec3 directional_lightColor;

// ··············· Point light ···············|

// Point light position
uniform vec3 point_lightPosition;
// Point light color
uniform vec3 point_lightColor;

// ··············· Spotlight ···············|

// Spot light position
uniform vec3 spot_lightPosition;
// Spot light direction
uniform vec3 spot_lightDirection;
// Spot light color
uniform vec3 spot_lightColor;

// ··············· Other ···············|

// Camera position for the viewing vector.
uniform vec3 cameraPos;


// ------------------------ Attribute inputs ------------------------|

// The position of this specific vertex in object space.
layout(location = 0) in vec3 vPos;
// The normal vector of the specific vertex.
layout(location = 1) in vec3 vertexNormal;

// ------------------------ Varying outputs ------------------------|

// Color to apply to this vertex
out vec3 color;


void main() {

    // Transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // Transforming the vector position to world space.
    vec3 worldPosition = vec3(modelMatrix * vec4(vPos, 1.0));

    // Normal and viewing vectors.
    vec3 N = normalize(normalMatrix * vertexNormal);
    vec3 V = normalize(cameraPos - worldPosition);

    // Phong Reflectance coefficients
    const vec3 K_amb = vec3(0.2, 0.2, 0.2);
    const vec3 K_diff = vec3(0.9, 0.9, 0.9);
    const vec3 K_spec = vec3(0.7, 0.7, 0.7);

    // How much the material shines.
    const float shininess = 40.0;

    // ======================= DIRECTIONAL =======================|

    // Diffuse Illumination
    vec3 Ld = normalize(-directional_lightDirection);
    vec3 diffuseD = K_diff * directional_lightColor * materialColor * max(dot(Ld, N), 0.0);

    // Specular Illumination
    vec3 Rd = reflect(-Ld, N);
    vec3 specularD = K_spec * directional_lightColor * pow(max(dot(Rd, V), 0.0), shininess);

    // Color with diffuse and specular.
    vec3 I_d = diffuseD + specularD;

    // ========================= POINT =========================|

    // Point light attenuation coefficients (constant, linear and quadratic)
    const float point_const_atten = 1.0, point_linear_atten = 0.045, point_quad_atten = 0.0075;

    // Diffuse Illumination
    vec3 point_distanceVector = point_lightPosition - worldPosition;
    float pointDistance = length(point_distanceVector);
    vec3 Lp = point_distanceVector / pointDistance;
    vec3 diffuseP = K_diff * materialColor * point_lightColor * max(dot(Lp, N), 0.0);

    // Attenuation : (c + l*d + q*d^2)
    float point_atten = 1.0 / (point_const_atten +
                point_linear_atten * pointDistance +
                point_quad_atten * pointDistance * pointDistance);

    // Specular Illumination
    vec3 Rp = reflect(-Lp, N);
    vec3 specularP = K_spec * point_lightColor * pow(max(dot(Rp, V), 0.0), shininess);

    // Color with diffuse, specular and attenuation.
    vec3 I_p = (diffuseP + specularP) * point_atten;

    // ========================= SPOTLIGHT =========================|

    // Angular cutoff of the spotlight beam
    const float spot_innerCos = cos(radians(15.0)), spot_outerCos = cos(radians(25.0));
    // Spotlight attenuation coefficients (constant, linear and quadratic)
    const float spot_const_atten = 1.0, spot_linear_atten = 0.09, spot_quad_atten = 0.032;

    // Diffuse Illumination
    vec3 spot_distanceVector = spot_lightPosition - worldPosition;
    float spotDistance = length(spot_distanceVector);
    vec3 Ls = spot_distanceVector / spotDistance;
    vec3 diffuseS = K_diff * materialColor * spot_lightColor * max(dot(Ls, N), 0.0);

    // Attenuation : (c + l*d + q*d^2)
    float spot_atten = 1.0 / (spot_const_atten +
                spot_linear_atten * spotDistance +
                spot_quad_atten * spotDistance * spotDistance);

    // Specular Illumination
    float cosTheta = dot(normalize(-Ls), normalize(spot_lightDirection));
    float spotFactor = clamp((cosTheta - spot_outerCos) / (spot_innerCos - spot_outerCos), 0.0, 1.0);
    vec3 Rs = reflect(-Ls, N);
    vec3 specularS = K_spec * spot_lightColor * pow(max(dot(Rs, V), 0.0), shininess);

    // Color with diffuse, specular, attenuation and spot factor.
    vec3 I_s = (diffuseS + specularS) * spot_atten * spotFactor;


    // Ambient Illumination (applied only once for all lights)
    vec3 ambient = K_amb * materialColor;

    // Phong Illumination Model with all lights.
    color = I_d + I_p + I_s + ambient;
}
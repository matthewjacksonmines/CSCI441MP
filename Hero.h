//
// Created by s.H.f. on 11/1/25.
//

#ifndef MP_HERO_H
#define MP_HERO_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


class Hero {

public:

    virtual ~Hero() = default;

    /**
     * Hero Drawing function
     * Using the three components of the MVP matrix, model, view and projection, executes all
     * the drawing functions to create our hero.
     * @param modelMtx : Model matrix to go from object space to world space
     * @param viewMtx : View matrix to go from world space to view/eye/camera space.
     * @param projMtx : Projection matrix to go from view space to clip space.
     */
    virtual void drawHero( glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const = 0;


    /**
     * Uniform loction setter
     * Sets all the uniform location for the shader passed as parameters.
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    virtual void setProgramUniformLocations( GLuint shaderProgramHandle,
                                     GLint mvpMtxUniformLocation,
                                     GLint normalMtxUniformLocation,
                                     GLint materialColorUniformLocation ) = 0;

    /**
     * Blinking flag setter
     * @param blink : boolean to know if Daglas is closing his eyes.
     */
    virtual void setBlink(bool blink) = 0;

    /**
     * Blinking flag getter
     * @return boolean with the blink flag
     */
    virtual bool getBlink() = 0;

    /**
     * Left walking setter
     * @param walk : boolean, true if walking with left leg.
     */
    virtual void setWalkLeft(bool walk) = 0;

    /**
     * Right walking setter
     * @param walk : boolean, true if walking with right leg.
     */
    virtual void setWalkRight(bool walk) = 0;

    /**
     * Left walking getter
     * @return boolean with left-walking flag
     */
    virtual bool getWalkLeft() = 0;

    /**
     * Right walking getter
     * @return boolean with right-walking flag
     */
    virtual bool getWalkRight() = 0;

    /**
     * Stopped setter
     * @param stop : true if Daglas is not moving
     */
    virtual void setStop(bool stop) = 0;


private:

    /// Handle for the shader program, used when drawing Daglas.
    GLuint _shaderProgramHandle;

    /// Structure storing the uniform locations for the shader program.
    struct ShaderProgramUniformLocations {
        /// Location of the precomputed Model-View-Projection matrix.
        GLint mvpMtx;

        /// Location of the precomputed Normal matrix.
        GLint normalMtx;

        /// Location of the material color.
        GLint materialColor;

    } _shaderProgramUniformLocations;

};


#endif //MP_HERO_H

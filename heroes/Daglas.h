/**
 * Hero building header file : Daglas!
 *
 * Created by Santiago Hevia F.
 * on Thursday October 23rd 2025
 */

#ifndef MP_DAGLAS_H
#define MP_DAGLAS_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../Hero.h"

/**
 * Daglas Hero Class
 * Class containing all the necessary functions and variables to draw our hero.
 */
class Daglas : public Hero {

public:

    /**
     * Daglas constructor
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    Daglas( GLuint shaderProgramHandle,
            GLint mvpMtxUniformLocation,
            GLint normalMtxUniformLocation,
            GLint materialColorUniformLocation );

    /**
     * Daglas Drawing function
     * Using the three components of the MVP matrix, model, view and projection, executes all
     * the drawing functions to create our hero.
     * @param modelMtx : Model matrix to go from object space to world space
     * @param viewMtx : View matrix to go from world space to view/eye/camera space.
     * @param projMtx : Projection matrix to go from view space to clip space.
     */
    void drawHero(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const override;


    /**
     * Uniform location setter
     * Sets all the uniform location for the shader passed as parameters.
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    void setProgramUniformLocations( GLuint shaderProgramHandle,
                                     GLint mvpMtxUniformLocation,
                                     GLint normalMtxUniformLocation,
                                     GLint materialColorUniformLocation ) override;

    /**
     * Blinking flag setter
     * @param blink : boolean to know if Daglas is closing his eyes.
     */
    void setBlink(bool blink) override;

    /**
     * Blinking flag getter
     * @return boolean with the blink flag
     */
    bool getBlink() override;

    /**
     * Left walking setter
     * @param walk : boolean, true if walking with left leg.
     */
    void setWalkLeft(bool walk) override;

    /**
     * Right walking setter
     * @param walk : boolean, true if walking with right leg.
     */
    void setWalkRight(bool walk) override;

    /**
     * Left walking getter
     * @return boolean with left-walking flag
     */
    bool getWalkLeft() override;

    /**
     * Right walking getter
     * @return boolean with right-walking flag
     */
    bool getWalkRight() override;

    /**
     * Stopped setter
     * @param stop : true if Daglas is not moving
     */
    void setStop(bool stop) override;


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

    // ------------- DRAWING VARIABLES -------------

    // Body parts:
    const glm::vec3 _body;
    const glm::vec3 _shellBig;
    const glm::vec3 _shellSmall;
    const glm::vec3 _leg;
    const glm::vec3 _arm;
    const glm::vec3 _neck;
    const glm::vec3 _head;
    const glm::vec3 _cheek;
    const glm::vec3 _eye;
    const glm::vec3 _closedEye;
    const glm::vec3 _pupil;
    const glm::vec3 _mouth;
    const glm::vec3 _nose;

    // Colors:
    const glm::vec3 _green;
    const glm::vec3 _darkGreen;
    const glm::vec3 _beige;
    const glm::vec3 _white;
    const glm::vec3 _black;
    const glm::vec3 _red;

    // Animation flags
    bool _blink;
    bool _walkLeft;
    bool _walkRight;
    bool _stop = true;


    // --------------------------------- DRAWING FUNCTIONS ---------------------------------

    /// Drawing Daglas Body
    void _drawBody(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Shell
    void _drawShell(const bool big, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Legs
    void _drawLegs(const bool leftLeg, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Arms
    void _drawArms(const bool leftArm, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Neck
    void _drawNeck(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Head
    void _drawHead(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Cheeks
    void _drawCheeks(const bool leftCheek, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Eyes
    void _drawEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Closed eyes
    void _drawClosedEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Pupils
    void _drawPupils(const bool leftPupil, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Mouth
    void _drawMouth(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing Daglas Nose
    void _drawNose(const bool leftNose, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;


    /**
     * Matrix uniform processor
     * This function computes the MVP and normal matrices and sends them to the shader through the GPU.
     * @param modelMtx : Model matrix to go from object space to world space
     * @param viewMtx : View matrix to go from world space to view/eye/camera space.
     * @param projMtx : Projection matrix to go from view space to clip space.
     */
    void _computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

};

#endif //MP_DAGLAS_H

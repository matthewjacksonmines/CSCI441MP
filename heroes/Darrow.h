
#ifndef MP_DARROW_H
#define MP_DARROW_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <CSCI441/ModelLoader.hpp>
#include "../Hero.h"

/**
 *  Hero Class
 * Class containing all the necessary functions and variables to draw our hero.
 */
class Darrow : public Hero {

public:

    /**
     *  constructor
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    Darrow( GLuint shaderProgramHandle,
            GLint mvpMtxUniformLocation,
            GLint normalMtxUniformLocation,
            GLint materialColorUniformLocation );

    /**
     *  Drawing function
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
     * @param blink : boolean to know if  is closing his eyes.
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
     * @param stop : true if  is not moving
     */
    void setStop(bool stop) override;

    void setCheck(bool check);


private:
    
    /// Handle for the shader program, used when drawing .
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
    const glm::vec3 _leg;
    const glm::vec3 _arm;
    const glm::vec3 _hand;
    const glm::vec3 _head;
    const glm::vec3 _eye;
    const glm::vec3 _pupil;
    const glm::vec3 _mouth;
    const glm::vec3 _hairTop;
    const glm::vec3 _hairSides;
    const glm::vec3 _hairBack;

    // Colors:
    const glm::vec3 _skintone;
    const glm::vec3 _black;
    const glm::vec3 _red;
    const glm::vec3 _white;
    const glm::vec3 _gold;

    CSCI441::ModelLoader* _hair;
    bool _check = false;

    // Animation flags
    bool _walkLeft;
    bool _walkRight;
    bool _stop = true;


    // --------------------------------- DRAWING FUNCTIONS ---------------------------------

    /// Drawing  Body
    void _drawBody(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Legs
    void _drawLegs(const bool leftLeg, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Arms
    void _drawArms(const bool leftArm, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    // Drawing Hands
    void _drawHands(const bool leftArm, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Head
    void _drawHead(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Eyes
    void _drawEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Pupils
    void _drawPupils(const bool leftPupil, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    /// Drawing  Mouth
    void _drawMouth(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const;

    void _drawHairTop(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

    void _drawHairSides(const bool leftHair, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

    void _drawHairBack(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;



    /**
     * Matrix uniform processor
     * This function computes the MVP and normal matrices and sends them to the shader through the GPU.
     * @param modelMtx : Model matrix to go from object space to world space
     * @param viewMtx : View matrix to go from world space to view/eye/camera space.
     * @param projMtx : Projection matrix to go from view space to clip space.
     */
    void _computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

};

#endif //MP_DARROW_H

/**
 * Hero building header file : Petre
 *
 * Created by Matthew Jackson
 */


#ifndef MP_PETRE_H
#define MP_PETRE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "../Hero.h"

/**
 * Petre Hero Class
 * Class containing all the necessary functions and variables to draw our hero.
 */
class Petre : public Hero {
public:

    /**
     * Petre constructor
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    Petre(GLuint shaderProgramHandle,
          GLint mvpMtxUniformLocation,
          GLint normalMtxUniformLocation,
          GLint materialColorUniformLocation);

    /**
     * Petre Drawing function
     * Using the three components of the MVP matrix, model, view and projection, executes all
     * the drawing functions to create our hero.
     * @param modelMtx : Model matrix to go from object space to world space
     * @param viewMtx : View matrix to go from world space to view/eye/camera space.
     * @param projMtx : Projection matrix to go from view space to clip space.
     */
    void drawHero(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const override;

    /**
     * Uniform loction setter
     * Sets all the uniform location for the shader passed as parameters.
     * @param shaderProgramHandle : Handle for the shader program (vertex + fragment)
     * @param mvpMtxUniformLocation : Location of the MVP matrix uniform
     * @param normalMtxUniformLocation : Location of the normal matrix uniform
     * @param materialColorUniformLocation : Location of the material color uniform
     */
    void setProgramUniformLocations(GLuint shaderProgramHandle,
                                    GLint mvpMtxUniformLocation,
                                    GLint normalMtxUniformLocation,
                                    GLint materialColorUniformLocation) override;

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
     * @param stop : true if Petre is not moving
     */
    void setStop(bool stop) override;

private:

    GLuint _shaderProgramHandle;
    struct ShaderProgramUniformLocations {
        GLint mvpMtx;
        GLint normalMtx;
        GLint materialColor;
    } _shaderProgramUniformLocations;

    // Torso stack
    const glm::vec3 _torsoXZ;     // (width, UNUSED, depth)
    const float     _hPants;      // pants height
    const float     _hBelt;       // belt height
    const float     _hShirt;      // shirt height

    // Limbs/head/hair
    const glm::vec3 _leg;
    const glm::vec3 _arm;
    const glm::vec3 _head;
    const glm::vec3 _hair;

    // Colors
    const glm::vec3 _skin;
    const glm::vec3 _shirtWhite;
    const glm::vec3 _beltBlack;
    const glm::vec3 _pantsGreen;
    const glm::vec3 _hairBrown;
    const glm::vec3 _eyeWhite;
    const glm::vec3 _pupilBlack;
    const glm::vec3 _mouthDark;

    // Animation flags
    bool _blink;
    bool _walkLeft;
    bool _walkRight;
    bool _stop = true;

    // Draw helpers
    void _drawBox(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx,
                  glm::vec3 scale, glm::vec3 color) const;
    void _drawStack(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;
    void _drawLegs(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;
    void _drawArms(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;
    void _drawHeadAndHair(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;
    void _drawFace(glm::mat4 headCenterM, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

    void _computeAndSendMatrixUniforms(const glm::mat4& modelMtx,
                                       const glm::mat4& viewMtx,
                                       const glm::mat4& projMtx) const;
};

#endif // MP_PETRE_H

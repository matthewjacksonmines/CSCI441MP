/**
 *Hero building class : Petre
 *
 *Created by Matthew Jackson
*/

#include "Petre.h"
#include <glm/gtc/matrix_transform.hpp>
#include <CSCI441/objects.hpp>
#include <GLFW/glfw3.h> // (for time, to hide the animation in this implementation)


// -------------------------------- PUBLIC --------------------------------

Petre::Petre(GLuint shaderProgramHandle,
             GLint mvpMtxUniformLocation,
             GLint normalMtxUniformLocation,
             GLint materialColorUniformLocation
            ): 
            // Parts
            _torsoXZ(0.80f, 0.0f, 0.50f),
            _hPants(0.45f),
            _hBelt(0.06f),
            _hShirt(0.55f),
            _leg(0.18f, 0.45f, 0.18f),
            _arm(0.14f, 0.42f, 0.14f),
            _head(0.50f, 0.45f, 0.45f),
            _hair(0.50f, 0.08f, 0.45f),
            _skin(0.95f, 0.80f, 0.70f),

            // Colors
            _shirtWhite(1.0f, 1.0f, 1.0f),
            _beltBlack(0.0f, 0.0f, 0.0f),
            _pantsGreen(0.0f, 0.50f, 0.20f),
            _hairBrown(0.40f, 0.26f, 0.13f),
            _eyeWhite(1.0f, 1.0f, 1.0f),
            _pupilBlack(0.0f, 0.0f, 0.0f),
            _mouthDark(0.55f, 0.12f, 0.12f)
{
    setProgramUniformLocations(shaderProgramHandle, mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation);
}

void Petre::setProgramUniformLocations(GLuint shaderProgramHandle,
                                       GLint mvpMtxUniformLocation,
                                       GLint normalMtxUniformLocation,
                                       GLint materialColorUniformLocation) {
    _shaderProgramHandle = shaderProgramHandle;
    _shaderProgramUniformLocations = { mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation };
}

void Petre::drawHero(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, 0.0f));

    _drawLegs(modelMtx, viewMtx, projMtx);
    _drawStack(modelMtx, viewMtx, projMtx);
    _drawArms(modelMtx, viewMtx, projMtx);
    _drawHeadAndHair(modelMtx, viewMtx, projMtx); // Also draw face
}

// (unused)
void Petre::setBlink(bool blink) {_blink = blink;}
bool Petre::getBlink() {return _blink;}
void Petre::setWalkLeft(bool walk) { _walkLeft = walk; }
void Petre::setWalkRight(bool walk) { _walkRight = walk; }
bool Petre::getWalkLeft() { return _walkLeft; }
bool Petre::getWalkRight() { return _walkRight; }
void Petre::setStop(bool stop) { _stop = stop; }

// Wrapper function to draw a scaled cube/box
void Petre::_drawBox(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx,
                     glm::vec3 scale, glm::vec3 color) const {
    modelMtx = glm::scale(modelMtx, scale);
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &color[0]);
    CSCI441::drawSolidCube(1.0);
}

// Draw torso box stack
void Petre::_drawStack(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    const float yLeg = _leg.y;
    const float yPantsC = yLeg + _hPants*0.5f;
    const float yBeltC  = yLeg + _hPants + _hBelt*0.5f;
    const float yShirtC = yLeg + _hPants + _hBelt + _hShirt*0.5f;

    // Pants
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(0.0f, yPantsC, 0.0f));
        _drawBox(M, viewMtx, projMtx, glm::vec3(_torsoXZ.x, _hPants, _torsoXZ.z), _pantsGreen);
    }
    // Belt
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(0.0f, yBeltC, 0.0f));
        _drawBox(M, viewMtx, projMtx, glm::vec3(_torsoXZ.x, _hBelt, _torsoXZ.z), _beltBlack);
    }
    // Shirt
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(0.0f, yShirtC, 0.0f));
        _drawBox(M, viewMtx, projMtx, glm::vec3(_torsoXZ.x, _hShirt, _torsoXZ.z), _shirtWhite);
    }
}

void Petre::_drawLegs(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    const float xOff = 0.20f;
    // Left leg
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(-xOff, _leg.y*0.5f, 0.0f));
        _drawBox(M, viewMtx, projMtx, _leg, _skin);
    }
    // Right leg
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(+xOff, _leg.y*0.5f, 0.0f));
        _drawBox(M, viewMtx, projMtx, _leg, _skin);
    }
}

void Petre::_drawArms(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    const float yLeg = _leg.y;
    const float yMidShirt = yLeg + _hPants + _hBelt + _hShirt*0.5f;
    const float xOff = (_torsoXZ.x*0.5f) + (_arm.x*0.5f);

    float swingAngle = 0.0f;
    if(!_stop && (_walkLeft ^ _walkRight)) {
        const float t = static_cast<float>(glfwGetTime());
        const float A = glm::radians(28.0f);        // swinging amplitude in radians
        const float w = 10.0f;                      // speed
        const float swing = sinf(w*t);              // in [-1,1]
        swingAngle =  A*swing;    // opposite phase
    }

    // left arm
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(-xOff, yMidShirt, 0.0f));
        M = glm::rotate(M, swingAngle, glm::vec3(1, 0, 0));
        M = glm::translate(M, glm::vec3(0.0f, -_arm.y*0.5f, 0.0f));
        _drawBox(M, viewMtx, projMtx, _arm, _skin);
    }
    // right arm
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(+xOff, yMidShirt, 0.0f));
        M = glm::rotate(M, -swingAngle, glm::vec3(1,0,0));
        M = glm::translate(M, glm::vec3(0.0f, -_arm.y*0.5f, 0.0f));
        _drawBox(M, viewMtx, projMtx, _arm, _skin);
    }
}

void Petre::_drawHeadAndHair(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    const float yLeg = _leg.y;
    const float yHeadC = yLeg + _hPants + _hBelt + _hShirt + _head.y*0.5f;
    const float yHairC = yHeadC + _head.y*0.5f + _hair.y*0.5f;

    // Head
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(0.0f, yHeadC, 0.0f));
        _drawBox(M, viewMtx, projMtx, _head, _skin);
        _drawFace(M, viewMtx, projMtx);
    }
    // Hair
    {
        glm::mat4 M = glm::translate(modelMtx, glm::vec3(0.0f, yHairC, 0.0f));
        _drawBox(M, viewMtx, projMtx, _hair, _hairBrown);
    }
}

void Petre::_drawFace(glm::mat4 headCenterM, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    const float headHalfZ = _head.z*0.5f;
    const float zFlush = headHalfZ + 0.01f; // Stick out of the head a little bit
    const float zPupil = zFlush + 0.02f; // Stick out a bit further
    const float zMouth = zFlush;
    const float zNoseC = headHalfZ + 0.015f;

    const glm::vec3 eyeSize(0.12f, 0.08f, 0.02f);
    const glm::vec3 pupilSize(0.04f, 0.04f, 0.01f);
    const glm::vec3 noseSize(0.06f, 0.08f, 0.03f);
    const glm::vec3 mouthSize(0.22f, 0.04f, 0.02f);

    const float eyeY = 0.07f;
    const float eyeX = 0.10f;
    const float mouthY = -0.10f;

    // Left eye white
    { glm::mat4 M = glm::translate(headCenterM, {-eyeX, eyeY, zFlush});
      _drawBox(M, viewMtx, projMtx, eyeSize, _eyeWhite); }

    // Right eye white
    { glm::mat4 M = glm::translate(headCenterM, {+eyeX, eyeY, zFlush});
      _drawBox(M, viewMtx, projMtx, eyeSize, _eyeWhite); }

    // Left pupil
    { glm::mat4 M = glm::translate(headCenterM, {-eyeX, eyeY, zPupil});
      _drawBox(M, viewMtx, projMtx, pupilSize, _pupilBlack); }

    // Right pupil
    { glm::mat4 M = glm::translate(headCenterM, {+eyeX, eyeY, zPupil});
      _drawBox(M, viewMtx, projMtx, pupilSize, _pupilBlack); }

    // Nose (skin), slightly protruding
    { glm::mat4 M = glm::translate(headCenterM, {0.0f, 0.0f, zNoseC});
      _drawBox(M, viewMtx, projMtx, noseSize, _skin); }

    // Mouth
    { glm::mat4 M = glm::translate(headCenterM, {0.0f, mouthY, zMouth});
      _drawBox(M, viewMtx, projMtx, mouthSize, _mouthDark); }
}

void Petre::_computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {

    // Precomputing the Model-View-Projection matrix on the CPU.
    glm::mat4 mvpMtx = projMtx*viewMtx*modelMtx;
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    // Precomputing the normal matrix.
    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
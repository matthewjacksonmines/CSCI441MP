#include "Darrow.h"
#include <glm/gtc/matrix_transform.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/ModelLoader.hpp>
#include <CSCI441/ShaderProgram.hpp>

// -------------------------------- PUBLIC --------------------------------

Darrow::Darrow(const GLuint shaderProgramHandle, const GLint mvpMtxUniformLocation, const GLint normalMtxUniformLocation, const GLint materialColorUniformLocation):

               _body(0.6, 0.9, 0.3),
               _leg(0.25, 0.828, 0.25),
               _arm(0.2, 0.8, 0.2),
               _hand(0.18,0.3, 0.18),
               _head(0.3, 0.4, 0.3),
               _eye(0.06, 0.05, 0.01),
               _pupil(0.03, 0.025, 0.01),
               _mouth(0.15, 0.02, 0.01),
               _hairTop(0.3, 0.01, 0.3),
               _hairSides(0.01, 0.4, 0.3),
               _hairBack(0.3, 0.6, 0.01),

               _skintone(1.0, 0.878, 0.741),
               _gold(0.871, 0.714, 0.294),
               _black(0.0, 0.0, 0.0),
               _red(0.851, 0.137, 0.137),
               _white(1.0, 1.0, 1.0)

{

    setProgramUniformLocations(shaderProgramHandle, mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation);
}

void Darrow::drawHero(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, 0.4f, 0.0f));

    _drawBody(modelMtx, viewMtx, projMtx);
    _drawLegs(true, modelMtx, viewMtx, projMtx);
    _drawLegs(false, modelMtx, viewMtx, projMtx);
    _drawArms(true, modelMtx, viewMtx, projMtx);
    _drawArms(false, modelMtx, viewMtx, projMtx);
    _drawHands(true, modelMtx, viewMtx, projMtx);
    _drawHands(false, modelMtx, viewMtx, projMtx);
    _drawHead(modelMtx, viewMtx, projMtx);
    _drawEyes(true, modelMtx, viewMtx, projMtx);
    _drawEyes(false, modelMtx, viewMtx, projMtx);
    _drawPupils(true, modelMtx, viewMtx, projMtx);
    _drawPupils(false, modelMtx, viewMtx, projMtx);
    _drawMouth(modelMtx, viewMtx, projMtx);
    _drawHairTop(modelMtx, viewMtx, projMtx);
    _drawHairSides(true, modelMtx, viewMtx, projMtx);
    _drawHairSides(false, modelMtx, viewMtx, projMtx);
    _drawHairBack(modelMtx, viewMtx, projMtx);

}

void Darrow::setProgramUniformLocations( GLuint shaderProgramHandle,
                                 GLint mvpMtxUniformLocation,
                                 GLint normalMtxUniformLocation,
                                 GLint materialColorUniformLocation ) {
    _shaderProgramHandle = shaderProgramHandle;
    _shaderProgramUniformLocations = {mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation};
}

void Darrow::setWalkLeft( bool walk) {
    _walkLeft = walk;
}

void Darrow::setWalkRight(bool walk) {
    _walkRight = walk;
}

bool Darrow::getWalkLeft() {
    return _walkLeft;
}

bool Darrow::getWalkRight() {
    return _walkRight;
}

void Darrow::setStop(bool stop) {
    _stop = stop;
}

void Darrow::setBlink(bool blink) {
    return;
}

bool Darrow::getBlink() {
    return false;
}


// -------------------------------- PRIVATE --------------------------------


// DRAWING FUNCTIONS :

void Darrow::_drawBody(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_leg.y), 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _body );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawLegs(const bool leftLeg, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    if (_stop) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), 0.0f, 0.0f));
    }
    else if (_walkLeft) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), 0.0f, (leftLeg ? -0.1f : 0.1f)));
    }
    else if (_walkRight) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), 0.0f, (leftLeg ? 0.1f : -0.1f)));
    }
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _leg );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawArms(const bool leftArm, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftArm ? -((_body.x / 2.0f) + _arm.x / 2.0f) : ((_body.x / 2.0f) + _arm.x / 2.0f)), (_body.y / 2.0f) + (_leg.y / 2.0f), 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _arm );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawHands(const bool leftHand, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftHand ? -((_body.x / 2.0f) + _arm.x / 2.0f) : ((_body.x / 2.0f) + _arm.x / 2.0f)), (_body.y / 2.0f) + (_leg.y / 2.0f)- 0.1f - (_arm.y / 2.0f), 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _hand );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_skintone[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawHead(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_head.y / 2.0f) + (_body.y / 2.0f) + (_leg.y), 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _head );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_skintone[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftEye ? -_eye.x : _eye.x), (_eye.y / 2.0f) + (_body.y / 2.0f) + _leg.y + (_head.y / 2.0f), (_head.z / 2.0f) + (_eye.z / 2.0f)));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _eye );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_white[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawPupils(const bool leftPupil, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    GLfloat pupilHeight = (_pupil.y / 2.0f) + _body.y + 0.23;
    modelMtx = glm::translate( modelMtx, glm::vec3((leftPupil ? -_eye.x : _eye.x), _pupil.y + (_body.y / 2.0f) + _leg.y + (_head.y / 2.0f), (_head.z / 2.0f) + (_eye.z / 2.0f) + (_pupil.z / 2.0f)));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _pupil );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_gold[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawMouth(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_mouth.y / 2.0f) + (_body.y / 2.0f) + _leg.y + (_head.y / 4.0f), 0.15f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _mouth );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawHairTop(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // Top of hair
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, _leg.y + _body.y -.045, 0.0f));
    modelMtx = glm::scale( modelMtx, _hairTop );
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_gold[0]);
    CSCI441::drawSolidCube(1.0);
 
}

void Darrow::_drawHairSides(const bool leftHair, glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const{
        // Sides of hair
    modelMtx = glm::translate( modelMtx, glm::vec3(leftHair ? -((_head.x / 2.0f) + _hairSides.x / 2.0f) : ((_head.x / 2.0f) + _hairSides.x / 2.0f), _leg.y + _body.y - _hairSides.y*.6 - _hairTop.y, 0.0f));
    modelMtx = glm::scale( modelMtx, _hairSides );
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_gold[0]);
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_drawHairBack(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // Back of hair
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, _leg.y + _body.y - _hairBack.y*.55 - _hairTop.y*2, -(_head.z / 2.0f)));
    modelMtx = glm::scale( modelMtx, _hairBack );
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_gold[0]);
    CSCI441::drawSolidCube(1.0);
}

void Darrow::_computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // Precomputing the Model-View-Projection matrix on the CPU.
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );
    // Precomputing the normal matrix.
    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

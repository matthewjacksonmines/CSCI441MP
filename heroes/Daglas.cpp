/**
 * Hero building class : Daglas!
 *
 * Created by Santiago Hevia F.
 * on Thursday October 23rd 2025
 */

#include "Daglas.h"
#include <glm/gtc/matrix_transform.hpp>
#include <CSCI441/objects.hpp>


// -------------------------------- PUBLIC --------------------------------

Daglas::Daglas(const GLuint shaderProgramHandle,
               const GLint mvpMtxUniformLocation,
               const GLint normalMtxUniformLocation,
               const GLint materialColorUniformLocation
               ):

               // Parts
               _body({0.6, 0.9, 0.3}),
               _shellBig({0.7, 1.0, 0.1}),
               _shellSmall({0.6, 0.9, 0.1}),
               _leg(0.1, 0.4, 0.1),
               _arm(0.1, 0.7, 0.1),
               _neck(0.2, 0.3, 0.1),
               _head(0.6, 0.3, 0.3),
               _cheek(0.1, 0.2, 0.2),
               _eye(0.15, 0.15, 0.15),
               _closedEye(0.16, 0.16, 0.16),
               _pupil(0.1, 0.1, 0.1),
               _mouth(0.6, 0.1, 0.1),
               _nose(0.04, 0.04, 0.04),

               // Colors
               _green({0.31, 0.965, 0.133}),
               _darkGreen({0.0, 0.596, 0.0}),
               _beige(0.925, 0.898, 0.71),
               _white(1.0, 1.0, 1.0),
               _black(0.0, 0.0, 0.0),
               _red(0.851, 0.137, 0.137)

{

    setProgramUniformLocations(shaderProgramHandle, mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation);
}


void Daglas::drawHero(glm::mat4 modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx ) const {
    // Translating Daglas entirely to position him over the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, 0.4f, 0.0f));

    _drawBody(modelMtx, viewMtx, projMtx);

    _drawShell(true, modelMtx, viewMtx, projMtx);
    _drawShell(false, modelMtx, viewMtx, projMtx);

    _drawLegs(true, modelMtx, viewMtx, projMtx);
    _drawLegs(false, modelMtx, viewMtx, projMtx);

    _drawArms(true, modelMtx, viewMtx, projMtx);
    _drawArms(false, modelMtx, viewMtx, projMtx);

    _drawNeck(modelMtx, viewMtx, projMtx);

    _drawHead(modelMtx, viewMtx, projMtx);

    _drawCheeks(true, modelMtx, viewMtx, projMtx);
    _drawCheeks(false, modelMtx, viewMtx, projMtx);

    if (_blink) {
        _drawClosedEyes(true, modelMtx, viewMtx, projMtx);
        _drawClosedEyes(false, modelMtx, viewMtx, projMtx);
    } else {
        _drawEyes(true, modelMtx, viewMtx, projMtx);
        _drawEyes(false, modelMtx, viewMtx, projMtx);

        _drawPupils(true, modelMtx, viewMtx, projMtx);
        _drawPupils(false, modelMtx, viewMtx, projMtx);
    }

    _drawMouth(modelMtx, viewMtx, projMtx);

    _drawNose(true, modelMtx, viewMtx, projMtx);
    _drawNose(false, modelMtx, viewMtx, projMtx);

}

void Daglas::setProgramUniformLocations( GLuint shaderProgramHandle,
                                 GLint mvpMtxUniformLocation,
                                 GLint normalMtxUniformLocation,
                                 GLint materialColorUniformLocation ) {

    _shaderProgramHandle = shaderProgramHandle;
    _shaderProgramUniformLocations = {mvpMtxUniformLocation, normalMtxUniformLocation, materialColorUniformLocation};
}

void Daglas::setBlink(bool blink) {
    _blink = blink;
}

bool Daglas::getBlink() {
    return _blink;
}

void Daglas::setWalkLeft( bool walk) {
    _walkLeft = walk;
}

void Daglas::setWalkRight(bool walk) {
    _walkRight = walk;
}

bool Daglas::getWalkLeft() {
    return _walkLeft;
}

bool Daglas::getWalkRight() {
    return _walkRight;
}

void Daglas::setStop(bool stop) {
    _stop = stop;
}


// -------------------------------- PRIVATE --------------------------------


// DRAWING FUNCTIONS :

void Daglas::_drawBody(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, _body.y / 2.0f, 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _body );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawShell(const bool big, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, _body.y / 2.0f, (big ? -0.2f : -0.3f)));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, (big ? _shellBig : _shellSmall) );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_darkGreen[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawLegs(const bool leftLeg, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.

    if (_stop) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), -(_leg.y / 2.0f), 0.0f));
    }
    else if (_walkLeft) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), -(_leg.y / 2.0f), (leftLeg ? -0.1f : 0.1f)));
    }
    else if (_walkRight) {
        modelMtx = glm::translate( modelMtx, glm::vec3((leftLeg ? -0.15f : 0.15f), -(_leg.y / 2.0f), (leftLeg ? 0.1f : -0.1f)));
    }

    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _leg );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_beige[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawArms(const bool leftArm, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftArm ? -0.35f : 0.35f), _body.y / 2.0f, 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _arm );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawNeck(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_neck.y / 2.0f) + _body.y, 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _neck );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawHead(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_head.y / 2.0f) + _body.y + _neck.y, 0.0f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _head );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawCheeks(const bool leftCheek, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftCheek ? -0.35f : 0.35f), (_cheek.y / 2.0f) + _body.y + _neck.y, 0.05f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _cheek );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftEye ? -0.15f : 0.15f), (_eye.y / 2.0f) + _body.y + _neck.y + 0.2, 0.15f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _eye );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_white[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawClosedEyes(const bool leftEye, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3((leftEye ? -0.15f : 0.15f), (_closedEye.y / 2.0f) + _body.y + _neck.y + 0.2, 0.15f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _closedEye );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_green[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawPupils(const bool leftPupil, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    GLfloat pupilHeight = (_pupil.y / 2.0f) + _body.y + _neck.y + 0.23;
    modelMtx = glm::translate( modelMtx, glm::vec3((leftPupil ? -0.15f : 0.15f), pupilHeight, 0.2f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _pupil );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawMouth(glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, (_mouth.y / 2.0f) + _body.y + _neck.y + 0.05, 0.15f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _mouth );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_red[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}

void Daglas::_drawNose(const bool leftNose, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {
    // Adjusting with respect to the grid.
    GLfloat pupilHeight = (_nose.y / 2.0f) + _body.y + _neck.y + 0.17;
    modelMtx = glm::translate( modelMtx, glm::vec3((leftNose ? -0.03f : 0.03f), pupilHeight, 0.15f));
    // Scaling by the defined coordinates.
    modelMtx = glm::scale( modelMtx, _nose );
    // Shading and color
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, &_black[0]);
    // Drawing
    CSCI441::drawSolidCube(1.0);
}


void Daglas::_computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {

    // Precomputing the Model-View-Projection matrix on the CPU.
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    // Precomputing the normal matrix.
    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    // Sending it to the shader on the GPU to apply to every vertex.
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}

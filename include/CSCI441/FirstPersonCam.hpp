/**
 * FIRST PERSON CAMERA
 *
 * Computer Graphics - CSCI441
 * Colorado School of Mines
 *
 * @author Santiago Hevia F.
 */

#ifndef CSCI441_FIRSTPERSON_CAM_HPP
#define CSCI441_FIRSTPERSON_CAM_HPP

#include "PerspectiveCamera.hpp"

namespace CSCI441 {

    /**
     * @brief A camera that implements an Arcball (orbit) camera model.
     *
     * The First-person camera is positioned where our hero head is.
     * The camera's position and orientation are the only parameters that change,
     * and they depend on the hero movement, it cannot be moved directly by the user.
     *
     * - WASD should move the character and hence the camera.
     */
    class FirstPersonCam final : public CSCI441::PerspectiveCamera {
    public:
        /**
         * @brief Construct an First-person camera with the specified perspective projection.
         * @param aspectRatio aspect ratio of view plane (defaults to 1.0f)
         * @param fovy vertical field of view in degrees (defaults to 45.0f)
         * @param nearClipPlane near z clip plane (defaults to 0.001f)
         * @param farClipPlane far z clip plane (defaults to 1000.0f)
         */
        explicit FirstPersonCam(GLfloat aspectRatio = 1.0f,
                            GLfloat fovy = 45.0f,
                            GLfloat nearClipPlane = 0.001f,
                            GLfloat farClipPlane = 1000.0f);

        /// Copy assignment
        FirstPersonCam& operator = (const FirstPersonCam&) = default;

        /// Move constructor
        FirstPersonCam(FirstPersonCam&&) = default;

        /// Move assignment
        FirstPersonCam& operator = (FirstPersonCam&&) = default;

        /// Destructor
        ~FirstPersonCam() override = default;

        /**
         * @brief Recompute the camera position and orientation with coordinates, yaw and pitch.
         *
         * Computes the forward, right and up vectors from yaw and pitch and
         * updates the camera position, orientation, and view matrix.
         */
        void recomputeOrientation() override;

        /**
         * @brief updates the camera's position by the adding the camera's direction to the camera's position
         * @param movementFactor : distance factor to scale the movement step
         * @note internally sets the camera's view matrix
         */
        void moveForward(GLfloat movementFactor) override;

        /**
         * @brief updates the camera's position by the adding the camera's negative direction to the camera's position
         * @param movementFactor : distance factor to scale the movement step
         * @note internally sets the camera's view matrix
         */
        void moveBackward(GLfloat movementFactor) override;

        /**
         * @brief Sets the new position of the first-person camera and recomputes the orientation.
         * @param pos : new position to update.
         */
        void setPosition(const glm::vec3& pos) {
            position = pos;
            recomputeOrientation();
        }

        /**
         * @brief Sets the new yaw (theta) of the first-person camera and recomputes the orientation.
         * @param y : new yaw orientation.
         */
        void setYaw(GLfloat y) {
            yaw = y;
            recomputeOrientation();
        }

        /**
         * @brief Sets the new pitch (phi) of the first-person camera and recomputes the orientation.
         * @param p : new pitch orientation.
         */
        void setPitch(GLfloat p) {
            pitch = p;
            recomputeOrientation();
        }


    private:
        /// Coordinates where the camera and hero are
        glm::vec3 position {0.0f, 0.0f, 0.0f};
        /// Angle of orientation in the X-Axis
        GLfloat yaw {0.0f};
        /// Angle of orientation in the Y-Axis
        GLfloat pitch {0.0f};

        /// Updates the position and orientation and recalculates the view matrix
        void _updateFirstPersonCameraViewMatrix();
    };
}

// ---------------- Implementation ----------------

inline CSCI441::FirstPersonCam::FirstPersonCam(
        const GLfloat aspectRatio,
        const GLfloat fovy,
        const GLfloat nearClipPlane,
        const GLfloat farClipPlane
) : PerspectiveCamera(aspectRatio, fovy, nearClipPlane, farClipPlane) {
    recomputeOrientation();
}

inline void CSCI441::FirstPersonCam::recomputeOrientation() {

    // Computing the forward vector from yaw and pitch.
    glm::vec3 front;
    front.x = cos(pitch) * sin(yaw);
    front.y = sin(pitch);
    front.z = cos(pitch) * cos(yaw);
    mCameraDirection = glm::normalize(front);

    // Recomputing right and up
    glm::vec3 right = glm::normalize(glm::cross(mCameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, mCameraDirection));

    // Setting the position and up vector.
    mCameraPosition = position;
    mCameraUpVector = up;

    _updateFirstPersonCameraViewMatrix();
}


inline void CSCI441::FirstPersonCam::moveForward(const GLfloat movementFactor) {
    position += mCameraDirection * movementFactor;
    recomputeOrientation();
}

inline void CSCI441::FirstPersonCam::moveBackward(const GLfloat movementFactor) {
    position -= mCameraDirection * movementFactor;
    recomputeOrientation();
}


inline void CSCI441::FirstPersonCam::_updateFirstPersonCameraViewMatrix() {
    mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraDirection, mCameraUpVector);
}

#endif // CSCI441_FIRSTPERSON_CAM_HPP

/**
 * ARC-BALL CAMERA
 *
 * Computer Graphics - CSCI441
 * Colorado School of Mines
 *
 * @author Santiago Hevia F.
 */

#ifndef CSCI441_ARCBALL_CAM_HPP
#define CSCI441_ARCBALL_CAM_HPP

#include "PerspectiveCamera.hpp"

namespace CSCI441 {

    /**
     * @brief A camera that implements an Arcball (orbit) camera model.
     *
     * The Arcball camera orbits around a fixed target point at a given radius.
     * The camera's position is computed from spherical coordinates (theta, phi, radius),
     * and it always looks at the target point.
     *
     * - Mouse movement updates theta/phi (orbit angles).
     * - Scroll wheel updates radius (zoom).
     * - WASD should move the *character*, not the camera.
     */
    class ArcballCam final : public CSCI441::PerspectiveCamera {
    public:
        /**
         * @brief Construct an Arcball camera with the specified perspective projection.
         * @param aspectRatio aspect ratio of view plane (defaults to 1.0f)
         * @param fovy vertical field of view in degrees (defaults to 45.0f)
         * @param nearClipPlane near z clip plane (defaults to 0.001f)
         * @param farClipPlane far z clip plane (defaults to 1000.0f)
         */
        explicit ArcballCam(GLfloat aspectRatio = 1.0f,
                            GLfloat fovy = 45.0f,
                            GLfloat nearClipPlane = 0.001f,
                            GLfloat farClipPlane = 1000.0f);

        /// Copy constructor
        ArcballCam(const ArcballCam&) = default;

        /// Copy assignment
        ArcballCam& operator=(const ArcballCam&) = default;

        /// Move constructor
        ArcballCam(ArcballCam&&) = default;

        /// Move assignment
        ArcballCam& operator=(ArcballCam&&) = default;

        /// Destructor
        ~ArcballCam() override = default;

        /**
         * @brief Recompute the camera position and direction from spherical coordinates.
         *
         * Converts (theta, phi, radius) into Cartesian coordinates relative to the target point.
         * Updates the camera position, direction, and view matrix.
         */
        void recomputeOrientation() override;

        /**
         * @brief Set the target point the camera orbits around.
         * @param target new target point
         */
        void setTarget(const glm::vec3& target);

        /**
         * @brief updates the camera's position by the adding the camera's direction to the camera's position
         * @param movementFactor  distance factor to scale the movement step
         * @note internally sets the camera's view matrix
         */
        void moveForward(GLfloat movementFactor) override;

        /**
         * @brief updates the camera's position by the adding the camera's negative direction to the camera's position
         * @param movementFactor distance factor to scale the movement step
         * @note internally sets the camera's view matrix
         */
        void moveBackward(GLfloat movementFactor) override;

        /**
         * @brief Zoom the camera in/out by adjusting the radius.
         * @param delta amount to add to radius (negative = zoom in, positive = zoom out)
         */
        void zoom(GLfloat delta);

    private:
        /// Points the camera orbits around
        glm::vec3 mTarget {0.0f, 0.0f, 0.0f};
        /// Distance from the target
        GLfloat mRadius {10.0f};

        /// Updates the look-at point and recalculates the view matrix
        void _updateArcballCameraViewMatrix();
    };
}

// ---------------- Implementation ----------------

inline CSCI441::ArcballCam::ArcballCam(
        const GLfloat aspectRatio,
        const GLfloat fovy,
        const GLfloat nearClipPlane,
        const GLfloat farClipPlane
) : PerspectiveCamera(aspectRatio, fovy, nearClipPlane, farClipPlane) {
    recomputeOrientation();
}

inline void CSCI441::ArcballCam::recomputeOrientation() {

    // Compute camera position in Cartesian coordinates
    mCameraPosition.x = mTarget.x + mCameraRadius * glm::sin(mCameraTheta) * glm::sin(mCameraPhi);
    mCameraPosition.y = mTarget.y + mCameraRadius * glm::cos(mCameraPhi);
    mCameraPosition.z = mTarget.z + mCameraRadius * glm::cos(mCameraTheta) * glm::sin(mCameraPhi);

    // Direction is from camera to target
    mCameraDirection = glm::normalize(mTarget - mCameraPosition);

    _updateArcballCameraViewMatrix();
}

inline void CSCI441::ArcballCam::setTarget(const glm::vec3& target) {
    mTarget = target;
    recomputeOrientation();
}

inline void CSCI441::ArcballCam::moveForward(const GLfloat movementFactor) {
    // Zoom IN
    zoom(-movementFactor);
}

inline void CSCI441::ArcballCam::moveBackward(const GLfloat movementFactor) {
    // Zoom OUT
    zoom(movementFactor);
}

inline void CSCI441::ArcballCam::zoom(GLfloat delta) {
    mCameraRadius = glm::max(mCameraRadius + delta, 0.5f);
    recomputeOrientation();
}

inline void CSCI441::ArcballCam::_updateArcballCameraViewMatrix() {
    setLookAtPoint(mTarget);
    computeViewMatrix();
}

#endif // CSCI441_ARCBALL_CAM_HPP

#ifndef MP_ENGINE_H
#define MP_ENGINE_H

#include <CSCI441/Camera.hpp>
#include <CSCI441/ArcballCam.hpp>
#include <CSCI441/FreeCam.hpp>
#include <CSCI441/FirstPersonCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include "Hero.h"
#include "heroes/Daglas.h"
#include "heroes/Paco.h"

#include <vector>

/**
 * *********************** MP - The Alchemist's Guild ***********************
 * -------------------------------- HEADER FILE --------------------------------
 *
 * Computer Graphics
 * CSCI441 - Fall 2025
 * Colorado School of Mines
 *
 * @author Santiago Hevia
 * @hero Marcos Rogelio De la Hoz
 */

class MPEngine final : public CSCI441::OpenGLEngine {
public:

    // Constructor
    MPEngine();

    // Destructor
    ~MPEngine() override;

    // Running function
    void run() override;

    // Keyboard event handler
    void handleKeyEvent(GLint KEY, GLint ACTION);

    // Mouse event handler
    void handleMouseButtonEvent(GLint BUTTON, GLint ACTION);

    // Cursor event handler
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// Value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

    const int NUMBER_OF_HEROES = 4;

private:

    // ----- Setup functions -----
    void mSetupGLFW() override;
    void mSetupOpenGL() override;
    void mSetupShaders() override;
    void mSetupBuffers() override;
    void mSetupScene() override;

    // ----- Cleanup functions -----
    void mCleanupScene() override;
    void mCleanupBuffers() override;
    void mCleanupShaders() override;

    // Scene rendering
    void _renderScene(const glm::mat4& viewMtx, const glm::mat4& projMtx) const;

    // Scene update (animation and interaction)
    void _updateScene();

    /// Number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;

    /**
     * Boolean array tracking each key state
     * - If true, then the key is in a pressed or held down state.
     * - If false, then the key is in a released state and not being interacted with.
     */
    GLboolean _keys[NUM_KEYS] = {0};

    /// Last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// Current state of the left mouse button
    GLint _leftMouseButtonState;

    /// Current camera being used
    CSCI441::Camera* _camera;

    /// Arcball camera for each hero in our world
    CSCI441::ArcballCam* _arcballCam;

    /// Free camera to move around the world
    CSCI441::FreeCam* _freeCam;

    /// First person camera in picture-in-picture
    CSCI441::FirstPersonCam* _firstPersonCam;

    /**
     * Values to store the speed the camera can move/rotate.
     * x = forward/backward delta
     * y = rotational delta
     */
    glm::vec2 _cameraSpeed;

    /// First person camera enabling flag
    bool _enableFPC;

    /// First-person viewport render flag
    bool _firstPersonView;

    /// OUR HERO Models
    Hero* _daglas;
    Hero* _paco;

    /// Information for each hero
    struct HeroData {
        // The hero class instance.
        Hero* hero;
        // The hero position in the world.
        glm::vec3 heroPosition;
        // Current hero yaw along the X-axis.
        GLfloat heroYaw = 0.0f;
        // Current hero pitch along the Y-axis.
        GLfloat heroPitch = 0.0f;
        // Model matrix of hero with all transformations.
        glm::mat4 modelMatrix;
        // Time since last time hero blinked.
        double lastBlinkTime;
    };

    /// Current hero being controlled by the player.
    HeroData* _hero;

    /// Index of the hero being controlled from the list.
    GLint heroIndex = 0;

    /// List of heroes in our world.
    std::vector<HeroData> _heroes;

    /// Time since last walk, left or right leg
    double lastWalkTime;

    // Change to next hero
    void changeHero();

    // Change the current camera view
    void changeCamera();

    // Blinking animation
    void _blink(HeroData& hero);

    // Walking animation
    void _walk();

    // Grass animation
    void swayGrass();

    /// Size of the world (controls the ground size and locations of objects)
    static constexpr GLfloat WORLD_SIZE = 100.0f;

    /// VAO for our ground
    GLuint _groundVAO;

    /// Number of points that make up our ground object
    GLsizei _numGroundPoints;

    // Ground buffer creation
    void _createGroundBuffers();

    /// Grass drawing information
    struct GrassData {
        /// Fixed position matrix of the grass.
        glm::mat4 baseMatrix;
        /// Model matrix to translate and scale the grass.
        glm::mat4 modelMatrix;
        /// Color of the grass.
        glm::vec3 color;
    };

    /// Tree drawing information
    struct TreeData {
        /// Model matrix to translate and scale the trees.
        glm::mat4 modelMatrix;
        /// Color of the tree trunk.
        glm::vec3 trunkColor;
        /// Color of the tree leaves.
        glm::vec3 leavesColor;
        /// Thickness of the tree trunk.
        float trunkThickness;
    };

    /// List of grass to draw.
    std::vector<GrassData> _grass;

    /// List of trees to draw.
    std::vector<TreeData> _trees;

    // Environment generation
    void _generateEnvironment();

    // Grass drawing function
    void drawGrass(glm::mat4 modelMtx, glm::vec3 color, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const;

    // Tree drawing function
    void drawTree(const TreeData& tree, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const;

    /// Shader program that performs lighting
    CSCI441::ShaderProgram* _lightingShaderProgram ;   // the wrapper for our shader program

    /// Shader program uniform locations
    struct LightingShaderUniformLocations {
        /// Precomputed MVP matrix location
        GLint mvpMatrix;
        /// Material diffuse color location
        GLint materialColor;
        /// Normal matrix location
        GLint normalMatrix;
        /// Light direction location (for Phong Illumination)
        GLint lightDirection;
        /// Light color location
        GLint lightColor;
        /// Camera position location (for Specular Illumination)
        GLint cameraPosition;
        /// Model matrix location
        GLint modelMatrix;

    } _lightingShaderUniformLocations;

    /// Shader program attribute locations
    struct LightingShaderAttributeLocations {
        /// Vertex position location
        GLint vPos;
        /// Vertex normal location
        GLint vertexNormal;

    } _lightingShaderAttributeLocations;

    // Lighting setup
    void _setLightingParameters();

    // Matrix uniform computation and sending to the shader program.
    void _computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const;
};

// Keyboard callback
void mp_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );

// Cursor callback
void mp_engine_cursor_callback(GLFWwindow *window, double x, double y );

// Mouse callback
void mp_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif// MP_ENGINE_H

#include "MPEngine.h"

#include <CSCI441/ArcballCam.hpp>
#include <CSCI441/objects.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>

#include "Hero.h"
#include "heroes/Daglas.h"
#include "heroes/Paco.h"

/**
 * *********************** MP - The Alchemist's Guild ***********************
 *
 * Computer Graphics
 * CSCI441 - Fall 2025
 * Colorado School of Mines
 *
 * @author Santiago Hevia
 * @hero Marcos Rogelio De la Hoz
 */

//************************************************************************************
//================================= Helper Functions =================================
//************************************************************************************

/// Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand() {
    return static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
}

//************************************************************************************
//================================= Public Interface =================================
//************************************************************************************

/// Engine constructor
MPEngine::MPEngine()
    : CSCI441::OpenGLEngine(4, 1,
                            768, 576,
                            "MP : (._.)"),
      _mousePosition( {MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED} ),
      _leftMouseButtonState(GLFW_RELEASE),
      _arcballCam(nullptr),
      _freeCam(nullptr),
      _firstPersonCam(nullptr),
      _enableFPC(false),
      _cameraSpeed( {0.0f, 0.0f} ),
      _daglas(nullptr),
      _paco(nullptr),
      _groundVAO(0),
      _numGroundPoints(0),
      _lightingShaderProgram(nullptr),
      _lightingShaderUniformLocations( {-1, -1} ),
      _lightingShaderAttributeLocations( {-1} )
{
    for(auto& _key : _keys) _key = GL_FALSE;
}

/// Engine destructor
MPEngine::~MPEngine() {
    delete _arcballCam;
    delete _freeCam;
    delete _firstPersonCam;
    delete _daglas;
    delete _paco;
    delete _lightingShaderProgram;
}

/**
 * Keyboard callback handler
 * Function that handles the events triggered by the keyboard.
 * @param KEY : The keyboard key that triggered the event.
 * @param ACTION : The action that triggered the event (press, release...)
 */
void MPEngine::handleKeyEvent(const GLint KEY, const GLint ACTION) {

    if (KEY != GLFW_KEY_UNKNOWN)
        _keys[KEY] = ((ACTION == GLFW_PRESS) || (ACTION == GLFW_REPEAT));
    // If a key is pressed.
    if (ACTION == GLFW_PRESS) {
        switch( KEY ) {
            // Press Q / ESC : quit the program
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;
            // Press R : reload the shaders and lighting
            case GLFW_KEY_R:
                mReloadShaders();
                _setLightingParameters();
                // Give the hero the normal matrix location.
                _hero->hero -> setProgramUniformLocations(
                    _lightingShaderProgram->getShaderProgramHandle(),
                    _lightingShaderUniformLocations.mvpMatrix,
                    _lightingShaderUniformLocations.normalMatrix,
                    _lightingShaderUniformLocations.materialColor
                );
                break;
            // Press P : change the hero controlled by the user
            case GLFW_KEY_P:
                changeHero();
                break;
            // Press C : change the camera view
            case GLFW_KEY_C:
                changeCamera();
                break;
            // Press V : enable picture-in-picture first person camera
            case GLFW_KEY_V:
                _enableFPC = !_enableFPC;
                break;
                // Suppress CLion warning
            default: break;
        }
    }
}

/**
 * Mouse callback handler
 * Function that handles the events triggered by the mouse buttons.
 * @param BUTTON : Mouse button that triggered the event.
 * @param ACTION : Action in the mouse that triggered the event.
 */
void MPEngine::handleMouseButtonEvent(const GLint BUTTON, const GLint ACTION) {
    // Event for the left mouse button.
    if( BUTTON == GLFW_MOUSE_BUTTON_LEFT ) {
        // Update the left mouse button's state.
        _leftMouseButtonState = ACTION;
    }
}

/**
 * Cursor callback handler
 * Function that handles the events triggered by the mouse cursor.
 * @param currMousePosition : Current position of the mouse cursor on the screen.
 */
void MPEngine::handleCursorPositionEvent(const glm::vec2 currMousePosition) {
    // If mouse hasn't moved in the window, prevent camera from flipping out.
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    // If the left mouse button is being held down while the mouse is moving.
    if(_leftMouseButtonState == GLFW_PRESS) {
        // Rotate the camera by the distance the mouse moved.
        _camera->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                         (_mousePosition.y - currMousePosition.y) * 0.005f );
    }

    // Update the mouse position.
    _mousePosition = currMousePosition;
}

//********************************************************************************
//================================= Engine Setup =================================
//********************************************************************************

/**
 * GLFW Setup
 * Configures all GLFW options before rendering, like the callbacks.
 */
void MPEngine::mSetupGLFW() {
    CSCI441::OpenGLEngine::mSetupGLFW();

    // Setting our callbacks
    glfwSetKeyCallback(mpWindow, mp_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, mp_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, mp_engine_cursor_callback);
}

/**
 * OpenGL Setup
 * Configures OpenGL options before rendering.
 */
void MPEngine::mSetupOpenGL() {
    glEnable( GL_DEPTH_TEST );                          // Enabling depth testing
    glDepthFunc( GL_LESS );                             // Using less than depth test

    glEnable(GL_BLEND);                                 // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Using one minus blending equation

    glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );             // Clearing the frame buffer to gray
}

/**
 * Shaders Setup
 * Configures options and parameters used by the vertex and fragment shaders.
 * Reads the GLSL files and sets the uniform and attribute locations.
 */
void MPEngine::mSetupShaders() {
    // Shader program
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/mp.v.glsl", "shaders/mp.f.glsl" );

    // --------------------------------------------- UNIFORMS ---------------------------------------------|

    // ============ MATRICES ============|
    // MVP matrix
    _lightingShaderUniformLocations.mvpMatrix = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    // Model matrix
    _lightingShaderUniformLocations.modelMatrix = _lightingShaderProgram->getUniformLocation("modelMatrix");
    // Normal Matrix
    _lightingShaderUniformLocations.normalMatrix = _lightingShaderProgram->getUniformLocation("normalMatrix");

    // ============ MATERIAL ============|
    // Material Color
    _lightingShaderUniformLocations.materialColor = _lightingShaderProgram->getUniformLocation("materialColor");

    // ============ DIRECTIONAL LIGHT ============|

    // Directional light direction
    _lightingShaderUniformLocations.dir_lightDirection = _lightingShaderProgram->getUniformLocation("directional_lightDirection");
    // Directional light color
    _lightingShaderUniformLocations.dir_lightColor = _lightingShaderProgram->getUniformLocation("directional_lightColor");

    // ============ POINT LIGHT ============|

    // Point light position
    _lightingShaderUniformLocations.point_lightPosition = _lightingShaderProgram->getUniformLocation("point_lightPosition");
    // Point light Color
    _lightingShaderUniformLocations.point_lightColor = _lightingShaderProgram->getUniformLocation("point_lightColor");

    // ============ SPOTLIGHT ============|

    // Spotlight position
    _lightingShaderUniformLocations.spot_lightPosition = _lightingShaderProgram->getUniformLocation("spot_lightPosition");
    // Spotlight direction
    _lightingShaderUniformLocations.spot_lightDirection = _lightingShaderProgram->getUniformLocation("spot_lightDirection");
    // Spotlight color
    _lightingShaderUniformLocations.spot_lightColor = _lightingShaderProgram->getUniformLocation("spot_lightColor");


    // Camera Position
    _lightingShaderUniformLocations.cameraPosition = _lightingShaderProgram->getUniformLocation("cameraPos");


    // --------------------------------------------- ATTRIBUTES ---------------------------------------------|
    // Vertex Position
    _lightingShaderAttributeLocations.vPos = _lightingShaderProgram->getAttributeLocation("vPos");
    // Vertex Normal
    _lightingShaderAttributeLocations.vertexNormal = _lightingShaderProgram->getAttributeLocation("vertexNormal");

}

/**
 * Buffers Setup
 * Configures buffer parameters such as vertex positions, normal matrix and also generates the environment.
 */
void MPEngine::mSetupBuffers() {
    // Passing the vertex positions and vertex normal to our object library.
    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos, _lightingShaderAttributeLocations.vertexNormal);

    // Giving the hero the normal matrix location.
    _daglas = new Daglas(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mvpMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor);

    // Giving the hero the normal matrix location.
    _paco = new Paco(_lightingShaderProgram->getShaderProgramHandle(),
                         _lightingShaderUniformLocations.mvpMatrix,
                         _lightingShaderUniformLocations.normalMatrix,
                         _lightingShaderUniformLocations.materialColor);

    _createGroundBuffers();
    _generateEnvironment();
}

/**
 * Ground Buffers Creation
 * This function sets the ground position and normals, generates the VAO, VBO and IBO,
 * binds them, and enables the vertex position and normal attribute arrays.
 */
void MPEngine::_createGroundBuffers() {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };

    // Position and normals aligned with the positive Y axis (0, 1, 0)
    constexpr Vertex groundQuad[4] = {
            { {-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
            { { 1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
            { {-1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f} },
            { { 1.0f, 0.0f,  1.0f}, {0.0f, 1.0f, 0.0f} }
    };

    constexpr GLushort indices[4] = {0,1,2,3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2]; // 0 : VBO, 1 : IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vPos);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    glEnableVertexAttribArray(_lightingShaderAttributeLocations.vertexNormal);
    glVertexAttribPointer(_lightingShaderAttributeLocations.vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

/**
 * Environment Generation
 * Function that generates and stores randomly all the objects scattered along the world.
 * Using the grid dimensions and iterating over the positions, it generates random objects
 * and stores them into a vector to be drawn later.
 */
void MPEngine::_generateEnvironment() {
    //************************ Grid Parameters ****************************
    // Parameters to make up our grid size and spacing.
    constexpr GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    constexpr GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    constexpr GLfloat GRID_SPACING_WIDTH = 1.0f;
    constexpr GLfloat GRID_SPACING_LENGTH = 1.0f;
    // Precomputed parameters based on above.
    constexpr GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    constexpr GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    constexpr GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    constexpr GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //**********************************************************************

    srand( time(0) ); // seed our RNG

    // psych! everything's on a grid.
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {

            // ----------------------- GRASS GENERATION -----------------------
            if( i % 2 && j % 2 && getRand() < 0.05f ) {
                // Move to random position
                glm::mat4 positionMatrix = glm::translate( glm::mat4(1.0), glm::vec3(i, -0.35f, j) );
                // Fixed height
                GLdouble height = 10.0;
                // Scaling to grass size
                glm::mat4 scaleMatrix = glm::scale( glm::mat4(1.0), glm::vec3(1, height, 0.3) );
                // Computing the full model matrix
                glm::mat4 modelMatrix = positionMatrix * scaleMatrix;
                // Green color
                glm::vec3 color( 0.275, 0.839, 0.122 );

                // Storing grass properties
                GrassData newGrass = {modelMatrix, modelMatrix, color};
                _grass.emplace_back( newGrass );
            }

            // ----------------------- TREE GENERATION -----------------------
            if( i % 2 && j % 2 && getRand() < 0.02f ) {
                // Moving to random position
                glm::mat4 positionMatrix = glm::translate( glm::mat4(1.0), glm::vec3(j, 0.0f, i) );
                // Computing a random height
                GLdouble height = powf(getRand(), 2.5) * 10 + 10;
                // Scaling the tree size
                glm::mat4 scaleMatrix = glm::scale( glm::mat4(1.0), glm::vec3(1, height, 1) );
                // Computing the full model matrix
                glm::mat4 modelMatrix = scaleMatrix * positionMatrix;

                // Computing random colors
                // Trunk dark brown shades (around RGB(0.4, 0.25, 0.1))
                float trunkR = 0.3f + getRand() * 0.2f;
                float trunkG = 0.15f + getRand() * 0.15f;
                float trunkB = 0.05f + getRand() * 0.1f;
                glm::vec3 trunkColor(trunkR, trunkG, trunkB);

                // Leaves dark green shades (around RGB(0.0, 0.3–0.6, 0.0))
                float leafR = 0.0f + getRand() * 0.1f;
                float leafG = 0.3f + getRand() * 0.3f;
                float leafB = 0.0f + getRand() * 0.1f;
                glm::vec3 leavesColor(leafR, leafG, leafB);

                // Random trunk thickness
                float trunkThickness = 3.0f + getRand() * 1.0f;

                // Storing tree properties
                TreeData newTree = {modelMatrix, trunkColor, leavesColor, trunkThickness };
                _trees.emplace_back( newTree );
            }
        }
    }
}

/**
 * Scene Setup
 * Sets the arc-ball camera and hero parameters before rendering.
 * Camera position, radius, orientation and speed, and hero position.
 */
void MPEngine::mSetupScene() {
    // Initializing the different cameras
    _arcballCam = new CSCI441::ArcballCam();
    _freeCam = new CSCI441::FreeCam();
    _firstPersonCam = new CSCI441::FirstPersonCam();

    // Calculating the window aspect ratio
    GLint framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );
    float aspectRatio = static_cast<float> (framebufferWidth) / framebufferHeight;

    // Setting the actual window aspect ratio to all cameras.
    _arcballCam->setAspectRatio(aspectRatio);
    _freeCam->setAspectRatio(aspectRatio);
    _firstPersonCam->setAspectRatio(aspectRatio);

    // Initially using the arc-ball camera.
    _camera = _arcballCam;

    // Arc-ball camera orbit around world origin
    _arcballCam->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));

    // Positioning the free cam away from the grid.
    _freeCam->setPosition(glm::vec3(0.0f, 20.0f, 20.0f));
    _freeCam->recomputeOrientation();


    // Distance from the target
    _camera->setRadius(30.0f);
    // Horizontal angle
    _camera->setTheta(glm::radians(360.0f));
    // Vertical angle
    _camera->setPhi(glm::radians(60.0f));
    // Recalculating camera orientation.
    _camera->recomputeOrientation();
    // Camera moving speed
    _cameraSpeed = glm::vec2(0.25f, 0.02f);
    _setLightingParameters();

    // Data of the current hero being controlled:
    glm::mat4 heroModelMtx(1.0f);
    glm::vec3 initialPosition(0.0f, 0.0f, 0.0f);
    GLfloat yaw = 0.0f;
    GLfloat pitch = 0.0f;
    double blinkTime = 0.0;

    // Inserting the heroes into the list.
    _heroes.reserve(NUMBER_OF_HEROES);
    _heroes.push_back({_daglas, initialPosition, yaw, pitch, heroModelMtx, blinkTime});
    _heroes.push_back({_paco, initialPosition, yaw, pitch, heroModelMtx, blinkTime});

    // Initializing the position and size of the heroes.
    for (HeroData& hero : _heroes) {
        glm::mat4 modelMatrix(1.0f);
        glm::vec3 position = {hero.heroPosition.x + 5.0, hero.heroPosition.y, hero.heroPosition.z};
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f));
        hero.modelMatrix = modelMatrix;
    }

    // Getting the first hero from the list.
    _hero = &_heroes.front();
    _heroes[heroIndex].heroPosition = _hero->heroPosition;
}

/**
 * Lighting Setup
 * Configures the lighting parameters used by our shaders program, light direction and color.
 */
void MPEngine::_setLightingParameters() {

    // ------- Lighting uniforms -------

    // Directional light
    glm::vec3 dir_lightDirection = {-1, -1, -1}; // Normalized vector
    glm::vec3 dir_lightColor = {1.0, 1.0, 1.0}; // White light

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),   // Program handle
                        _lightingShaderUniformLocations.dir_lightDirection, // Uniform location
                        1,                                                  // Number of elements
                        glm::value_ptr(dir_lightDirection));             //  Start of the data

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),   // Program handle
                        _lightingShaderUniformLocations.dir_lightColor,     // Uniform location
                        1,                                                  // Number of elements
                        glm::value_ptr(dir_lightColor));                 // Start of the data

    // Point light
    sunPosition = {0.0f, 50.0f, 50.0f};
    glm::vec3 point_lightPosition = sunPosition; // Position of our sun
    glm::vec3 point_lightColor = {1, 0.882, 0.765}; // Orange light

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),    // Program handle
                        _lightingShaderUniformLocations.point_lightPosition, // Uniform location
                        1,                                                   // Number of elements
                        glm::value_ptr(point_lightPosition));             //  Start of the data

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),    // Program handle
                        _lightingShaderUniformLocations.point_lightColor,    // Uniform location
                        1,                                                   // Number of elements
                        glm::value_ptr(point_lightColor));                // Start of the data

    // Spotlight
    glm::vec3 spot_lightPosition = {0.0, 15.0, 0.0};
    glm::vec3 spot_lightDirection = {0, -1, 0};
    glm::vec3 spot_lightColor = {1, 0.8, 0.8};

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),    // Program handle
                        _lightingShaderUniformLocations.spot_lightPosition,  // Uniform location
                        1,                                                   // Number of elements
                        glm::value_ptr(spot_lightPosition));             //  Start of the data

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),    // Program handle
                        _lightingShaderUniformLocations.spot_lightDirection, // Uniform location
                        1,                                                   // Number of elements
                        glm::value_ptr(spot_lightDirection));             // Start of the data

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(),    // Program handle
                        _lightingShaderUniformLocations.spot_lightColor,     // Uniform location
                        1,                                                   // Number of elements
                        glm::value_ptr(spot_lightColor));                 // Start of the data

}

//**********************************************************************************
//================================= Engine Cleanup =================================
//**********************************************************************************

/**
 * Shader Cleanup
 * Like good programmers, deletes the lighting shaders to save memory.
 */
void MPEngine::mCleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
    _lightingShaderProgram = nullptr;
}

/**
 * Buffer Cleanup
 * Like good programmers, deletes the VAOs, VBOs and models to save memory.
 */
void MPEngine::mCleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );
    _groundVAO = 0;

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _daglas;
    _daglas = nullptr;

    delete _paco;
    _paco = nullptr;
}

/**
 * Scene Cleanup
 * Like good programmers, deletes the cameras to save memory.
 */
void MPEngine::mCleanupScene() {
    fprintf( stdout, "[INFO]: ...deleting camera..\n" );
    delete _arcballCam;
    delete _freeCam;
    delete _firstPersonCam;
    _arcballCam = nullptr;
    _freeCam = nullptr;
    _firstPersonCam = nullptr;
}

//***************************************************************************************
//============================ Rendering / Drawing Functions ============================
//                               where the magic happens !
//***************************************************************************************

/**
 * Scene Rendering
 * This function does all the drawing in our scene: grid, world and hero.
 * @param viewMtx : View matrix used by the drawing functions to go from world space to eye space.
 * @param projMtx : Projection matrix used by the drawing functions to go from eye space to clip space.
 */
void MPEngine::_renderScene(const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // use our lighting shader program
    _lightingShaderProgram->useProgram();

    /// ---------------------------- DRAWING GROUND ----------------------------
    // Drawing the ground plane
    const glm::mat4 groundModelMtx = glm::scale( glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    // Green ground made of grass.
    constexpr glm::vec3 groundColor(0.161f, 0.522f, 0.024f);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, groundColor);

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);

    /// ---------------------------- DRAWING WORLD ----------------------------

    drawSun(viewMtx, projMtx);

    // Drawing grass
    for( const GrassData& newGrass : _grass ) {
        drawGrass(newGrass.color, newGrass.modelMatrix, viewMtx, projMtx);
    }

    // Drawing trees
    /*for( const TreeData& newTree : _trees ) {
        drawTree(newTree, viewMtx, projMtx);
    }*/

    /// ---------------------------- DRAWING HEROES ----------------------------

    // Drawing our models, the heroes!!!
    for (int i = 0 ; i < _heroes.size() ; i++) {
        // Hiding the hero in first-person camera view
        if (_enableFPC && _firstPersonView && i == heroIndex) {
            continue;
        }
        _heroes[i].hero -> drawHero(_heroes[i].modelMatrix, viewMtx, projMtx);
        _heroes[i].hero -> setStop(true);
    }

}

/**
 * Sun Drawing
 * Helper function to draw a beautiful sun to light our world
 * It draws a big sphere
 * @param viewMtx : View matrix from the scene rendering.
 * @param projMtx : Projection matrix from the scene rendering.
 */
void MPEngine::drawSun(const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {

    // Sun sphere:
    glm::mat4 sunModelMtx = glm::translate(glm::mat4(1.0f), sunPosition);
    sunModelMtx = glm::scale(sunModelMtx, glm::vec3(5.0f));
    _computeAndSendMatrixUniforms(sunModelMtx, viewMtx, projMtx);
    glm::vec3 brightOrange(1.0f, 0.72f, 0.0f);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, brightOrange);
    CSCI441::drawSolidSphere(1.0f, 40, 40);

    // Bean in the +X axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(1, 0, 0),
             glm::vec3(0, 0, 1),
             glm::radians(-90.0f));

    // Bean in the -X axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(-1, 0, 0),
             glm::vec3(0, 0, 1),
             glm::radians(90.0f));

    // Bean in the +Y axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(0, 1, 0),
             glm::vec3(1, 0, 0),
             glm::radians(0.0f));

    // Bean in the -Y axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(0, -1, 0),
             glm::vec3(1, 0, 0),
             glm::radians(180.0f));

    // Bean in the +Z axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(0, 0, 1),
             glm::vec3(1, 0, 0),
             glm::radians(90.0f));

    // Bean in the -Z axis
    drawBeam(viewMtx, projMtx,
             glm::vec3(0, 0, -1),
             glm::vec3(1, 0, 0),
             glm::radians(-90.0f));

}

/**
 * Beam Drawing
 * @param viewMtx : View matrix from the scene rendering.
 * @param projMtx : Projection matrix from the scene rendering.
 * @param dirAxis : direction axis where the beam is facing.
 * @param rotAxis : rotation axis to orient the beam outwards.
 * @param rotAngle : rotation angle to rotate the cone.
 */
void MPEngine::drawBeam(const glm::mat4 &viewMtx, const glm::mat4 &projMtx,
                        glm::vec3 dirAxis, glm::vec3 rotAxis, float rotAngle) const {
    // Beam separation from the sphere.
    float beamOffset = 5.5;

    // Moving, rotating and scaling the cone.
    glm::mat4 beamModelMtx = glm::translate(glm::mat4(1.0f), sunPosition + dirAxis * beamOffset);
    beamModelMtx = glm::rotate(beamModelMtx, rotAngle, rotAxis);
    beamModelMtx = glm::scale(beamModelMtx, glm::vec3(2.0f, 15.0f, 2.0f));

    // Sending the cone to the shader and drawing.
    _computeAndSendMatrixUniforms(beamModelMtx, viewMtx, projMtx);
    glm::vec3 brightYellow(1.0f, 0.83f, 0.0f);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, brightYellow);
    CSCI441::drawSolidCone(0.5f, 0.3f, 20, 20);
}


/**
 * Grass Drawing
 * Helper function to draw the grass generated previously.
 * It draws three green cones close together.
 * @param color : color of the grass.
 * @param modelMtx : Model matrix to perform transformations.
 * @param viewMtx : View matrix from the scene rendering.
 * @param projMtx : Projection matrix from the scene rendering.
 */
void MPEngine::drawGrass(glm::vec3 color, glm::mat4 modelMtx, const glm::mat4 &viewMtx, const glm::mat4 &projMtx) const {

    // Three flat cones close together and the middle one is taller.
    for (int i = 0 ; i < 3 ; i++) {
        _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, color);
        float grassHeight = 0.15f;
        if (i == 1)
            grassHeight = 0.20f;
        CSCI441::drawSolidCone( 0.15f, grassHeight, 10, 10 );
        modelMtx = glm::translate(modelMtx, glm::vec3(0.15f, 0.0f, 0.0f));
    }

}

/**
 * Tree Drawing
 * Helper function to draw trees with a trunk and leaves.
 * It draws a cylinder trunk with random thickness and three cones stacked as leaves.
 * @param tree : TreeData structure storing model, color and thickness.
 * @param viewMtx : View matrix from the scene rendering.
 * @param projMtx : Projection matrix from the scene rendering.
 */
void MPEngine::drawTree(const TreeData& tree, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // Drawing the tree trunk
    glm::mat4 trunkMtx = tree.modelMatrix;
    trunkMtx = glm::scale(trunkMtx, glm::vec3(0.3f, 3.0f, 0.3f));
    _computeAndSendMatrixUniforms(trunkMtx, viewMtx, projMtx);
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, tree.trunkColor);
    CSCI441::drawSolidCylinder(tree.trunkThickness, tree.trunkThickness, 1.0f, 20, 20);

    // Drawing the tree leaves (three stacked cones)
    for(int i = 0; i < 3; i++) {
        glm::mat4 leavesMtx = tree.modelMatrix;
        // Moving up for trunk height and cone stack.
        leavesMtx = glm::translate(leavesMtx, glm::vec3(0.0f, 3.0f + i * 1.1f, 0.0f));
        // Making smaller cones as we go up.
        float scale = 2.5f - i * 0.5f;
        leavesMtx = glm::scale(leavesMtx, glm::vec3(scale, 2.0f, scale));
        _computeAndSendMatrixUniforms(leavesMtx, viewMtx, projMtx);
        _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.materialColor, tree.leavesColor);
        CSCI441::drawSolidCone(1.5f, 1.0f, 20, 20);
    }
}


/**
 * Scene Update
 * This function is where interaction and animation comes to life.
 * User interactions are implemented to allow to move the camera and hero.
 * Constant and moving animation is created based on current time.
 */
void MPEngine::_updateScene() {

    // ----------------------------- HERO PARAMETERS -----------------------------|

    // Current hero being controlled by the player.
    HeroData& currentHero = _heroes[heroIndex];
    glm::mat4 heroModelMatrix(1.0f);

    // Translating our model by its position.
    heroModelMatrix = glm::translate(heroModelMatrix, currentHero.heroPosition);

    // Rotating our model in the Y-axis by the hero yaw (theta) in the arc-ball camera.
    heroModelMatrix = glm::rotate(heroModelMatrix, currentHero.heroYaw, CSCI441::Y_AXIS );

    // Rotating our model in the X-axis by the hero pitch (phi) in the arc-ball camera.
    heroModelMatrix = glm::rotate(heroModelMatrix, currentHero.heroPitch, CSCI441::X_AXIS );

    // Scaling our model to make it bigger.
    heroModelMatrix = glm::scale(heroModelMatrix, glm::vec3(5.0f));

    // Setting the new model matrix each frame.
    currentHero.modelMatrix = heroModelMatrix;
    _heroes[heroIndex] = currentHero;


    // ----------------------------- CAMERA CONTROLS -----------------------------|

    // Press SPACE : zoom
    if( _keys[GLFW_KEY_SPACE] ) {
        // Press SHIFT + SPACE : zoom camera out
        if( _keys[GLFW_KEY_LEFT_SHIFT] || _keys[GLFW_KEY_RIGHT_SHIFT] ) {
            _camera->moveBackward(_cameraSpeed.x);
        }
        // Press SPACE : zoom camera in
        else {
            _camera->moveForward(_cameraSpeed.x);
        }
    }

    // Updating the first-person camera position by the hero position.
    glm::vec3 heroPos = _heroes[heroIndex].heroPosition;
    float yaw = _heroes[heroIndex].heroYaw;
    float pitch = _heroes[heroIndex].heroPitch;

    // Initial hero height.
    float heroHeight = 10.0f;

    // Placing the camera at the height of the hero head.
    glm::vec3 eyePosition = heroPos + glm::vec3(0.0f, heroHeight * 0.9f, 0.0f);

    // Setting the position and orientation.
    _firstPersonCam->setPosition(eyePosition);
    _firstPersonCam->setYaw(yaw);
    _firstPersonCam->setPitch(pitch);
    _firstPersonCam->recomputeOrientation();


    // ----------------------------- HERO CONTROLS -----------------------------|

    // Forward vector to move our hero.
    glm::vec3 forward = glm::vec3(sin(currentHero.heroYaw), 0.0f, cos(currentHero.heroYaw));

    // Press W / up-arrow : Move forward and walk :)
    if( _keys[GLFW_KEY_W] || _keys[GLFW_KEY_UP] ) {
        _heroes[heroIndex].heroPosition += forward * 0.2f;
        _walk();
    }
    // Press S / down-arrow : Move backward and walk, almost like moon-walking :o
    if( _keys[GLFW_KEY_S] || _keys[GLFW_KEY_DOWN] ) {
        _heroes[heroIndex].heroPosition -= forward * 0.2f;
        _walk();
    }
    // Press A / left-arrow : Turn left
    if( _keys[GLFW_KEY_A] || _keys[GLFW_KEY_LEFT] ) {
        currentHero.heroYaw += 0.05f;
    }
    // Press D / right-arrow : Turn right
    if( _keys[GLFW_KEY_D] || _keys[GLFW_KEY_RIGHT] ) {
        currentHero.heroYaw -= 0.05f;
    }


    // [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[ CONSTANT ANIMATIONS ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

    // [----------- blinking eyes ;-) -----------]
    // All heroes in the world blink.
    for(int i = 0 ; i < _heroes.size() ; i++) {
        _blink(_heroes[i]);
    }

    // [----------- grass swaying -----------]
    swayGrass();


    // Bound checking the hero position.
    _heroes[heroIndex].heroPosition.x = glm::clamp(_heroes[heroIndex].heroPosition.x, -WORLD_SIZE, WORLD_SIZE);
    _heroes[heroIndex].heroPosition.z = glm::clamp(_heroes[heroIndex].heroPosition.z, -WORLD_SIZE, WORLD_SIZE);

    // Changing the hero position.
    if (_camera == _arcballCam) {
        _arcballCam->setTarget(_heroes[heroIndex].heroPosition);
        _arcballCam->recomputeOrientation();
    }
}

/**
 * Change Hero
 * Allows to swap the character being controlled by the player.
 */
void MPEngine::changeHero() {

    // Saving the current position.
    _heroes[heroIndex].heroPosition = _heroes[heroIndex].heroPosition;

    // Incrementing the hero index to change the player.
    heroIndex += 1;
    if (heroIndex >= _heroes.size()) {
        heroIndex = 0;
    }

    // Setting the new hero and position.
    _hero = &_heroes[heroIndex];
    _heroes[heroIndex].heroPosition = _hero->heroPosition;
}

/**
 * Change camera
 * Moves from the arc-ball camera centered at the hero, to a free camera.
 */
void MPEngine::changeCamera() {
    if (_camera == _arcballCam)
        _camera = _freeCam;
    else
        _camera = _arcballCam;
}

/**
 * Blinking animation
 * Using the current time and last time blinked, sets the blink flag to change the hero rendering.
 */
void MPEngine::_blink(HeroData& h) {
    double currentTime = glfwGetTime();

    // Blink eyes every 3 seconds
    if (!h.hero -> getBlink() && currentTime - h.lastBlinkTime > 3.0) {
        // CLOSE EYE
        h.hero -> setBlink(true);
        h.lastBlinkTime = currentTime;
    }

    // Open eyes after 0.4 seconds
    if (h.hero -> getBlink() && currentTime - h.lastBlinkTime > 0.4) {
        // OPEN EYE
        h.hero -> setBlink(false);
        h.lastBlinkTime = currentTime;
    }
}

void MPEngine::swayGrass() {
    float time = glfwGetTime();
    for (GrassData& g : _grass) {
        // Grass oscillating angle using a sine wave.
        float angle = sin(time * 2.0f + g.baseMatrix[3].x + g.baseMatrix[3].z) * 5.0f;

        // Sway matrix that pivots around the bottom.
        glm::mat4 sway(1.0f);

        // Translating down so the base is at the origin.
        sway = glm::translate(sway, glm::vec3(0.0f, -1.0f, 0.0f)); // adjust -0.5f to match your blade height

        // Rotating the grass by the angle.
        sway = glm::rotate(sway, glm::radians(angle), glm::vec3(0,0,1));

        // Translating back up.
        sway = glm::translate(sway, glm::vec3(0.0f, 1.0f, 0.0f));

        // Updating the model matrix with the animation.
        g.modelMatrix = g.baseMatrix * sway;
    }
}

/**
 * Walking animation
 * Using booleans and time, changes the hero legs position each 0.3 seconds,
 * only when moving forwards or backwards.
 * Stop : when the hero is not moving
 * Walk-left : to move hero's left leg.
 * Walk-right : to move hero's right leg.
 */
void MPEngine::_walk() {
    double currentTime = glfwGetTime();
    _hero->hero -> setStop(false);

    if (!_hero->hero -> getWalkLeft() && currentTime - lastWalkTime > 0.3) {
        _hero->hero -> setWalkLeft(true);
        _hero->hero -> setWalkRight(false);
        lastWalkTime = currentTime;
    }

    if (!_hero->hero -> getWalkRight() && currentTime - lastWalkTime > 0.3) {
        _hero->hero -> setWalkRight(true);
        _hero->hero -> setWalkLeft(false);
        lastWalkTime = currentTime;
    }
}

/**
 * Running Drawing LOOP
 * Executing loop that performs the rendering and update of all this program.
 */
void MPEngine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(mpWindow) ) {	                // Checking if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				                // Working with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// Clearing the current color contents and depth buffer in the window

        // Get the size of our framebuffer. Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window. Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( mpWindow, &framebufferWidth, &framebufferHeight );

        // Updating the viewport - Telling OpenGL we want to render to the whole window.
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // Drawing everything to the window.
        _renderScene(_camera->getViewMatrix(), _camera->getProjectionMatrix());

        if (_enableFPC) {
            // Updating the viewport for the picture-in-picture first person camera.
            int pipWidth  = framebufferWidth / 4;
            int pipHeight = framebufferHeight / 4;
            int pipX = framebufferWidth - pipWidth - 10;
            int pipY = 10;
            glViewport(pipX, pipY, pipWidth, pipHeight);
            // Drawing everything to the small window, except the hero being controlled.
            _firstPersonView = true;
            _renderScene(_firstPersonCam->getViewMatrix(), _firstPersonCam->getProjectionMatrix());
        }
        // Flag to hide the controlled hero on the first-person viewport.
        _firstPersonView = false;

        _updateScene();

        glfwSwapBuffers(mpWindow);      // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();               // check for any events and signal to redraw screen
    }
}

//**********************************************************************************
//============================ Private Helper Functions ============================
//**********************************************************************************

/**
 * Matrix uniforms maker
 * Precomputes the MVP matrix, normal matrix, camera position and model matrix and sends them to the shader.
 * @param modelMtx : Current model matrix to compute the MVP matrix.
 * @param viewMtx : Current view matrix to compute the MVP matrix.
 * @param projMtx : Current projection matrix to compute the MVP matrix.
 */
void MPEngine::_computeAndSendMatrixUniforms(const glm::mat4& modelMtx, const glm::mat4& viewMtx, const glm::mat4& projMtx) const {
    // Precomputing and sending the MVP matrix.
    const glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    // Precomputing and sending the normal matrix.
    glm::mat3 normalMatrix = glm::mat3( glm::transpose( glm::inverse(modelMtx) ) );
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normalMatrix, normalMatrix);

    // Precomputing and sending the arc-ball camera position.
    glm::vec3 cameraPosition = _camera->getPosition();
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.cameraPosition, cameraPosition);

    // Sending the model matrix.
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.modelMatrix, modelMtx);

}

//*****************************************************************************
//================================= Callbacks =================================
//*****************************************************************************

/**
 * Keyboard Callback Function
 * Creates the engine from the window and calls the keyboard handler.
 */
void mp_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
   const auto engine = static_cast<MPEngine *>(glfwGetWindowUserPointer(window));

    // Passing the key and action through to the engine to the handling function.
    engine->handleKeyEvent(key, action);
}

/**
 * Cursor Callback Function
 * Creates the engine from the window and calls the cursor handler.
 */
void mp_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    const auto engine = static_cast<MPEngine *>(glfwGetWindowUserPointer(window));

    // Passing the key and action through to the engine to the handling function.
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

/**
 * Mouse Callback Function
 * Creates the engine from the window and calls the mouse handler.
 */
void mp_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    const auto engine = static_cast<MPEngine *>(glfwGetWindowUserPointer(window));

    // Passing the key and action through to the engine to the handling function.
    engine->handleMouseButtonEvent(button, action);
}

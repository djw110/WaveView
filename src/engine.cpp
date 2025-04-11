#include "engine.h"

enum state {start, play, over};
state screen;

// Colors
color originalFill, hoverFill, pressFill;

Engine::Engine(const shared_ptr<AudioData>& ad) : keys() {
    this->initWindow();
    this->initData(ad);
    this->initPlayback();
    this->initShaders();
    this->initShapes();

    originalFill = {1, 0, 0, 1};
    hoverFill.vec = originalFill.vec + vec4{0.5, 0.5, 0.5, 0};
    pressFill.vec = originalFill.vec - vec4{0.5, 0.5, 0.5, 0};
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag",  nullptr, "shape");

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms
    textShader.setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    size_t numBars = (2 * (numBins - 2)) + 1;
    cout << numBars << endl;
    cout << numBins << endl;
    color barFill = {1, 180/255.0f, 0/255.0f, 1};
    float barWidth = (width - (numBars - 1) * 25) / numBars + 1;
    float margin = barWidth;
    for (int i = 0; i < numBars; ++i){
        int x = margin + i * (barWidth + 20);
        int y = 100;
        bars.push_back(std::make_unique<Rect>(shapeShader, vec2{x, y}, vec2{barWidth, 0}, barFill));
    }
    hovered.resize(bars.size(), false);
}

void Engine::scaleUp(unique_ptr<Shape>& shape, float scale){
    shape->setSizeY(2.5 * scale);
    shape->setPosY(100 + (2.5 * scale / 2));
}

void Engine::initPlayback(){
    audioPlayback = std::make_shared<Playback>(dataHandler->getNormals());
}

void Engine::initData(const shared_ptr<AudioData>& ad){
    dataHandler = ad;
    frequencyHistory = dataHandler->getFreqs();
    numBins = frequencyHistory[0].size();
}

void Engine::processInput() {
    glfwPollEvents();

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE]){
        glfwSetWindowShouldClose(window, true);
        audioPlayback->stop();
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);

    if (keys[GLFW_KEY_S]){
        if(screen == start){
            screen = play;
            audioPlayback->start();
        }
    }

    if(screen == play){
    }

    // Mouse position is inverted because the origin of the window is in the top left corner
    MouseY = height - MouseY; // Invert y-axis of mouse position
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool mouseReleased = false;

    for(int i = 0; i < bars.size(); ++i){
        if(bars[i]->isOverlapping(vec2(MouseX, MouseY))){
            hovered[i] = true;
        }
        else{
            hovered[i] = false;
        }
    }

}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if(screen == play){
        size_t framesPlayed = audioPlayback->getSamplesPlayed() / dataHandler->getOverlap();
        size_t barIndex = numBins - 2;
        for(int i = 0; i < (barIndex); ++i){
            scaleUp(bars[i],frequencyHistory[framesPlayed][i] + 1);
            scaleUp(bars[bars.size()-1-i],frequencyHistory[framesPlayed][i] + 1);
        }
        scaleUp(bars[barIndex],frequencyHistory[framesPlayed][barIndex] + 1);
    }
    
}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to draw shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        case start: {
            string message = "Press s to start";
            this->fontRenderer->renderText(message, width/2 - (12 * message.length()), height/2, projection, 1, vec3{1, 1, 1});
            break;
        }
        case play: {
            shapeShader.use();
            for(unique_ptr<Shape>& c : bars){
                c->setUniforms();
                c->draw();
            }
            break;
        }
        case over: {
            string message = "You win!";
            this->fontRenderer->renderText(message, width/2 - (12 * message.length()), height/2, projection, 1, vec3{1, 1, 1});
            break;
        }
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}

GLenum Engine::glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        cout << error << " | " << file << " (" << line << ")" << endl;
    }
    return errorCode;
}
#define DR_WAV_IMPLEMENTATION
#define KISS_FFT_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include "engine.h"
#include "dr_wav.h"

using std::cout, std::endl, std::cerr, std::vector;

int main(int argc, char *argv[]) {

    std::unique_ptr<AudioData> aud = std::make_unique<AudioData>(argv[1]);
    aud->preProcess();
    aud->printVals();

    vector<float> norms = aud->getNormals();

    std::unique_ptr<Playback> pb = std::make_unique<Playback>(norms);
    pb->start();

    // Engine engine;

    // while (!engine.shouldClose()) {
    //     engine.processInput();
    //     engine.update();
    //     engine.render();
    // }

    pb->stop();
    // glfwTerminate();
    return 0;
}
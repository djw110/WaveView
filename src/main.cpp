#define DR_WAV_IMPLEMENTATION
#define KISS_FFT_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include "engine.h"
#include "dr_wav.h"

using std::cout, std::endl, std::cerr, std::vector;

int main(int argc, char *argv[]) {
    if(!argv[1]){
        cerr << "No file argument(s)" << endl;
        return 1;
    }

    std::unique_ptr<AudioData> aud;

    try{
        aud = std::make_unique<AudioData>(argv[1]);
    }
    catch(std::runtime_error& e){
        cerr << e.what() << endl;
        return 1;
    }
    aud->preProcess();

    vector<float> norms = aud->getNormals();

    std::unique_ptr<Playback> pb = std::make_unique<Playback>(norms);
    pb->start();

    Engine engine;

    while (!engine.shouldClose()) {
        engine.processInput();
        engine.update();
        engine.render();
    }

    glfwTerminate();
    pb->stop();
    return 0;
}
#define DR_WAV_IMPLEMENTATION
#define KISS_FFT_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include "engine.h"
#include "dr_wav.h"

using std::cout, std::endl, std::cerr, std::vector, std::unique_ptr, std::make_unique, std:: make_shared, std::shared_ptr;

int main(int argc, char *argv[]) {
    if(!argv[1]){
        cerr << "No file argument(s)" << endl;
        return 1;
    }

    unique_ptr<AudioData> aud;

    try{
        aud = make_unique<AudioData>(argv[1]);
    }
    catch(std::runtime_error& e){
        cerr << e.what() << endl;
        return 1;
    }
    aud->preProcess();

    vector<float> norms = aud->getNormals();
    shared_ptr<Playback> audioHandler = make_shared<Playback>(norms);
    unique_ptr<Engine> engine = make_unique<Engine>(audioHandler);

    while (!engine->shouldClose()) {
        engine->processInput();
        engine->update();
        engine->render();
    }

    glfwTerminate();
    return 0;
}
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

    shared_ptr<AudioData> aud;

    try{
        aud = make_shared<AudioData>(argv[1]);
    }
    catch(std::runtime_error& e){
        cerr << e.what() << endl;
        return 1;
    }

    aud->preProcess();
    //aud->printVals();
    
    unique_ptr<Engine> engine = make_unique<Engine>(aud);

    while (!engine->shouldClose()) {
        engine->processInput();
        engine->update();
        engine->render();
    }

    glfwTerminate();
    return 0;
}
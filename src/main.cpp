#define DR_WAV_IMPLEMENTATION
#define KISS_FFT_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include "engine.h"
#include "dr_wav.h"
extern "C" {
    #include "kissfft/kiss_fft.h"
    #include "kissfft/kiss_fftr.h"
}

using std::cout, std::endl, std::cerr, std::vector;



int main(int argc, char *argv[]) {

    Engine engine;

    std::unique_ptr<AudioEngine> aud = std::make_unique<AudioEngine>(argv[1]);
    aud->preProccess();
    aud->printVals();

    // while (!engine.shouldClose()) {
    //     engine.processInput();
    //     engine.update();
    //     engine.render();
    // }

    // glfwTerminate();
    return 0;
}
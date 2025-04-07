#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <vector>
#include <iostream>
#include <cmath>
#include "dr_wav.h"
extern "C" {
    #include "kissfft/kiss_fft.h"
    #include "kissfft/kiss_fftr.h"
}

using std::cout, std::endl, std::cerr, std::vector;

class AudioEngine {
    private:
    const size_t frameSize = 1024, overlap = 512;
    const float sampleRate  = 44100.0f;
    drwav wav;
    drwav_int32* pDecodedInterleavedPCMFrames;
    size_t totalSamples;

    vector<vector<float>> frequencyHist;
    vector<vector<float>> audioFrames;
    vector<float> bins = {0.0f, 100.0f, 1000.0f, 5000.0f, sampleRate / 2.0f};
    float maxMag = 0.0f;

    kiss_fft_cfg fftCfg = kiss_fft_alloc(frameSize, 0, NULL, NULL); // Configuration for the FFT

    void hannWindow(vector<drwav_int32>& samples);
    
    float getMag(const kiss_fft_cpx& value);

    public:
    AudioEngine(const char* fileName);

    ~AudioEngine();
    
    void preProccess();

    void printVals();

};


#endif //AUDIO_ENGINE_H
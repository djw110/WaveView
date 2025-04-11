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

class AudioData {
    private:
    const size_t frameSize = 1024, overlap = 512;
    size_t totalSamples;
    float sampleRate;
    float maxMag;

    drwav wav;

    vector<vector<float>> frequencyHist;
    vector<vector<float>> audioFrames;
    vector<float> bins = {0.0f, 50.0f, 126.0f, 316.0f, 794.0f, 1495.0f, 3000.0f, 6000.0f, 10000.0f, sampleRate / 2.0f};
    vector<float> normalSamples;

    kiss_fft_cfg fftCfg = kiss_fft_alloc(frameSize, 0, NULL, NULL); // Configuration for the FFT

    void hannWindow(vector<float>& samples);
    
    float getMag(const kiss_fft_cpx& value);

    public:
    AudioData(const char* fileName);

    ~AudioData();
    
    void preProcess();

    void printVals();

    vector<float> getNormals();

    size_t getOverlap(){
        return overlap;
    }

    vector<vector<float>>& getFreqs();

};


#endif //AUDIO_ENGINE_H
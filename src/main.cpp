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


int analyzeAudio(drwav& wav, vector<vector<float>>& outputHistory, size_t frameSize, size_t overlap, float samplingRate);

int main(int argc, char *argv[]) {

    const size_t frameSize = 1024, overlap = 512;
    const float samplingRate = 44100.0f;

    //Read audio data into object
    drwav wav;
    if (!drwav_init_file(&wav, argv[1], NULL)) {
        cerr << "File not found" << endl;
        return 1;
    }
    else {
        cout << "Using file: " << argv[1] << endl;
    }

    //Will contain an array of frequency bins for every frame in the sample
    vector<vector<float>> frequencyHistory;
    analyzeAudio(wav, frequencyHistory, frameSize, overlap, samplingRate);

    drwav_uninit(&wav);

    Engine engine;
    engine.preProcessFreqs(frequencyHistory);

    while (!engine.shouldClose()) {
        engine.processInput();
        engine.update();
        engine.render();
    }

    glfwTerminate();
    return 0;
}

void hannWindow(drwav_int32* samples, size_t frameSize){
    for (size_t n = 0; n < frameSize; ++n){
        double hannValue = 0.5 * (1 - cos(2 * M_PI * n / (frameSize - 1)));
        samples[n] = static_cast<drwav_int32>(samples[n] * hannValue);
    }
}

float getMag(const kiss_fft_cpx& value) {
    return sqrt(value.r * value.r + value.i * value.i);
}

int analyzeAudio(drwav& wav, vector<vector<float>>& outputHistory, size_t frameSize, size_t overlap, float samplingRate){

    vector<float> bins = {0.0f, 100.0f, 1000.0f, 5000.0f, samplingRate / 2.0f};
    float maxMag = 0.0f;

    const size_t totalSamples = wav.totalPCMFrameCount * wav.channels;
    drwav_int32 windowedSamples[frameSize]; // A chunk of the decoded samples
    kiss_fft_cfg fftCfg = kiss_fft_alloc(frameSize, 0, NULL, NULL); // Configuration for the FFT
    kiss_fft_cpx fftInput[frameSize]; //windowedSamples converted to a complex array
    kiss_fft_cpx fftOutput[frameSize]; 

    //Decode sample data from wav file
    drwav_int32* pDecodedInterleavedPCMFrames = (drwav_int32*) malloc(wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int32));
    size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s32(&wav, wav.totalPCMFrameCount, pDecodedInterleavedPCMFrames);
    
    if(!pDecodedInterleavedPCMFrames || numberOfSamplesActuallyDecoded != wav.totalPCMFrameCount){
        cerr << "Memory allocation failed" << endl;
        return 1; 
    }
    else {
        cout << "Memory allocated" << endl;
    }

    //Break samples into frames
    for (size_t i = 0; i + frameSize <= totalSamples; i += overlap){
        for (size_t j = 0; j < frameSize; ++j){
            windowedSamples[j] = pDecodedInterleavedPCMFrames[i+j];
        }

        //Apply hann function to reduce artifacts
        hannWindow(windowedSamples, frameSize);

        //Create a complex array from windowed samples
        for (size_t j = 0; j < frameSize; ++j) {
            fftInput[j].r = windowedSamples[j];
            fftInput[j].i = 0;
        }

        //Assign Frequency Bins to fftOutput using a Fast Fourier Transform
        kiss_fft(fftCfg, fftInput, fftOutput);

        //Magnitude + binning
        float freqResolution = samplingRate / frameSize;
        size_t numBins = bins.size();
        vector<float> binMagnitudes(numBins, 0.0f);

        for(size_t i = 0; i < frameSize / 2; ++i ){
            float magnitude = getMag(fftOutput[i]);
            float frequency = i * freqResolution;

            //For each frequency bin, add it's magnitude to the appropriate bin
            for(size_t bindex = 0; bindex < bins.size(); ++bindex){
                if(frequency >= bins[bindex] && frequency < bins[bindex + 1]){
                    binMagnitudes[bindex] += magnitude;
                    break;
                }
            }
            for(float binMag : binMagnitudes){ 
                maxMag = std::max(maxMag, binMag);
            }
        }
        
        outputHistory.push_back(binMagnitudes);
    }

    for(vector<float>& frame : outputHistory){
        for(float& f : frame){
            f = f/maxMag;
        }
    }

    free(pDecodedInterleavedPCMFrames);
    free(fftCfg);

    return 0;
}
#include "AudioEngine.h"

void AudioEngine::hannWindow(vector<drwav_int32>& samples){
    for (size_t n = 0; n < frameSize; ++n){
        double hannValue = 0.5 * (1 - cos(2 * M_PI * n / (frameSize - 1)));
        samples[n] = static_cast<drwav_int32>(samples[n] * hannValue);
    }
}

float AudioEngine::getMag(const kiss_fft_cpx& value) {
    return sqrt(value.r * value.r + value.i * value.i);
}

AudioEngine::AudioEngine(const char* fileName){
    if (!drwav_init_file(&wav, fileName, NULL)) {
        cerr << "File not found" << endl;
    }
    else {
        cout << "Using file: " << fileName << endl;
    }

    totalSamples = wav.totalPCMFrameCount * wav.channels;
    
    pDecodedInterleavedPCMFrames = (drwav_int32*) malloc(totalSamples * sizeof(drwav_int32));
    size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s32(&wav, wav.totalPCMFrameCount, pDecodedInterleavedPCMFrames);
    if(!pDecodedInterleavedPCMFrames || numberOfSamplesActuallyDecoded != wav.totalPCMFrameCount){
        cerr << "Memory allocation failed" << endl;
    }
    else {
        cout << "Memory allocated" << endl;
    }
}

AudioEngine::~AudioEngine(){
    free(pDecodedInterleavedPCMFrames);
    free(fftCfg);
    drwav_uninit(&wav);
}

void AudioEngine::preProccess(){
    vector<drwav_int32> windowedSamples(frameSize); // A chunk of the decoded samples
    kiss_fft_cpx fftInput[frameSize]; //windowedSamples converted to a complex array
    kiss_fft_cpx fftOutput[frameSize];

    //Break samples into frames
    for (size_t i = 0; i + frameSize <= totalSamples; i += overlap){
        for (size_t j = 0; j < frameSize; ++j){
            windowedSamples[j] = pDecodedInterleavedPCMFrames[i+j];
        }

        //Apply hann function to reduce artifacts
        hannWindow(windowedSamples);

        //Create a complex array from windowed samples
        for (size_t j = 0; j < frameSize; ++j) {
            fftInput[j].r = windowedSamples[j];
            fftInput[j].i = 0;
        }

        //Assign Frequency Bins to fftOutput using a Fast Fourier Transform
        kiss_fft(fftCfg, fftInput, fftOutput);

        //Magnitude + binning
        float freqResolution = sampleRate / frameSize;
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
        
        frequencyHist.push_back(binMagnitudes);
    }

    for(vector<float>& frame : frequencyHist){
        for(float& f : frame){
            f = f/maxMag;
        }
    }
}

void AudioEngine::printVals(){
    for(int i = 0; i < 1000; ++i){
        for(float f : frequencyHist[i]){
            cout << f << " ";
        }
        cout << endl;
    }
}
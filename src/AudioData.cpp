#include "AudioData.h"

void AudioData::hannWindow(vector<float>& samples){
    for (size_t n = 0; n < frameSize; ++n){
        double hannValue = 0.5 * (1 - cos(2 * M_PI * n / (frameSize - 1)));
        samples[n] = (samples[n] * hannValue);
    }
}

float AudioData::getMag(const kiss_fft_cpx& value) {
    return sqrt(value.r * value.r + value.i * value.i);
}

AudioData::AudioData(const char* fileName){
    if (!drwav_init_file(&wav, fileName, NULL)) {
        throw std::runtime_error("File not found: '" + std::string(fileName) + "'");
    }
    else {
        cout << "Using file: " << fileName << endl;
    }

    totalSamples = wav.totalPCMFrameCount * wav.channels;
    sampleRate = wav.sampleRate;
    maxMag = 0.0f;

    
    normalSamples.resize(totalSamples);
    
    size_t framesDecoded = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, normalSamples.data());
    if(framesDecoded != wav.totalPCMFrameCount){
        cerr << "Memory allocation failed" << endl;
    }
    else {
        cout << "Memory allocated" << endl;
    }
    
}

AudioData::~AudioData(){
    free(fftCfg);
    drwav_uninit(&wav);
}

void AudioData::preProcess(){
    vector<float> windowedSamples(frameSize); // A chunk of the decoded samples
    kiss_fft_cpx fftInput[frameSize]; //windowedSamples converted to a complex array
    kiss_fft_cpx fftOutput[frameSize];

    //Break samples into frames
    for (size_t i = 0; i + frameSize <= totalSamples; i += overlap){
        for (size_t j = 0; j < frameSize; ++j){
            windowedSamples[j] = normalSamples[i+j];
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
        vector<float> binMagnitudes(bins.size(), 0.0f);

        for(size_t j = 0; j < frameSize / 2; ++j ){
            float magnitude = getMag(fftOutput[j]);
            float frequency = j * freqResolution;

            //For each frequency bin, add it's magnitude to the appropriate bin
            for(size_t bindex = 0; bindex < bins.size(); ++bindex){
                if(frequency >= bins[bindex] && (bindex == bins.size() - 1 || frequency < bins[bindex + 1])){
                    binMagnitudes[bindex] += magnitude;
                    break;
                }
            }
        }
        for(float binMag : binMagnitudes){ 
            maxMag = std::max(maxMag, binMag);
        }

        frequencyHist.push_back(binMagnitudes);
    }
}

void AudioData::printVals(){
    for(int i = 0; i < 1000; ++i){
        for(float f : frequencyHist[i]){
            cout << f << " ";
        }
        cout << endl;
    }
    cout << "Max: " << maxMag << endl;
}

vector<float> AudioData::getNormals(){
    return normalSamples;
}

vector<vector<float>>& AudioData::getFreqs(){ 
    return frequencyHist; 
}

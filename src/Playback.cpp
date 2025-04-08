#include "Playback.h"

Playback::Playback(vector<float> audioData){
    buffer = audioData;
}


int Playback::audioCallback(void* outputBuffer, void*, unsigned int bufferFrames,
                            double, RtAudioStreamStatus, void* userData) {
    auto* self = static_cast<Playback*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    for (unsigned int i = 0; i < bufferFrames; ++i) {
        for (unsigned int ch = 0; ch < 2; ++ch){    
            if (self->samplesPlayed < self->buffer.size()) {
                out[i * 2 + ch] = self->buffer[self->samplesPlayed++];
            } 
            else {
                out[i * 2 + ch] = 0.0f; // silence
            }
        }
    }

    return 0;
}


void Playback::start() {
    RtAudio::StreamParameters outputParams;
    if (dac.getDeviceCount() < 1){
        cerr << "No audio devices found";
        return;
    }
    outputParams.deviceId = dac.getDefaultOutputDevice();
    outputParams.nChannels = 2;

    try {
        dac.openStream(&outputParams, nullptr, RTAUDIO_FLOAT32, 44100, &bufferSize, &Playback::audioCallback, this);
        dac.startStream();
        cout << "Stream started" << endl;
    } catch (RtAudioErrorType& e) {
        cerr << "RtAudio error"<< endl;
    }
}

void Playback::stop() {
    if (dac.isStreamRunning()) {
        dac.stopStream();
    }
    if (dac.isStreamOpen()) {
        dac.closeStream();
    }
}
#include "Playback.h"

Playback::Playback(vector<float> audioData){
    buffer = audioData;
}


int Playback::audioCallback(void* outputBuffer, void*, unsigned int bufferFrames,
                            double, RtAudioStreamStatus, void* userData) {
    auto* self = static_cast<Playback*>(userData);
    float* out = static_cast<float*>(outputBuffer);

    for (unsigned int i = 0; i < bufferFrames; ++i) {
        if (self->samplesPlayed < self->buffer.size()) {
            out[i] = self->buffer[self->samplesPlayed++];
        } 
        else {
            out[i] = 0.0f; // silence
        }
    }

    return 0;
}


void Playback::start() {
    RtAudio::StreamParameters outputParams;
    outputParams.deviceId = dac.getDefaultOutputDevice();
    if (dac.getDeviceCount() < 1){
        cerr << "No audio devices found";
        return;
    }
    for (unsigned int i = 0; i < dac.getDeviceCount(); ++i) {
        RtAudio::DeviceInfo info = dac.getDeviceInfo(i);
        std::cout << "Device " << i << ": " << info.name << std::endl;
    }
    outputParams.nChannels = 1;

    try {
        dac.openStream(&outputParams, nullptr, RTAUDIO_FLOAT32, 44100, &bufferSize, &Playback::audioCallback, this);
        dac.startStream();
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
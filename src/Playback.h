#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "RtAudio.h"
#include "AudioData.h"

using std::vector;

class Playback {
    private:

    RtAudio dac;
    vector<float> buffer;
    unsigned int bufferSize = 512;
    size_t samplesPlayed = 0;
    size_t framesPlayed = 0;

    public:

    Playback(vector<float> audioData);

    static int audioCallback(void *outputBuffer, void *inputBuffer,
                             unsigned int nBufferFrames, double streamTime,
                             RtAudioStreamStatus status, void *userData);

    void start();

    size_t getFramesPlayed();

    void stop();

};


#endif // PLAYBACK_H
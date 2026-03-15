#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include <vector>
#include <mutex>
#include "miniaudio.h"

namespace AuraFlow {

class AudioRecorder {
public:
    AudioRecorder();
    ~AudioRecorder();

    bool start();
    void stop();
    std::vector<float> getAudioBuffer();
    bool isRecording() const { return m_isRecording; }

private:
    static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    ma_device m_device;
    std::vector<float> m_buffer;
    std::mutex m_mutex;
    bool m_isRecording = false;
    bool m_isInitialized = false;
};

} // namespace AuraFlow

#endif // AUDIO_RECORDER_H

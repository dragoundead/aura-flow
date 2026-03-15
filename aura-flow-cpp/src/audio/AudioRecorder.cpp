#define MINIAUDIO_IMPLEMENTATION
#include "AudioRecorder.h"
#include <iostream>

namespace AuraFlow {

AudioRecorder::AudioRecorder() {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format   = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate       = 16000;
    config.dataCallback     = dataCallback;
    config.pUserData        = this;

    if (ma_device_init(NULL, &config, &m_device) != MA_SUCCESS) {
        std::cerr << "[AudioRecorder] Failed to initialize capture device." << std::endl;
        return;
    }
    m_isInitialized = true;
}

AudioRecorder::~AudioRecorder() {
    if (m_isInitialized) {
        ma_device_uninit(&m_device);
    }
}

bool AudioRecorder::start() {
    if (!m_isInitialized) return false;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_buffer.clear();
        m_isRecording = true;
    }

    if (ma_device_start(&m_device) != MA_SUCCESS) {
        m_isRecording = false;
        return false;
    }
    return true;
}

void AudioRecorder::stop() {
    if (m_isRecording) {
        ma_device_stop(&m_device);
        m_isRecording = false;
    }
}

std::vector<float> AudioRecorder::getAudioBuffer() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_buffer;
}

void AudioRecorder::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioRecorder* pRecorder = (AudioRecorder*)pDevice->pUserData;
    if (pRecorder == nullptr || !pRecorder->m_isRecording) return;

    const float* pInputF32 = (const float*)pInput;
    
    std::lock_guard<std::mutex> lock(pRecorder->m_mutex);
    pRecorder->m_buffer.insert(pRecorder->m_buffer.end(), pInputF32, pInputF32 + frameCount);
}

} // namespace AuraFlow

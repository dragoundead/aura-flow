#include "AuraEngine.h"
#include <iostream>
#include <thread>

namespace AuraFlow {

AuraEngine::AuraEngine() {
    // Default parameters for whisper.cpp
    // Default parameters for whisper.cpp optimized for speed
    m_wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    m_wparams.n_threads = std::max(1, (int)std::thread::hardware_concurrency());
    m_wparams.greedy.best_of = 1;
    m_wparams.beam_search.beam_size = 1;
    m_wparams.no_context = true; // Faster, ignores previous context
    m_wparams.print_special = false;
    m_wparams.print_progress = false;
    m_wparams.print_realtime = false;
    m_wparams.print_timestamps = false;
    
    std::cout << "[AuraEngine] Using " << m_wparams.n_threads << " CPU threads" << std::endl;
}

AuraEngine::~AuraEngine() {
    if (m_ctx) {
        whisper_free(m_ctx);
    }
}

bool AuraEngine::loadModel(const std::string& modelPath, bool useGpu) {
    if (m_ctx) {
        whisper_free(m_ctx);
        m_ctx = nullptr;
    }
    m_isLoaded = false;

    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = false; // Force CPU - AMD GPU causes hangs with DirectML

    std::cout << "[AuraEngine] Loading model: " << modelPath << " (CPU mode)" << std::endl;

    m_ctx = whisper_init_from_file_with_params(modelPath.c_str(), cparams);
    if (!m_ctx) {
        std::cerr << "[AuraEngine] Failed to load: " << modelPath << std::endl;
        return false;
    }
    m_isLoaded = true;
    std::cout << "[AuraEngine] Model loaded successfully!" << std::endl;
    return true;
}

std::string AuraEngine::transcribe(const std::vector<float>& audioBuffer, const std::string& task) {
    if (!m_isLoaded || audioBuffer.empty() || m_isBusy) return "";

    m_isBusy = true;
    m_wparams.translate = (task == "translate");
    m_wparams.language = "ru";

    std::cout << "[AuraEngine] Transcribing " << audioBuffer.size() / 16000.0f << "s of audio..." << std::endl;

    if (whisper_full(m_ctx, m_wparams, audioBuffer.data(), audioBuffer.size()) != 0) {
        std::cerr << "[AuraEngine] Failed to process audio" << std::endl;
        return "";
    }

    std::string result = "";
    int n_segments = whisper_full_n_segments(m_ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char* text = whisper_full_get_segment_text(m_ctx, i);
        result += text;
    }

    std::cout << "[AuraEngine] Result: " << result << std::endl;
    m_isBusy = false;
    return result;
}

} // namespace AuraFlow

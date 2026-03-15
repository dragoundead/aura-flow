#ifndef AURA_ENGINE_H
#define AURA_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include "whisper.h"

namespace AuraFlow {

class AuraEngine {
public:
    AuraEngine();
    ~AuraEngine();

    bool loadModel(const std::string& modelPath, bool useGpu = true);
    std::string transcribe(const std::vector<float>& audioBuffer, const std::string& task = "translate");

    bool isBusy() const { return m_isBusy; }

private:
    whisper_context* m_ctx = nullptr;
    whisper_full_params m_wparams;
    bool m_isLoaded = false;
    bool m_isBusy = false;
};

} // namespace AuraFlow

#endif // AURA_ENGINE_H

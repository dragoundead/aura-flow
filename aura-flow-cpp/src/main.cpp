#include <QApplication>
#include <QThread>
#include <iostream>
#include <memory>
#include <filesystem>
#include "engine/AuraEngine.h"
#include "audio/AudioRecorder.h"
#include "system/HotkeyManager.h"
#include "system/ClipboardManager.h"
#include "utils/PostFormatter.h"
#include "ui/OverlayWindow.h"
#include "ui/SystemTray.h"

using namespace AuraFlow;

int main(int argc, char *argv[]) {
#ifdef _WIN32
    system("chcp 65001 > nul"); // Force UTF-8 in console
#endif
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    // Get the directory where the exe is located
    std::filesystem::path exeDir = std::filesystem::path(argv[0]).parent_path();
    if (exeDir.empty()) exeDir = ".";
    
    std::cout << "[Aura Flow] Starting C++ Version..." << std::endl;
    std::cout << "[Aura Flow] Exe directory: " << exeDir.string() << std::endl;

    // Components
    auto engine = std::make_shared<AuraEngine>();
    auto recorder = std::make_shared<AudioRecorder>();
    auto overlay = std::make_shared<OverlayWindow>();
    auto tray = std::make_shared<SystemTray>();
    HotkeyManager hotkeys;

    static std::string currentMode = "translate";

    // Tray logic
    tray->setModeCallbacks([](QString mode) {
        currentMode = mode.toStdString();
        std::cout << "[Aura Flow] Mode: " << currentMode << std::endl;
    });
    tray->show();

    // Hotkey logic
    hotkeys.setCallbacks(
        [recorder, overlay]() {
            std::cout << "[REC] Start event received" << std::endl;
            if (recorder->start()) {
                QString label = (currentMode == "translate") ? "Recording [EN]" : 
                                (currentMode == "post") ? "Recording [POST]" : "Recording [RU]";
                
                // Thread-safe UI call
                QMetaObject::invokeMethod(overlay.get(), "showRecording", Qt::QueuedConnection, Q_ARG(QString, label));
            }
        },
        [recorder, engine, overlay]() {
            if (engine->isBusy()) {
                std::cout << "[REC] Ignored - Engine is busy" << std::endl;
                return;
            }
            std::cout << "[REC] Stop event received. Processing..." << std::endl;
            recorder->stop();
            
            // Thread-safe UI call
            QMetaObject::invokeMethod(overlay.get(), "showProcessing", Qt::QueuedConnection);

            // Run transcription in a separate thread to keep UI responsive
            std::thread([recorder, engine, overlay]() {
                auto buffer = recorder->getAudioBuffer();
                std::cout << "[Aura Flow] Transcription thread started. Buffer size: " << buffer.size() << std::endl;
                
                std::string text;
                try {
                    if (currentMode == "post") {
                        text = engine->transcribe(buffer, "transcribe");
                        if (!text.empty()) {
                            text = PostFormatter::formatPost(text);
                        }
                    } else {
                        text = engine->transcribe(buffer, currentMode);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "[Aura Flow] Exception in transcription: " << e.what() << std::endl;
                }

                if (!text.empty()) {
                    std::cout << "[Aura Flow] Transcription result: " << text << std::endl;
                    ClipboardManager::writeAndPaste(text);
                } else {
                    std::cout << "[Aura Flow] Transcription returned empty result" << std::endl;
                }
                
                // Hide overlay on main thread
                QMetaObject::invokeMethod(overlay.get(), "hideIndicator", Qt::QueuedConnection);
            }).detach();
        }
    );

    // Build model paths relative to the exe directory
    // Relative to out/build/x64-Release/, models are at ../../../models/
    std::filesystem::path modelsBase = exeDir / ".." / ".." / ".." / "models";
    
    // Try current dir first, then parent-relative
    std::vector<std::string> modelPaths = {
        (modelsBase / "ggml-large-v3-turbo-q8_0.bin").string(),
        "models/ggml-large-v3-turbo-q8_0.bin",
        "ggml-large-v3-turbo-q8_0.bin"
    };

    bool modelLoaded = false;
    for (const auto& p : modelPaths) {
        std::cout << "[Aura Flow] Checking for model at: " << p << std::endl;
        if (engine->loadModel(p, false)) {
            modelLoaded = true;
            break;
        }
    }
    
    if (!modelLoaded) {
        std::cerr << "[Aura Flow] ERROR: Default models not found. Will try any .bin in models/" << std::endl;
        // Fallback: list models dir and try first bin
        if (std::filesystem::exists(modelsBase)) {
            for (const auto& entry : std::filesystem::directory_iterator(modelsBase)) {
                if (entry.path().extension() == ".bin") {
                    if (engine->loadModel(entry.path().string(), false)) {
                        modelLoaded = true;
                        break;
                    }
                }
            }
        }
    }

    if (!modelLoaded) {
        std::cerr << "[Aura Flow] CRITICAL ERROR: No model loaded! Transcription will fail." << std::endl;
    }

    hotkeys.start();
    std::cout << "[Aura Flow] ✓ READY! Ctrl+Space to record." << std::endl;

    int result = app.exec();
    
    hotkeys.stop();
    return result;
}

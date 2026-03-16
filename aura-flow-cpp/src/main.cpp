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
    
    // Show launching indicator IMMEDIATELY
    overlay->showLaunching("Aura Flow: Launching...");

    auto tray = std::make_shared<SystemTray>();
    tray->show();
    tray->showStartingMessage();
    
    HotkeyManager hotkeys;

    static std::string currentMode = "transcribe";

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
    std::filesystem::path modelsBase = exeDir / ".." / ".." / ".." / "models";
    if (!std::filesystem::exists(modelsBase)) {
        modelsBase = exeDir / "models";
    }

    // Model selection logic
    tray->setModelCallback([engine, overlay, modelsBase](QString modelName) {
        if (engine->isBusy()) return;
        
        std::filesystem::path fullPath = modelsBase / modelName.toStdString();
        std::cout << "[Aura Flow] Switching to model: " << fullPath.string() << std::endl;
        
        QMetaObject::invokeMethod(overlay.get(), "showLaunching", Qt::QueuedConnection, Q_ARG(QString, "Switching Model..."));
        
        std::thread([engine, overlay, fullPath]() {
            if (engine->loadModel(fullPath.string(), false)) {
                std::cout << "[Aura Flow] Model switch success!" << std::endl;
            }
            QMetaObject::invokeMethod(overlay.get(), "hideIndicator", Qt::QueuedConnection);
        }).detach();
    });

    // Initial scan and load
    std::string defaultModel = "ggml-medium-q5_k.bin";
    bool modelLoaded = false;

    if (std::filesystem::exists(modelsBase)) {
        for (const auto& entry : std::filesystem::directory_iterator(modelsBase)) {
            if (entry.path().extension() == ".bin") {
                std::string filename = entry.path().filename().string();
                bool isDefault = (filename == defaultModel);
                
                if (!modelLoaded && (isDefault || !std::filesystem::exists(modelsBase / defaultModel))) {
                    if (engine->loadModel(entry.path().string(), false)) {
                        modelLoaded = true;
                        tray->addModelOption(QString::fromStdString(filename), true);
                        continue;
                    }
                }
                tray->addModelOption(QString::fromStdString(filename), false);
            }
        }
    }

    if (!modelLoaded) {
        std::cerr << "[Aura Flow] CRITICAL ERROR: No model loaded!" << std::endl;
    }

    hotkeys.start();
    std::cout << "[Aura Flow] ✓ READY! Ctrl+Space to record." << std::endl;
    
    overlay->hideIndicator();
    tray->showReadyMessage();

    int result = app.exec();
    
    hotkeys.stop();
    return result;
}

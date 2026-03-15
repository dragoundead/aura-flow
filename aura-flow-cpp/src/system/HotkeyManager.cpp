#include "HotkeyManager.h"
#include <iostream>

namespace AuraFlow {

HotkeyManager::HotkeyManager() {}

HotkeyManager::~HotkeyManager() {
    stop();
}

void HotkeyManager::setCallbacks(std::function<void()> onPress, std::function<void()> onRelease) {
    m_onPress = onPress;
    m_onRelease = onRelease;
}

void HotkeyManager::start() {
    if (m_running) return;
    m_running = true;
    m_thread = std::thread(&HotkeyManager::loop, this);
}

void HotkeyManager::stop() {
    if (!m_running) return;
    m_running = false;
    // Post a dummy message to wake up GetMessage
    PostThreadMessage(GetThreadId(m_thread.native_handle()), WM_QUIT, 0, 0);
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void HotkeyManager::loop() {
    // Register Ctrl+Space
    // MOD_CONTROL = 0x0002, VK_SPACE = 0x20
    if (!RegisterHotKey(NULL, ID_HOTKEY, MOD_CONTROL | MOD_NOREPEAT, VK_SPACE)) {
        std::cerr << "[HotkeyManager] Failed to register hotkey." << std::endl;
        m_running = false;
        return;
    }

    MSG msg = {0};
    bool isKeyPressed = false;

    while (m_running && GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY && msg.wParam == ID_HOTKEY) {
            if (!isKeyPressed) {
                isKeyPressed = true;
                if (m_onPress) m_onPress();
            }
            
            // Note: WM_HOTKEY doesn't give a "release" event easily.
            // For push-to-talk, we usually need a keyboard hook (SetWindowsHookEx)
            // or pollGetAsyncKeyState.
            // Let's use Polling for the release since we have the hotkey trigger.
            
            while (isKeyPressed) {
                if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) || !(GetAsyncKeyState(VK_SPACE) & 0x8000)) {
                    isKeyPressed = false;
                    if (m_onRelease) m_onRelease();
                }
                Sleep(10);
            }
        }
    }

    UnregisterHotKey(NULL, ID_HOTKEY);
}

} // namespace AuraFlow

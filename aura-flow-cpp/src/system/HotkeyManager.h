#ifndef HOTKEY_MANAGER_H
#define HOTKEY_MANAGER_H

#include <windows.h>
#include <functional>
#include <thread>
#include <atomic>

namespace AuraFlow {

class HotkeyManager {
public:
    HotkeyManager();
    ~HotkeyManager();

    void setCallbacks(std::function<void()> onPress, std::function<void()> onRelease);
    void start();
    void stop();

private:
    void loop();

    std::function<void()> m_onPress;
    std::function<void()> m_onRelease;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    
    static const int ID_HOTKEY = 1;
};

} // namespace AuraFlow

#endif // HOTKEY_MANAGER_H

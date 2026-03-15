#include "ClipboardManager.h"
#include <windows.h>
#include <iostream>

namespace AuraFlow {

void ClipboardManager::writeAndPaste(const std::string& text) {
    if (text.empty()) return;
    setClipboardText(text);
    Sleep(100); // Wait for system to process clipboard
    simulateCtrlV();
}

void ClipboardManager::setClipboardText(const std::string& text) {
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();

    // Convert to UTF-16 for Windows clipboard
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeNeeded * sizeof(wchar_t));
    if (hGlobal) {
        wchar_t* pBuf = (wchar_t*)GlobalLock(hGlobal);
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, pBuf, sizeNeeded);
        GlobalUnlock(hGlobal);
        SetClipboardData(CF_UNICODETEXT, hGlobal);
    }
    CloseClipboard();
}

void ClipboardManager::simulateCtrlV() {
    INPUT inputs[4] = {0};

    // Ctrl down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;

    // V down
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';

    // V up
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // Ctrl up
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(4, inputs, sizeof(INPUT));
}

} // namespace AuraFlow

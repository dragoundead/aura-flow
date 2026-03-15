#ifndef CLIPBOARD_MANAGER_H
#define CLIPBOARD_MANAGER_H

#include <string>

namespace AuraFlow {

class ClipboardManager {
public:
    static void writeAndPaste(const std::string& text);

private:
    static void setClipboardText(const std::string& text);
    static void simulateCtrlV();
};

} // namespace AuraFlow

#endif // CLIPBOARD_MANAGER_H

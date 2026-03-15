#ifndef POST_FORMATTER_H
#define POST_FORMATTER_H

#include <string>
#include <vector>
#include <map>

namespace AuraFlow {

class PostFormatter {
public:
    static std::string formatPost(const std::string& rawText);

private:
    static std::vector<std::string> splitSentences(const std::string& text);
    static std::string findEmoji(const std::string& sentence);
    static std::string capitalize(std::string s);

    static const std::map<std::string, std::string> EMOJI_MAP;
    static const std::vector<std::string> FILLER_EMOJIS;
};

} // namespace AuraFlow

#endif // POST_FORMATTER_H

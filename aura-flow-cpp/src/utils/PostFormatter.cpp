#include "PostFormatter.h"
#include <regex>
#include <algorithm>
#include <sstream>

namespace AuraFlow {

const std::map<std::string, std::string> PostFormatter::EMOJI_MAP = {
    {"привет", "👋"}, {"здравствуй", "👋"}, {"хай", "👋"},
    {"спасибо", "🙏"}, {"благодар", "🙏"},
    {"поздравля", "🎉"}, {"празднуем", "🎉"}, {"праздник", "🎉"},
    {"люб", "❤️"}, {"обожа", "❤️"}, {"нрав", "💖"},
    {"счастлив", "😊"}, {"рад", "😊"}, {"круто", "🔥"}, {"класс", "🔥"}, {"офигенно", "🔥"},
    {"грустн", "😔"}, {"пешаль", "😢"},
    {"смешн", "😂"}, {"ха-ха", "😂"}, {"лол", "😂"},
    {"работа", "💼"}, {"бизнес", "💼"}, {"проект", "📋"},
    {"деньг", "💰"}, {"заработ", "💰"}, {"доход", "💰"}, {"прибыл", "💰"},
    {"продаж", "📈"}, {"рост", "📈"}, {"увеличен", "📈"},
    {"результат", "✅"}, {"итог", "✅"}, {"вывод", "✅"},
    {"важно", "⚡"}, {"внимание", "⚡"}, {"главн", "⚡"},
    {"совет", "💡"}, {"идея", "💡"}, {"лайфхак", "💡"},
    {"вопрос", "❓"}, {"как", "❓"},
    {"ответ", "💬"}, {"мнение", "💬"},
    {"код", "💻"}, {"программ", "💻"}, {"разработ", "💻"},
    {"телефон", "📱"}, {"приложен", "📱"},
    {"интернет", "🌐"}, {"сайт", "🌐"}, {"онлайн", "🌐"},
    {"учи", "📚"}, {"обучен", "📚"}, {"курс", "📚"}, {"книг", "📚"},
    {"путешеств", "✈️"}, {"поездк", "✈️"}, {"отпуск", "🏖️"},
    {"еда", "🍽️"}, {"готов", "🍳"}, {"рецепт", "🍳"}, {"вкусн", "😋"},
    {"спорт", "💪"}, {"тренировк", "💪"}, {"фитнес", "💪"}, {"здоровь", "🏥"},
    {"музык", "🎵"}, {"песн", "🎵"},
    {"фото", "📸"}, {"видео", "🎬"}, {"фильм", "🎬"},
    {"подписы", "👇"}, {"ссылк", "🔗"},
    {"время", "⏰"}, {"дедлайн", "⏰"},
    {"новост", "📢"}, {"анонс", "📢"},
    {"список", "📝"}, {"план", "📝"},
    {"друг", "👥"}, {"команд", "👥"}, {"коллег", "👥"},
    {"побед", "🏆"}, {"успех", "🏆"}, {"достижен", "🏆"},
    {"огонь", "🔥"}, {"топ", "🔥"}, {"бомб", "💣"},
    {"стоп", "🛑"}, {"ошибк", "❌"}, {"проблем", "⚠️"},
    {"старт", "🚀"}, {"запуск", "🚀"}, {"начинаем", "🚀"}, {"поехали", "🚀"}
};

const std::vector<std::string> PostFormatter::FILLER_EMOJIS = {
    "✨", "💫", "🌟", "⭐", "🔹", "▪️", "📌", "🎯"
};

std::string PostFormatter::formatPost(const std::string& rawText) {
    if (rawText.empty()) return "";

    std::vector<std::string> sentences = splitSentences(rawText);
    if (sentences.empty()) return rawText;

    std::vector<std::string> formattedLines;
    size_t fillerIdx = 0;

    for (auto& sent : sentences) {
        // Trim
        sent.erase(0, sent.find_first_not_of(" \t\n\r"));
        sent.erase(sent.find_last_not_of(" \t\n\r") + 1);

        if (sent.empty()) continue;

        std::string emoji = findEmoji(sent);
        if (emoji.empty()) {
            emoji = FILLER_EMOJIS[fillerIdx % FILLER_EMOJIS.size()];
            fillerIdx++;
        }

        sent = capitalize(sent);

        // Ensure punctuation
        if (!sent.empty() && std::string(".!?…").find(sent.back()) == std::string::npos) {
            sent += ".";
        }

        formattedLines.push_back(emoji + " " + sent);
    }

    if (formattedLines.empty()) return rawText;
    if (formattedLines.size() == 1) return formattedLines[0];

    std::string result = formattedLines[0] + "\n\n";
    for (size_t i = 1; i < formattedLines.size(); ++i) {
        result += formattedLines[i];
        if (i < formattedLines.size() - 1) result += "\n\n";
    }

    return result;
}

std::vector<std::string> PostFormatter::splitSentences(const std::string& text) {
    std::vector<std::string> parts;
    // Regex for sentence endings in UTF-8
    std::regex re("([.!?…])\\s+");
    std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
    std::sregex_token_iterator end;

    for (; iter != end; ++iter) {
        std::string part = *iter;
        if (!part.empty()) parts.push_back(part);
    }

    // Fallback if no punctuation and long text (simplified commas)
    if (parts.size() <= 1 && text.length() > 100) {
        std::stringstream ss(text);
        std::string segment;
        while (std::getline(ss, segment, ',')) {
            if (!segment.empty()) parts.push_back(segment);
        }
    }

    return parts;
}

std::string PostFormatter::findEmoji(const std::string& sentence) {
    std::string lower = sentence;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    for (const auto& [keyword, emoji] : EMOJI_MAP) {
        if (lower.find(keyword) != std::string::npos) {
            return emoji;
        }
    }
    return "";
}

std::string PostFormatter::capitalize(std::string s) {
    if (s.empty()) return s;
    // Basic ASCII capitalize, UTF-8 would need more care but for most Russian letters 
    // it works if we handle the first byte correctly in some locales.
    // However, simplest is to just check if it's lower.
    if (s[0] >= 'a' && s[0] <= 'z') s[0] = std::toupper(s[0]);
    // Note: Russian capitalization in UTF-8 is multi-byte and tricky without wide/third-party.
    return s;
}

} // namespace AuraFlow

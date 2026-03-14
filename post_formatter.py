"""
Offline post formatter — turns raw speech text into a styled Telegram post with emojis.
"""

import re

# Keyword → emoji mapping (Russian)
EMOJI_MAP = {
    # Greetings / emotions
    "привет": "👋", "здравствуй": "👋", "хай": "👋",
    "спасибо": "🙏", "благодар": "🙏",
    "поздравля": "🎉", "празднуем": "🎉", "праздник": "🎉",
    "люб": "❤️", "обожа": "❤️", "нрав": "💖",
    "счастлив": "😊", "рад": "😊", "круто": "🔥", "класс": "🔥", "офигенно": "🔥",
    "грустн": "😔", "печаль": "😢",
    "смешн": "😂", "ха-ха": "😂", "лол": "😂",
    
    # Business / work
    "работа": "💼", "бизнес": "💼", "проект": "📋",
    "деньг": "💰", "заработ": "💰", "доход": "💰", "прибыл": "💰",
    "продаж": "📈", "рост": "📈", "увеличен": "📈",
    "результат": "✅", "итог": "✅", "вывод": "✅",
    "важно": "⚡", "внимание": "⚡", "главн": "⚡",
    "совет": "💡", "идея": "💡", "лайфхак": "💡",
    "вопрос": "❓", "как": "❓",
    "ответ": "💬", "мнение": "💬",
    
    # Tech
    "код": "💻", "программ": "💻", "разработ": "💻",
    "телефон": "📱", "приложен": "📱",
    "интернет": "🌐", "сайт": "🌐", "онлайн": "🌐",
    
    # Actions
    "учи": "📚", "обучен": "📚", "курс": "📚", "книг": "📚",
    "путешеств": "✈️", "поездк": "✈️", "отпуск": "🏖️",
    "еда": "🍽️", "готов": "🍳", "рецепт": "🍳", "вкусн": "😋",
    "спорт": "💪", "тренировк": "💪", "фитнес": "💪", "здоровь": "🏥",
    "музык": "🎵", "песн": "🎵",
    "фото": "📸", "видео": "🎬", "фильм": "🎬",
    "подписы": "👇", "ссылк": "🔗",
    "время": "⏰", "дедлайн": "⏰",
    "новост": "📢", "анонс": "📢",
    "список": "📝", "план": "📝",
    "друг": "👥", "команд": "👥", "коллег": "👥",
    "побед": "🏆", "успех": "🏆", "достижен": "🏆",
    "огонь": "🔥", "топ": "🔥", "бомб": "💣",
    "стоп": "🛑", "ошибк": "❌", "проблем": "⚠️",
    "старт": "🚀", "запуск": "🚀", "начинаем": "🚀", "поехали": "🚀",
}

# Filler emojis for sentences that don't match any keyword
FILLER_EMOJIS = ["✨", "💫", "🌟", "⭐", "🔹", "▪️", "📌", "🎯"]


def format_post(raw_text: str) -> str:
    """Turn raw transcribed text into a formatted Telegram post."""
    if not raw_text or not raw_text.strip():
        return ""
    
    # 1. Split into sentences
    sentences = _split_sentences(raw_text.strip())
    
    if not sentences:
        return raw_text
    
    # 2. Format each sentence with an emoji
    formatted_lines = []
    filler_idx = 0
    
    for sent in sentences:
        sent = sent.strip()
        if not sent:
            continue
            
        # Find a matching emoji
        emoji = _find_emoji(sent)
        if not emoji:
            emoji = FILLER_EMOJIS[filler_idx % len(FILLER_EMOJIS)]
            filler_idx += 1
        
        # Capitalize first letter
        if sent and sent[0].islower():
            sent = sent[0].upper() + sent[1:]
        
        # Ensure sentence ends with punctuation
        if sent and sent[-1] not in ".!?…":
            sent += "."
        
        formatted_lines.append(f"{emoji} {sent}")
    
    # 3. Build the post
    if len(formatted_lines) == 1:
        return formatted_lines[0]
    
    # First line as "header" (bold-style with caps/emphasis)
    result = formatted_lines[0] + "\n\n"
    result += "\n\n".join(formatted_lines[1:])
    
    return result


def _split_sentences(text: str) -> list:
    """Split text into sentences, handling Russian punctuation."""
    # Split on sentence-ending punctuation
    parts = re.split(r'(?<=[.!?…])\s+', text)
    
    # If no punctuation found, split by commas for long texts
    if len(parts) == 1 and len(text) > 100:
        # Try splitting on commas that separate independent clauses
        parts = [p.strip() for p in text.split(",") if p.strip()]
        # Recombine very short fragments
        merged = []
        buf = ""
        for p in parts:
            if buf:
                buf += ", " + p
            else:
                buf = p
            if len(buf) > 40:
                merged.append(buf)
                buf = ""
        if buf:
            merged.append(buf)
        parts = merged if len(merged) > 1 else [text]
    
    return parts


def _find_emoji(sentence: str) -> str | None:
    """Find the best emoji for a sentence based on keyword matching."""
    lower = sentence.lower()
    for keyword, emoji in EMOJI_MAP.items():
        if keyword in lower:
            return emoji
    return None

# 🚀 Aura Flow — AI Voice Assistant

**Offline voice-to-text tool** for Windows 11. Speak Russian → get English translation or Russian transcription instantly. No API keys, no cloud, 100% local.

![Python](https://img.shields.io/badge/Python-3.11+-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Platform](https://img.shields.io/badge/Platform-Windows%2011-blue)

## ✨ Features

- 🎙 **Hold Ctrl+Space** — record your voice, release to transcribe
- 🌍 **Translation Mode** — speak Russian → English text appears in any app
- 🇷🇺 **Transcription Mode** — speak Russian → Russian text (saves typing time)
- 📝 **Telegram Post Mode** — speak → formatted post with emojis
- 🔒 **100% Offline** — everything runs locally, no data leaves your PC
- ⚡ **Fast** — powered by Whisper `large-v3` with `int8` optimization
- 🎨 **Wispr Flow-style UI** — animated recording indicator at the bottom of screen

## 📦 Installation

```bash
# Clone the repo
git clone https://github.com/dragoundead/aura-flow.git
cd aura-flow

# Install dependencies
pip install -r requirements.txt

# Run (first launch downloads ~3GB AI model)
python main.py
```

## 🎯 Usage

| Hotkey | Action |
|---|---|
| **Ctrl + Space** (hold) | Start recording |
| **Ctrl + Space** (release) | Stop & process |

### Modes (right-click tray icon → Mode):
- **Translate to English** — Russian speech → English text
- **Transcribe (Original)** — Russian speech → Russian text
- **📝 Post for Telegram** — Russian speech → formatted post with emojis

## 🛠 Tech Stack

- **[faster-whisper](https://github.com/SYSTRAN/faster-whisper)** — CTranslate2-optimized Whisper
- **PyQt6** — system tray & animated overlay
- **keyboard** — global hotkey detection
- **sounddevice** — microphone recording

## 💻 System Requirements

- Windows 10/11
- Python 3.11+
- 8+ GB RAM (16+ recommended)
- Any modern CPU (tested on Ryzen 7 7700)

## 📄 License

MIT License — use freely!

# 🚀 Aura Flow — AI Voice Assistant (C++ & Python)

**Offline voice-to-text tool** for Windows. Speak Russian → get English translation or Russian transcription instantly. No API keys, no cloud, 100% local.

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Qt](https://img.shields.io/badge/Qt-6.5--LTS-green)
![Python](https://img.shields.io/badge/Python-3.11+-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blue)

---

## 🔥 NEWS: C++ Engine is here!
We've migrated the core engine to **C++** using `whisper.cpp`. 
- 🚀 **Faster** startup and processing.
- 📉 **Lower** memory usage.
- ⚡ **AVX2 & OpenMP** optimized for CPU inference (no GPU required).
- 🎨 **Smoother UI** with Qt 6.

---

## ✨ Features

- 🎙 **Hold Ctrl+Space** — record your voice, release to transcribe.
- 🌍 **Translation Mode** — Russian speech → English text in any app.
- 🇷🇺 **Transcription Mode** — Russian speech → Russian text.
- 📝 **Telegram Post Mode** — speak → formatted post with emojis.
- 🔒 **100% Offline** — privacy-focused, zero data leakage.

## 📦 Getting Started (C++ Version - Recommended)

### 1. Build the project
Follow the instructions in [aura-flow-cpp/BUILD_RU.md](aura-flow-cpp/BUILD_RU.md) to build using Visual Studio 2022 and Qt 6.5.

### 2. Download Model
Run the preparation script to download the optimized AI model:
```bash
python prepare_model.py
```

### 3. Run
Launch `AuraFlow.exe` from the build directory.

---

## 🐍 Python Version (Legacy/Lightweight)

If you prefer not to build from source, the Python version is still available.

```bash
# Clone the repo
git clone https://github.com/dragoundead/aura-flow.git
cd aura-flow

# Install dependencies
pip install -r requirements.txt

# Run
python main.py
```

## 🎯 Usage

| Hotkey | Action |
|---|---|
| **Ctrl + Space** (hold) | Start recording |
| **Ctrl + Space** (release) | Stop & process |

### Modes (right-click tray icon → Mode):
- **Translate to English** — Russian speech → English text.
- **Transcribe (Original)** — Russian speech → Russian text.
- **📝 Post for Telegram** — Russian speech → formatted post with emojis.

## 🛠 Tech Stack

- **C++ Version:** `whisper.cpp`, `Qt 6.5`, `miniaudio`, Win32 API.
- **Python Version:** `faster-whisper`, `PyQt6`, `keyboard`, `sounddevice`.

## 💻 System Requirements

- Windows 10/11
- 8+ GB RAM (16+ recommended for large models)
- CPU with AVX2 support (most modern CPUs since 2013)

## 📄 License

MIT License — use freely!

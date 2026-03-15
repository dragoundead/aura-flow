# 🏗 Aura Flow Architecture Guide

This document explains the internal workings of Aura Flow for future maintainers.

## 🚀 Overview
Aura Flow is a Python-based desktop application that provides offline voice-to-text. It features a system tray interface, a global hotkey (Ctrl+Space), and an animated overlay.

## 📂 Component Breakdown

### 1. `main.py` (The Orchestrator)
- **Role**: Handles the UI (PyQt6), global hotkeys (`keyboard`), and state management.
- **Threading**: 
  - Main Thread: PyQt6 event loop (UI & Tray).
  - Hotkey Thread: `keyboard` library runs its own listener.
  - Processing Thread: AI transcription runs in a separate `threading.Thread` to keep the UI responsive.
- **State Machine**: Uses `ui_state` (`idle`, `recording`, `processing`, `done`) to communicate between threads. A `QTimer` polls this state to update the UI safely.

### 2. `aura_engine.py` (The AI Core)
- **Role**: Manages the `faster-whisper` model and audio recording via `sounddevice`.
- **Key Logic**: 
  - `start_recording()`: Opens an input stream and stores chunks in a list.
  - `transcribe()`: Converts the raw audio to text.
- **Optimizations**: Uses `compute_type="int8"` to run efficiently on CPUs without sacrificing much accuracy.

### 3. `post_formatter.py` (Utility)
- **Role**: Offline text processing.
- **Logic**: Uses regex and keyword mapping to add emojis and format paragraphs for Telegram.

## 🛠 Key Technical Decisions

### 1. DLL Workaround (Critical)
On Windows, some AI libraries (specifically those using OpenMP or CTranslate2) can have DLL conflicts with Qt modules. 
**Solution**: We load the AI model (`engine.load_model()`) *before* importing any PyQt6 modules. This ensures the correct DLLs are loaded into the process memory first.

### 2. Typing via Clipboard
Standard typing simulations can fail with non-Latin characters (Russian) in many apps like Telegram.
**Solution**: We use the system clipboard (`QApplication.clipboard()`) and then simulate `Ctrl+V` using direct Win32 API calls (`keybd_event`) for maximum compatibility.

### 3. Animated Overlay
The overlay is a frameless, transparent `QWidget` with a custom `paintEvent`. It uses a 25FPS timer to update animations (waveform, spinning dots) to provide a premium "Wispr-style" look.

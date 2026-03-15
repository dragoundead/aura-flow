# 📦 Build & Deployment Guide

Technical details on how to package Aura Flow as a standalone executable.

## 🏗 PyInstaller Configuration

Aura Flow uses a specialized build script: `build_exe.py` (default build uses `AuraFlow.spec` for reproducibility).

### Key Flags:
- `--collect-all`: Essential for `faster_whisper` and `ctranslate2`. These libraries have many shared libraries (.dll) and data files that PyInstaller won't pick up automatically.
- `--hidden-import`: Explicitly includes libraries used in threads or via dynamic imports (e.g., `keyboard`, `PyQt6`).
- `--onedir`: Default. Creates a folder with an `.exe` and an `_internal` directory. This is generally more stable for large AI libraries than `--onefile`.
- `AURAFLOW_UPX=1` (or `python build_exe.py --upx`): Optional UPX compression (smaller build, but can increase AV false positives for some users).

## ⚠️ Potential Build Issues

### 1. Missing DLLs
If the EXE fails to start, it's often due to a missing `libiomp5md.dll` or similar Intel/OpenMP library.
- **Fix**: Check if `_internal` contains the dependencies from `ctranslate2`.

### 2. Antivirus False Positives
`pyinstaller` (especially with `--onefile`) often triggers antivirus warnings because it extracts files to `%TEMP%`.
- **Recommendation**: Use `--onedir` or sign the executable.

### 3. Model Path
The EXE does **not** bundle the AI model (~3GB). On first launch, the app will download the model to:
`%USERPROFILE%\.cache\huggingface\hub`
This keeps the installer size small.

## 🚀 How to build
1. Install requirements: `pip install -r requirements.txt`
   - If `pip` is not in PATH, use: `.\.venv\Scripts\python.exe -m pip install -r requirements.txt`
2. Run build (recommended, onedir): `python build_exe.py`
   - If `python` is not in PATH, use: `.\.venv\Scripts\python.exe build_exe.py`
3. Optional onefile build: `python build_exe.py --onefile`
4. The onedir output will be in `dist/AuraFlow/`.

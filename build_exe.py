import argparse
import os
import subprocess
import sys


def _ensure_pyinstaller():
    try:
        import PyInstaller.__main__  # noqa: F401
        return
    except ImportError:
        pass

    print("[Aura Flow] PyInstaller not found. Installing...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pyinstaller"])


def build():
    # Keep ASCII-only output to avoid Windows console encoding issues.
    print("[Aura Flow] Starting EXE build...")

    parser = argparse.ArgumentParser(description="Build Aura Flow Windows executable via PyInstaller.")
    parser.add_argument(
        "--onefile",
        action="store_true",
        help="Build a single-file EXE (slower start, more AV false positives).",
    )
    parser.add_argument(
        "--upx",
        action="store_true",
        help="Enable UPX compression (smaller, sometimes triggers AV).",
    )
    parser.add_argument(
        "--console",
        action="store_true",
        help="Show console window (debug builds).",
    )
    args = parser.parse_args()

    _ensure_pyinstaller()

    # Used by AuraFlow.spec (and also reasonable to keep for CLI builds).
    if args.upx:
        os.environ["AURAFLOW_UPX"] = "1"

    import PyInstaller.__main__

    if not args.onefile:
        pyinstaller_args = ["AuraFlow.spec", "--noconfirm", "--clean"]
        print(f"[Aura Flow] Running PyInstaller: {' '.join(pyinstaller_args)}")
        PyInstaller.__main__.run(pyinstaller_args)
        print("[Aura Flow] Build complete: dist/AuraFlow/AuraFlow.exe")
        return

    # Onefile build: keep config roughly in sync with AuraFlow.spec.
    pyinstaller_args = [
        "launcher.py",
        "--onefile",
        "--name=AuraFlow",
        "--noconfirm",
        "--clean",
        "--optimize=2",
        "--collect-all=faster_whisper",
        "--collect-all=ctranslate2",
        "--hidden-import=PyQt6.sip",
        "--hidden-import=keyboard",
        "--hidden-import=sounddevice",
        "--hidden-import=numpy",
        "--add-data=aura_engine.py;.",
        "--add-data=post_formatter.py;.",
    ]
    if not args.console:
        pyinstaller_args.append("--noconsole")

    print(f"[Aura Flow] Running PyInstaller: {' '.join(pyinstaller_args)}")
    PyInstaller.__main__.run(pyinstaller_args)
    print("[Aura Flow] Build complete: dist/AuraFlow.exe")


if __name__ == "__main__":
    build()

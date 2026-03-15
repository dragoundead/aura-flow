"""
Aura Flow Launcher — wraps main.py to catch ALL errors including import failures.
"""
import sys
import os
import traceback

# Explicit imports to help PyInstaller's static analysis
import PyQt6.QtWidgets
import PyQt6.QtCore
import PyQt6.QtGui
import keyboard
import sounddevice
import numpy

def get_log_path():
    """Get crash log path next to the executable."""
    if getattr(sys, 'frozen', False):
        return os.path.join(os.path.dirname(sys.executable), "crash_log.txt")
    else:
        return os.path.join(os.path.dirname(os.path.abspath(__file__)), "crash_log.txt")

def main():
    try:
        # Importing main executes the app (it ends with sys.exit(app.exec())).
        import main  # noqa: F401
    except SystemExit:
        pass  # Normal Qt exit
    except Exception as e:
        log_path = get_log_path()
        with open(log_path, "w", encoding="utf-8") as f:
            f.write(f"Aura Flow Crash Log\n{'='*40}\n\n")
            f.write(f"Error: {e}\n\n")
            f.write(traceback.format_exc())
            f.write(f"\nPython: {sys.version}\n")
            f.write(f"Frozen: {getattr(sys, 'frozen', False)}\n")
        
        # Show Windows error dialog
        try:
            import ctypes
            ctypes.windll.user32.MessageBoxW(
                0,
                f"Aura Flow crashed!\n\n{e}\n\nSee crash_log.txt",
                "Aura Flow Error", 0x10
            )
        except:
            pass

if __name__ == "__main__":
    main()

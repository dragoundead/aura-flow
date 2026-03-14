import sys
import os
import math
import random
import threading
import winreg
import time
from post_formatter import format_post

# ──────────────────────────────────────────────────────────
# 1. Load AI model BEFORE Qt (DLL conflict workaround)
# ──────────────────────────────────────────────────────────
from aura_engine import AuraEngine

print("[Aura Flow] Loading AI model...")
engine = AuraEngine()
engine.load_model()
print("[Aura Flow] Model ready!\n")

# ──────────────────────────────────────────────────────────
# 2. Import Qt & UI modules
# ──────────────────────────────────────────────────────────
os.environ["QT_ENABLE_HIGHDPI_SCALING"] = "0"

import keyboard
import pyautogui
from PyQt6.QtWidgets import QApplication, QSystemTrayIcon, QMenu, QWidget
from PyQt6.QtCore import Qt, QTimer, QRectF
from PyQt6.QtGui import (
    QColor, QIcon, QAction, QPainter, QBrush, QPixmap,
    QFont, QPen, QLinearGradient, QPainterPath
)


# ── Animated Wispr-style Indicator ────────────────────────
class RecordingIndicator(QWidget):
    """Wispr Flow-style animated recording pill at bottom of screen."""

    def __init__(self):
        super().__init__()
        self.setWindowFlags(
            Qt.WindowType.FramelessWindowHint
            | Qt.WindowType.WindowStaysOnTopHint
            | Qt.WindowType.Tool
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground)

        self._mode = "idle"       # idle / recording / processing
        self._label = ""
        self._color = QColor("#4B9AFF")
        self._tick = 0
        self._bar_heights = [0.1] * 5
        self._target_heights = [0.1] * 5

        # Animation timer
        self._anim = QTimer()
        self._anim.timeout.connect(self._animate)

        self.setFixedSize(240, 52)
        self._center_on_screen()
        self.hide()

    def _center_on_screen(self):
        screen = QApplication.primaryScreen().geometry()
        # Position slightly above the taskbar
        self.move((screen.width() - self.width()) // 2, screen.height() - 110)

    def show_recording(self, label="Recording"):
        self._mode = "recording"
        self._label = label
        self._color = QColor("#FF4B4B")
        self._tick = 0
        self._anim.start(40)  # ~25fps animation
        self.show()
        self.raise_()

    def show_processing(self):
        self._mode = "processing"
        self._label = "Processing..."
        self._color = QColor("#4B9AFF")
        self._anim.start(40)
        self.show()
        self.raise_()

    def hide_indicator(self):
        self._mode = "idle"
        self._anim.stop()
        self.hide()

    def _animate(self):
        self._tick += 1
        # Smooth random bars
        if self._tick % 2 == 0:
            self._target_heights = [0.1 + random.random() * 0.9 for _ in range(5)]
        for i in range(5):
            self._bar_heights[i] += (self._target_heights[i] - self._bar_heights[i]) * 0.4
        self.update()

    def paintEvent(self, event):
        p = QPainter(self)
        p.setRenderHint(QPainter.RenderHint.Antialiasing)

        w, h = self.width(), self.height()

        # ── Background pill ──
        bg = QPainterPath()
        bg.addRoundedRect(QRectF(0, 0, w, h), h / 2, h / 2)

        # Dark glassmorphism background
        grad = QLinearGradient(0, 0, w, 0)
        grad.setColorAt(0, QColor(15, 15, 20, 245))
        grad.setColorAt(1, QColor(25, 25, 35, 245))
        p.fillPath(bg, QBrush(grad))

        # Border glow
        p.setPen(QPen(QColor(self._color.red(), self._color.green(), self._color.blue(), 100), 1.5))
        p.drawPath(bg)

        if self._mode == "recording":
            self._draw_recording(p, w, h)
        elif self._mode == "processing":
            self._draw_processing(p, w, h)

        p.end()

    def _draw_recording(self, p, w, h):
        # Pulsing Red Dot
        pulse = 0.8 + 0.2 * math.sin(self._tick * 0.15)
        dot_r = 6 * pulse
        p.setBrush(QBrush(QColor(255, 75, 75, int(255 * pulse))))
        p.setPen(Qt.PenStyle.NoPen)
        p.drawEllipse(QRectF(20 - dot_r, h / 2 - dot_r, dot_r * 2, dot_r * 2))

        # Waveform bars
        bw, gap = 3, 4
        bx = 40
        max_bh = 20
        for i, bh in enumerate(self._bar_heights):
            bh_pix = max(4, bh * max_bh)
            p.setBrush(QBrush(QColor(255, 255, 255, 180)))
            p.drawRoundedRect(QRectF(bx + i * (bw + gap), (h - bh_pix) / 2, bw, bh_pix), bw / 2, bw / 2)

        # Text
        p.setPen(QPen(QColor(255, 255, 255, 240)))
        p.setFont(QFont("Segoe UI", 11, QFont.Weight.DemiBold))
        p.drawText(QRectF(85, 0, w - 95, h), Qt.AlignmentFlag.AlignVCenter, self._label)

    def _draw_processing(self, p, w, h):
        # Spinning dots
        p.setPen(Qt.PenStyle.NoPen)
        for i in range(3):
            angle = self._tick * 0.15 + i * 2.1
            x = 22 + math.cos(angle) * 7
            y = h / 2 + math.sin(angle) * 7
            p.setBrush(QBrush(QColor(75, 154, 255, 200)))
            p.drawEllipse(QRectF(x - 3, y - 3, 6, 6))

        # Text
        p.setPen(QPen(QColor(255, 255, 255, 220)))
        p.setFont(QFont("Segoe UI", 11, QFont.Weight.DemiBold))
        p.drawText(QRectF(46, 0, w - 56, h), Qt.AlignmentFlag.AlignVCenter, self._label)


# ── State ─────────────────────────────────────────────────
task_mode = "translate"
is_recording = False
ui_state = "idle"
pending_text = None
indicator = None
tray = None
act_translate = None
act_transcribe = None
act_post = None


# ── UI Polling Timer ──────────────────────────────────────
def poll_ui():
    global ui_state, pending_text

    if ui_state == "recording":
        tags = {"translate": "EN", "transcribe": "RU", "post": "📝 POST"}
        tag = tags.get(task_mode, "")
        if indicator._mode != "recording":
            indicator.show_recording(f"Recording [{tag}]")
    elif ui_state == "processing":
        if indicator._mode != "processing":
            indicator.show_processing()
    elif ui_state == "done":
        indicator.hide_indicator()
        if pending_text:
            text = pending_text
            pending_text = None
            type_text_via_clipboard(text)
        ui_state = "idle"
    elif ui_state == "idle":
        if indicator.isVisible():
            indicator.hide_indicator()


def type_text_via_clipboard(text):
    """Type text into active window using clipboard + Win32 Ctrl+V."""
    if not text: return
    print(f"[Aura Flow] Writing: {text}")
    
    import ctypes
    
    # Set clipboard via Qt
    cb = QApplication.clipboard()
    cb.setText(text)
    time.sleep(0.1)
    
    # Simulate Ctrl+V via Win32 keybd_event (works in Telegram, browsers, etc.)
    VK_CONTROL = 0x11
    VK_V = 0x56
    KEYEVENTF_KEYUP = 0x0002
    
    user32 = ctypes.windll.user32
    user32.keybd_event(VK_CONTROL, 0, 0, 0)          # Ctrl down
    user32.keybd_event(VK_V, 0, 0, 0)                # V down
    time.sleep(0.05)
    user32.keybd_event(VK_V, 0, KEYEVENTF_KEYUP, 0)  # V up
    user32.keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0)  # Ctrl up


# ── Mode & Autostart ──────────────────────────────────────
def set_mode(mode):
    global task_mode
    task_mode = mode
    act_translate.setChecked(mode == "translate")
    act_transcribe.setChecked(mode == "transcribe")
    act_post.setChecked(mode == "post")
    print(f"[Aura Flow] Mode → {mode}")


def is_autostart():
    try:
        k = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\Run", 0, winreg.KEY_READ)
        winreg.QueryValueEx(k, "AuraFlow")
        return True
    except: return False


def toggle_autostart(checked):
    try:
        k = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\Run", 0, winreg.KEY_SET_VALUE)
        if checked:
            exe = os.path.realpath(sys.argv[0])
            cmd = f'"{sys.executable}" "{exe}"' if exe.endswith(".py") else f'"{exe}"'
            winreg.SetValueEx(k, "AuraFlow", 0, winreg.REG_SZ, cmd)
        else:
            try: winreg.DeleteValue(k, "AuraFlow")
            except: pass
        winreg.CloseKey(k)
    except Exception as e: print(f"Autostart Error: {e}")


# ── Recording (keyboard thread) ──────────────────────────
def on_space_press(event):
    global is_recording, ui_state
    if keyboard.is_pressed("ctrl") and not is_recording:
        is_recording = True
        ui_state = "recording"
        engine.start_recording()
        print("[Aura Flow] REC Start")

def on_space_release(event):
    global is_recording, ui_state
    if is_recording:
        is_recording = False
        ui_state = "processing"
        audio = engine.stop_recording()
        print("[Aura Flow] REC Stop")
        threading.Thread(target=process_thread, args=(audio,), daemon=True).start()

def process_thread(audio):
    global ui_state, pending_text
    if audio is not None:
        # Post mode: transcribe Russian, then format with emojis
        if task_mode == "post":
            result = engine.transcribe(audio, task="transcribe")
            if result:
                result = format_post(result)
                print(f"[Aura Flow] Formatted post:\n{result}")
        else:
            result = engine.transcribe(audio, task=task_mode)
        if result:
            pending_text = result
    ui_state = "done"


def quit_app():
    print("[Aura Flow] Bye!")
    keyboard.unhook_all()
    QApplication.instance().quit()


# ── Main ──────────────────────────────────────────────────
if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(False)

    indicator = RecordingIndicator()
    
    timer = QTimer()
    timer.timeout.connect(poll_ui)
    timer.start(40)

    # Tray
    pix = QPixmap(64, 64)
    pix.fill(Qt.GlobalColor.transparent)
    p = QPainter(pix)
    p.setRenderHint(QPainter.RenderHint.Antialiasing)
    p.setPen(Qt.PenStyle.NoPen)
    p.setBrush(QBrush(QColor("#4B9AFF")))
    p.drawEllipse(10, 10, 44, 44)
    p.end()

    tray = QSystemTrayIcon(QIcon(pix))
    tray.setToolTip("Aura Flow")

    menu = QMenu()
    mode_menu = QMenu("Mode")
    act_translate = QAction("Translate to English", checkable=True, checked=True)
    act_translate.triggered.connect(lambda: set_mode("translate"))
    act_transcribe = QAction("Transcribe (Original)", checkable=True, checked=False)
    act_transcribe.triggered.connect(lambda: set_mode("transcribe"))
    act_post = QAction("📝 Post for Telegram", checkable=True, checked=False)
    act_post.triggered.connect(lambda: set_mode("post"))
    mode_menu.addAction(act_translate)
    mode_menu.addAction(act_transcribe)
    mode_menu.addAction(act_post)
    menu.addMenu(mode_menu)
    menu.addSeparator()
    act_auto = QAction("Start with Windows", checkable=True, checked=is_autostart())
    act_auto.triggered.connect(toggle_autostart)
    menu.addAction(act_auto)
    menu.addSeparator()
    menu.addAction("Exit").triggered.connect(quit_app)
    tray.setContextMenu(menu)
    tray.show()

    # Hotkey (Ctrl+Space)
    keyboard.on_press_key("space", on_space_press, suppress=False)
    keyboard.on_release_key("space", on_space_release, suppress=False)

    print("[Aura Flow] ✓ Ready! Press Ctrl+Space to record.")
    sys.exit(app.exec())

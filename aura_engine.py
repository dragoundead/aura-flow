import os
import time
import json
import numpy as np
import sounddevice as sd
from faster_whisper import WhisperModel


class AuraEngine:
    def __init__(self, compute_type="int8"):
        self.compute_type = compute_type
        self.model = None
        self.is_recording = False
        self.audio_data = []
        self.sample_rate = 16000
        self.history_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "history.json")

    def load_model(self):
        if self.model is None:
            # Use large-v3 (NOT distil) — distil can only output English
            # large-v3 supports 99 languages including Russian
            model_name = "large-v3"
            print(f"Loading Whisper model: {model_name}...")
            self.model = WhisperModel(model_name, device="cpu", compute_type=self.compute_type)
            print("Model loaded successfully.")

    def start_recording(self):
        self.is_recording = True
        self.audio_data = []

        def callback(indata, frames, time_info, status):
            if self.is_recording:
                self.audio_data.append(indata.copy())

        try:
            self.stream = sd.InputStream(
                samplerate=self.sample_rate, 
                channels=1, 
                callback=callback,
                dtype='float32'
            )
            self.stream.start()
        except Exception as e:
            print(f"[Error] Audio stream: {e}")

    def stop_recording(self):
        self.is_recording = False
        if hasattr(self, "stream"):
            self.stream.stop()
            self.stream.close()
        if not self.audio_data:
            return None
        return np.concatenate(self.audio_data, axis=0)

    def transcribe(self, audio_np, task="translate"):
        if audio_np is None or self.model is None:
            return None
        
        audio_float = audio_np.flatten().astype(np.float32)
        
        # Check volume
        rms = np.sqrt(np.mean(audio_float**2))
        print(f"[Aura Flow] Audio RMS: {rms:.4f}")
        if rms < 0.005:
            print("[Aura Flow] Audio too quiet, skipping.")
            return None

        try:
            if task == "transcribe":
                # Russian transcription — large-v3 outputs real Russian text
                segments, info = self.model.transcribe(
                    audio_float, 
                    task="transcribe", 
                    language="ru",
                    beam_size=5,
                    vad_filter=True
                )
            else:
                # Translation: Russian speech → English text
                segments, info = self.model.transcribe(
                    audio_float, 
                    task="translate", 
                    beam_size=5,
                    vad_filter=True
                )
            
            text = " ".join(seg.text.strip() for seg in segments).strip()
            if text:
                print(f"[Aura Flow] Result ({info.language} {info.language_probability:.0%}): {text}")
                self._save_history(text, task)
            return text
        except Exception as e:
            print(f"[Error] Transcription: {e}")
            return None

    def _save_history(self, text, task):
        entry = {"timestamp": time.strftime("%Y-%m-%d %H:%M:%S"), "task": task, "text": text}
        history = []
        if os.path.exists(self.history_file):
            try:
                with open(self.history_file, "r", encoding="utf-8") as f:
                    history = json.load(f)
            except Exception:
                pass
        history.append(entry)
        with open(self.history_file, "w", encoding="utf-8") as f:
            json.dump(history, f, indent=2, ensure_ascii=False)

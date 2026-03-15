import os
import sys
import urllib.request

def download_with_progress(url, dest):
    print(f"[Aura Flow] Downloading to: {dest}")
    def progress_hook(block_num, block_size, total_size):
        downloaded = block_num * block_size
        if total_size > 0:
            percent = min(100, downloaded * 100 / total_size)
            mb_down = downloaded / (1024 * 1024)
            mb_total = total_size / (1024 * 1024)
            sys.stdout.write(f"\r[Aura Flow] {mb_down:.1f} / {mb_total:.1f} MB ({percent:.1f}%)")
            sys.stdout.flush()
    urllib.request.urlretrieve(url, dest, reporthook=progress_hook)
    print()

base_dir = os.path.dirname(os.path.abspath(__file__))
models_dir = os.path.join(base_dir, "aura-flow-cpp", "models")
os.makedirs(models_dir, exist_ok=True)

model_path = os.path.join(models_dir, "ggml-medium.bin")
if os.path.exists(model_path):
    print(f"[Aura Flow] Model already exists: {model_path}")
else:
    url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.bin"
    print("[Aura Flow] Downloading whisper medium model (~1.5 GB)...")
    download_with_progress(url, model_path)
    print(f"[Aura Flow] ✓ Model ready!")

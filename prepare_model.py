import os
import sys
import urllib.request

def download_with_progress(url, dest):
    """Download a file with a progress indicator."""
    print(f"[Aura Flow] Downloading to: {dest}")
    print(f"[Aura Flow] URL: {url}")
    
    def progress_hook(block_num, block_size, total_size):
        downloaded = block_num * block_size
        if total_size > 0:
            percent = min(100, downloaded * 100 / total_size)
            mb_down = downloaded / (1024 * 1024)
            mb_total = total_size / (1024 * 1024)
            sys.stdout.write(f"\r[Aura Flow] {mb_down:.1f} / {mb_total:.1f} MB ({percent:.1f}%)")
            sys.stdout.flush()
    
    urllib.request.urlretrieve(url, dest, reporthook=progress_hook)
    print()  # newline after progress

def prepare_model():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    cpp_dir = os.path.join(base_dir, "aura-flow-cpp")
    models_dir = os.path.join(cpp_dir, "models")
    
    if not os.path.exists(models_dir):
        os.makedirs(models_dir)

    model_path = os.path.join(models_dir, "ggml-large-v3-turbo-q8_0.bin")
    
    print("=== Aura Flow Whisper Model Preparation ===")
    
    if os.path.exists(model_path):
        size_mb = os.path.getsize(model_path) / (1024 * 1024)
        print(f"[Aura Flow] Model already exists: {model_path} ({size_mb:.1f} MB)")
        print("[Aura Flow] ✓ Model ready!")
        return
    
    # Download pre-built GGML model from Hugging Face
    url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo-q8_0.bin"
    
    print(f"[Aura Flow] Downloading whisper large-v3-turbo q8_0 model (~800 MB)...")
    print(f"[Aura Flow] This may take several minutes depending on your internet speed.")
    
    try:
        download_with_progress(url, model_path)
    except Exception as e:
        print(f"\n[Error] Download failed: {e}")
        print("[Aura Flow] You can manually download the model from:")
        print(f"  {url}")
        print(f"  Save it to: {model_path}")
        if os.path.exists(model_path):
            os.remove(model_path)  # Remove partial download
        sys.exit(1)
    
    size_mb = os.path.getsize(model_path) / (1024 * 1024)
    print(f"\n[Aura Flow] ✓ Model ready! ({size_mb:.1f} MB)")
    print(f"[Aura Flow] Location: {model_path}")

if __name__ == "__main__":
    prepare_model()

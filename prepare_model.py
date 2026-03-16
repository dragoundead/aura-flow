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
    
    opener = urllib.request.build_opener()
    opener.addheaders = [('User-agent', 'Mozilla/5.0')]
    urllib.request.install_opener(opener)
    urllib.request.urlretrieve(url, dest, reporthook=progress_hook)
    print()  # newline after progress

def prepare_model():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    cpp_dir = os.path.join(base_dir, "aura-flow-cpp")
    models_dir = os.path.join(cpp_dir, "models")
    
    if not os.path.exists(models_dir):
        os.makedirs(models_dir)

    print("=== Aura Flow Whisper Model Preparation ===")
    print("Which model would you like to use?")
    print("1) Medium (Recommended for Speed! Fast on CPU, good quality)")
    print("2) Large-v3-Turbo (Better quality, but slower on CPU)")
    
    choice = input("Select option [1-2] (default 1): ").strip() or "1"
    
    if choice == "1":
        model_name = "ggml-medium.bin"
        url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.bin"
        desc = "Medium (Fast CPU, Standard)"
    else:
        model_name = "ggml-large-v3-turbo-q8_0.bin"
        url = "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-large-v3-turbo-q8_0.bin"
        desc = "Large-v3-Turbo Q8_0 (Quality)"

    model_path = os.path.join(models_dir, model_name)
    
    if os.path.exists(model_path):
        size_mb = os.path.getsize(model_path) / (1024 * 1024)
        print(f"[Aura Flow] Model déjà exists: {model_name} ({size_mb:.1f} MB)")
        print("[Aura Flow] ✓ Model ready!")
        return
    
    print(f"[Aura Flow] Downloading {desc} (~500-800 MB)...")
    
    try:
        download_with_progress(url, model_path)
    except Exception as e:
        print(f"\n[Error] Download failed: {e}")
        print("[Aura Flow] You can manually download the model from:")
        print(f"  {url}")
        print(f"  Save it to: {model_path}")
        if os.path.exists(model_path):
            os.remove(model_path)
        sys.exit(1)
    
    print(f"\n[Aura Flow] ✓ Model ready!")
    print(f"[Aura Flow] Location: {model_path}")

if __name__ == "__main__":
    prepare_model()

from PIL import Image
import os
import sys

# Fixed paths
png_path = r"C:\Users\drago\.gemini\antigravity\brain\a78b7f2a-3775-4c81-b137-5b79b5cbb9ca\aura_flow_logo_1773562008365.png"
ico_path = r"d:\PROJECTS\AURA WISP\aura-flow-cpp\aura_flow.ico"

if not os.path.exists(png_path):
    print(f"Error: {png_path} not found")
    sys.exit(1)

try:
    img = Image.open(png_path)
    # Ensure it's square and high quality
    icon_sizes = [(16, 16), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]
    img.save(ico_path, format='ICO', sizes=icon_sizes)
    print(f"Successfully saved icon to {ico_path}")
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)

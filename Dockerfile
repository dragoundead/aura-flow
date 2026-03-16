# Dockerfile for Aura Flow C++ Engine (Linux-based)
# Note: This will not support Windows Hotkeys or UI Overlays
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libqt6widgets6 \
    libqt6core6 \
    libqt6gui6 \
    qt6-base-dev \
    libasound2-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project (Linux version)
RUN mkdir build && cd build && \
    cmake .. -DWHISPER_DIRECTML=OFF && \
    make -j$(nproc)

# Command to run (assuming a CLI mode or server mode)
# CMD ["./build/AuraFlow"]

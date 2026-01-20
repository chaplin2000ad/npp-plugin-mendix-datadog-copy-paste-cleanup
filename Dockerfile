FROM ubuntu:22.04

# Install build tools and MinGW-w64 for cross-compilation
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mingw-w64 \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set up the working directory
WORKDIR /workspace
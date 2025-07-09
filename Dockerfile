# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    libssl-dev \
    libargon2-dev \
    libwxbase3.0-dev \
    libwxgtk3.0-gtk3-dev \
    libasio-dev \
    libnlohmann-json-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy source code into the container
COPY . .

# Configure the build
RUN cmake -B build -S .

# Build the project
RUN cmake --build build

# Run tests
CMD ["ctest", "--output-on-failure", "--test-dir", "build"]

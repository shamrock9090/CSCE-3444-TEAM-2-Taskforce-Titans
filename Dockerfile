# Build stage
FROM ubuntu:22.04 AS builder

# Avoid prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install compiler, cmake and git
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the backend files
COPY backend/CMakeLists.txt ./backend/
COPY backend/main.cpp ./backend/

# Configure and build the C++ backend
RUN cmake -S backend -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

# Running stage
FROM ubuntu:22.04

# Set working directory
WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/build/homework_tracker_backend .

# Copy the frontend files so the C++ backend can serve them
COPY frontend/ ./frontend/

# Create the data folder for storage
RUN mkdir -p data

# Expose default port
EXPOSE 8080

# Run the server
CMD ["./homework_tracker_backend"]

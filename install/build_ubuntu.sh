#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Number of CPU cores for parallel builds
CPU_CORES=$(nproc)

echo "Updating package lists..."
sudo apt update

echo "Installing build dependencies..."
sudo apt install -y \
    g++ \
    cmake \
    libssl-dev \
    libboost-all-dev \
    libsasl2-dev \
    liblz4-dev \
    libzstd-dev \
    libpq-dev \
    git \
    pkg-config \
    make

echo "Installing librdkafka..."
# Install librdkafka from source
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
./configure --install-deps
make -j"$CPU_CORES"
sudo make install
sudo ldconfig
cd ..
rm -rf librdkafka

echo "Installing cppkafka..."
# Install cppkafka from source
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka
mkdir build && cd build
cmake ..
make -j"$CPU_CORES"
sudo make install
sudo ldconfig
cd ../..
rm -rf cppkafka

echo "Installing libpqxx..."
# Install libpqxx from source
git clone --recursive https://github.com/jtv/libpqxx.git
cd libpqxx
mkdir build && cd build
cmake .. -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql
make -j"$CPU_CORES"
sudo make install
sudo ldconfig
cd ../..
rm -rf libpqxx

echo "Building Jetstream application..."
# Build Jetstream
if [ -d "jetstream" ]; then
    cd jetstream
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make -j"$CPU_CORES"
    echo "Build complete. You can now run ./jetstream in the jetstream/build directory."
else
    echo "Error: jetstream directory not found."
    exit 1
fi

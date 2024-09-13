#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Number of CPU cores for parallel builds
CPU_CORES=$(sysctl -n hw.ncpu)

echo "Installing Xcode Command Line Tools..."
# Install Xcode Command Line Tools if not already installed
if ! xcode-select -p &>/dev/null; then
    xcode-select --install
    # Wait until the Command Line Tools are installed
    until xcode-select -p &>/dev/null; do
        sleep 5
    done
else
    echo "Xcode Command Line Tools already installed."
fi

echo "Installing Homebrew..."
# Install Homebrew if not already installed
if ! command -v brew &>/dev/null; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    # Add Homebrew to the PATH
    eval "$(/opt/homebrew/bin/brew shellenv)" 2>/dev/null || eval "$(/usr/local/bin/brew shellenv)"
else
    echo "Homebrew already installed."
fi

echo "Updating Homebrew..."
brew update

echo "Installing dependencies with Homebrew..."
brew install \
    cmake \
    openssl \
    boost \
    cyrus-sasl \
    lz4 \
    zstd \
    libpq \
    git \
    pkg-config \
    librdkafka

echo "Setting OpenSSL environment variables..."
# Set environment variables for OpenSSL
export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
export OPENSSL_LIBRARIES="$OPENSSL_ROOT_DIR/lib"
export OPENSSL_INCLUDE_DIR="$OPENSSL_ROOT_DIR/include"

echo "Installing cppkafka..."
# Install cppkafka from source
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka

mkdir build && cd build
cmake .. -DOPENSSL_ROOT_DIR="$OPENSSL_ROOT_DIR"
make -j"$CPU_CORES"
sudo make install

cd ../..
rm -rf cppkafka

echo "Installing libpqxx..."
# Install libpqxx from source
git clone --recursive https://github.com/jtv/libpqxx.git
cd libpqxx

mkdir build && cd build
cmake .. -DPostgreSQL_TYPE_INCLUDE_DIR="$(brew --prefix libpq)/include" -DPostgreSQL_TYPE_LIBRARY_DIR="$(brew --prefix libpq)/lib"
make -j"$CPU_CORES"
sudo make install

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

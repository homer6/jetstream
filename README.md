# Jetstream

**Jetstream** is a general application framework written in modern C++. It provides a robust set of features for building scalable, high-performance applications with support for Kafka, HTTP, ElasticSearch, TypeSense, Loggly/Logz.io, and more.


## Table of Contents

- [Features](#features)
- [Roadmap](#roadmap)
- [Performance](#performance)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Automated Setup Script for macOS](#automated-setup-script-for-macos)
  - [Building on Linux](#building-on-linux)
- [Building with Docker](#building-with-docker)
- [Running Jetstream](#running-jetstream)
  - [Running with Docker](#running-with-docker)
  - [Running On-Premises](#running-on-premises)
- [Debugging](#debugging)
- [Additional Tools](#additional-tools)
  - [Installing Elasticsearch (Ubuntu)](#installing-elasticsearch-ubuntu)
- [Development](#development)
  - [Run Analysis](#run-analysis)
  - [Run API Locally](#run-api-locally)
- [License](#license)


## Features

1. Acts as a persistent Kafka producer/consumer
2. HTTP to Kafka functionality
3. Kafka to HTTP with batching support
4. Sink data to ElasticSearch
5. Sink data to TypeSense
6. Sink data to Loggly/Logz.io
7. HTTP API for interaction and control
8. HTTP web server capabilities
9. HTTP client functionality
10. Prometheus exporter for metrics
11. Parallelized XML/JSON/CSV parser with thread pooling
12. Data augmentation capabilities
13. PostgreSQL client integration
14. TypeSense client integration
15. Federated search and relational DB queries via a single HTTP call (GraphQL-like)
16. Event/message router to split Kafka events to multiple HTTP endpoints with batching

Jetstream optionally works with [Logport](https://github.com/homer6/logport), which watches log files and sends changes to Kafka (one line per message). Logport enables your applications to easily ship logs to Kafka as they are written and log-rotated.


## Roadmap

1. WebSocket support
2. RocksDB support for local storage
3. S3 sink and reader/writer functionality
4. HTML templates with CrowCPP integration
5. Web crawler implementation
6. LLM client support (OpenAI integration)


## Performance

- **Message Processing:** Each consumer instance processes around 10,000 messages per second per partition.
- **Memory Usage:** Approximately 50 MB of runtime memory.
- **CPU Usage:** Typically below 3%, depending on the workload.


## Installation

### Prerequisites

- **Operating System:** macOS or Linux
- **C++ Compiler:** Support for C++17 (e.g., `g++`, `clang++`)
- **Build Tools:** `cmake`, `make`
- **Package Manager:** Homebrew (for macOS), `apt` (for Ubuntu/Debian)

### Automated Setup Script for macOS

For macOS users, an automated script is provided to install all dependencies and build Jetstream.

#### **1. Download the Setup Script**

Save the following script as `setup_jetstream.sh` in your preferred directory:

```bash
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
```

#### **2. Prepare the Jetstream Source Code**

Ensure that your `jetstream` source code directory exists and is located in the same directory as the `setup_jetstream.sh` script. Your directory structure should look like this:

```
your-directory/
├── jetstream/
│   ├── [jetstream source files]
├── setup_jetstream.sh
```

#### **3. Make the Script Executable**

Open Terminal, navigate to the directory containing the script, and run:

```bash
chmod +x setup_jetstream.sh
```

#### **4. Run the Script**

Execute the script with the following command:

```bash
./setup_jetstream.sh
```

**Note:** You may be prompted to enter your password during the installation process for commands that require `sudo`.

---

### Building on Linux

For Linux users, follow these steps to build Jetstream:

#### **1. Install Dependencies**

```bash
sudo apt update
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
    pkg-config
```

#### **2. Install `librdkafka`**

```bash
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
./configure --install-deps
make -j$(nproc)
sudo make install
sudo ldconfig
cd ..
```

#### **3. Install `cppkafka`**

```bash
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig
cd ../..
```

#### **4. Install `libpqxx`**

```bash
git clone --recursive https://github.com/jtv/libpqxx.git
cd libpqxx
mkdir build && cd build
cmake .. -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql
make -j$(nproc)
sudo make install
sudo ldconfig
cd ../..
```

#### **5. Build Jetstream**

```bash
git clone https://github.com/homer6/jetstream.git
cd jetstream
mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

## Building with Docker

You can build Jetstream using Docker to containerize the application.

```bash
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build -t jetstream:latest .
```

To build and push the Docker image:

```bash
make
docker build -t jetstream:latest -t homer6/jetstream:latest .
docker push homer6/jetstream:latest
```

---

## Running Jetstream

### Running with Docker

Example for sending logs to Loggly:

```bash
docker run -d \
    --restart unless-stopped \
    \
    --env LOGPORT_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env LOGPORT_TOPIC=my_logs_logger \
    --env LOGPORT_PRODUCT_CODE=prd4096 \
    --env LOGPORT_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env JETSTREAM_CONSUMER_GROUP=prd4096_mylogs \
    --env JETSTREAM_TOPIC=my_logs \
    --env JETSTREAM_PRODUCT_CODE=prd4096 \
    --env JETSTREAM_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_DESTINATION_TOKEN=my_loggly_token \
    \
    homer6/jetstream:latest loggly
```

**Note:**

- **LOGPORT_TOPIC and JETSTREAM_TOPIC cannot be the same** to avoid feedback loops.
- **LOGPORT** is used to ship Jetstream's own logs to Kafka.

### Running On-Premises

To run Jetstream and sink data to ElasticSearch:

```bash
./jetstream elasticsearch -t my_logs 192.168.1.91:9200
```

---

## Debugging

To debug Jetstream using GDB within Docker:

1. Modify the Dockerfile:

   - Change the `CMD` or `ENTRYPOINT` to `/bin/bash`.
   - Optionally, switch the base image to Ubuntu.
   - Install GDB by adding `RUN apt-get update && apt-get install -y gdb` in the Dockerfile.

2. Build the Docker image:

   ```bash
   docker build -t jetstream_debug:latest .
   ```

3. Run the Docker container with debugging capabilities:

   ```bash
   docker run -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
       --env JETSTREAM_TOPIC=my_logs jetstream_debug:latest
   ```

4. Inside the container, start GDB:

   ```bash
   gdb --args ./jetstream elasticsearch 127.0.0.1:9200
   ```

---

## Additional Tools

### Installing Elasticsearch (Ubuntu)

```bash
wget -qO - https://artifacts.elastic.co/GPG-KEY-elasticsearch | sudo apt-key add -
sudo apt-get install apt-transport-https
echo "deb https://artifacts.elastic.co/packages/7.x/apt stable main" | sudo tee -a /etc/apt/sources.list.d/elastic-7.x.list
sudo apt-get update && sudo apt-get install elasticsearch
sudo systemctl enable elasticsearch
sudo systemctl start elasticsearch
```

---

## Development

### Run Analysis

To run a data job and output samples:

```bash
mkdir analysis
make -j"$(nproc)" && time ./jetstream data-job-1 /archive/data > analysis/samples.txt
```

### Run API Locally

To run the API server locally:

```bash
make -j"$(nproc)" && ./jetstream api-server
```

---

## License

Jetstream is released under the [MIT License](LICENSE).

---

By following this guide, you should be able to set up, build, and run Jetstream on your macOS or Linux system efficiently. If you encounter any issues or have suggestions for improvement, please feel free to reach out through the project's [GitHub repository](https://github.com/homer6/jetstream/issues).


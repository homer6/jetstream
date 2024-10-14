# Base image with build dependencies
FROM ubuntu:22.04 AS build

# Install necessary build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libboost-all-dev \
    libsasl2-dev \
    liblz4-dev \
    libzstd-dev \
    libpq-dev \
    autoconf \
    libtool \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the current directory contents into the container
COPY . .

# Initialize and update submodules
RUN git submodule update --init --recursive

# Build and install librdkafka
WORKDIR /app/dependencies/librdkafka
RUN ./configure --install-deps && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build and install cppkafka
WORKDIR /app/dependencies/cppkafka
RUN cmake -DCPPKAFKA_BUILD_SHARED=ON -DCPPKAFKA_DISABLE_TESTS=ON . && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build and install cyrus-sasl
WORKDIR /app/dependencies/cyrus-sasl
RUN ./autogen.sh && \
    ./configure --prefix=/usr && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Install pre-requisites for kubernetes-c
RUN apt-get update && apt-get install -y \
    libssl-dev \
    libcurl4-openssl-dev \
    libwebsockets-dev \
    uncrustify

# Build and install libyaml
WORKDIR /app/dependencies/libyaml
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build kubernetes-c
WORKDIR /app/dependencies/kubernetes-c/kubernetes
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Add dependencies for kubepp
RUN apt-get update && apt-get install -y \
    libspdlog-dev \
    libfmt-dev

# Build and install kubepp
WORKDIR /app/dependencies/kubepp
RUN cmake . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_PREFIX_PATH="/usr/lib/x86_64-linux-gnu/cmake/fmt;/usr/lib/x86_64-linux-gnu/cmake/spdlog" && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build and install libpqxx
WORKDIR /app/dependencies/libpqxx
RUN mkdir build && cd build && \
    cmake -DPostgreSQL_TYPE_INCLUDE_DIR:STRING=/usr/include/postgresql .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig

# Build Jetstream
WORKDIR /app
RUN cmake . && make -j$(nproc)

# Create final lightweight image
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl3 \
    libboost-system1.74.0 \
    libboost-filesystem1.74.0 \
    libsasl2-2 \
    liblz4-1 \
    libzstd1 \
    libpq5 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries and necessary files from build stage
COPY --from=build /usr/local/lib /usr/local/lib
COPY --from=build /app/build/jetstream /usr/local/bin/jetstream

# Update library cache
RUN ldconfig

# Set the working directory
WORKDIR /app

# Set the entrypoint to the Jetstream executable
ENTRYPOINT ["jetstream"]
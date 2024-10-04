FROM --platform=linux/amd64 ubuntu:22

# Update package lists and install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        g++ \
        cmake \
        libssl-dev \
        libboost-all-dev \
        libsasl2-dev \
        liblz4-dev \
        libzstd-dev \
        libpq-dev \
        git \
        make \
        pkg-config && \
    rm -rf /var/lib/apt/lists/*


# Install librdkafka
RUN git clone https://github.com/edenhill/librdkafka.git && \
    cd librdkafka && \
    ./configure --install-deps && \
    make -j"$(nproc)" && \
    make install && \
    ldconfig && \
    cd ..
    #cd .. && rm -rf librdkafka

# Install cppkafka
RUN git clone https://github.com/mfontanini/cppkafka.git && \
    cd cppkafka && \
    cmake . && \
    make -j"$(nproc)" && \
    make install && \
    ldconfig && \
    cd .. 
    #cd .. && rm -rf cppkafka

# Install libpqxx
RUN git clone --recursive https://github.com/jtv/libpqxx.git && \
    cd libpqxx && \
    rm -rf build && mkdir build && cd build && \
    #cmake -DPostgreSQL_TYPE_INCLUDE_DIR:STRING=/usr/include/postgresql .. && \
    cmake .. && \
    make -j"$(nproc)" && \
    make install && \
    ldconfig && \
    cd ../../
    # cd ../../ && rm -rf libpqxx


# Copy jetstream source code into the container
COPY jetstream/ /jetstream

# Build jetstream
WORKDIR /jetstream

RUN rm -rf CMakeCache.txt CMakeFiles/ Makefile cmake_install.cmake cmake-build-debug

RUN cmake . && \
    make -j"$(nproc)"

# Set the entrypoint to run jetstream
CMD ["./jetstream"]

# Building Jetstream and Dependencies from Source

This guide assumes:

- You have a C++17-compatible compiler (e.g., GCC 7+ or Clang 7+).
- You have `cmake`, `autoconf`, `automake`, `libtool`, and `pkg-config` installed.
- You have `sudo` privileges to install to system directories (e.g., `/usr/local`).

No prebuilt packages are needed; all dependencies are built from the submodules included in this repository.

## Step 1: Clone and Update Submodules

```sh
git clone https://github.com/homer6/jetstream.git
cd jetstream
git submodule update --init --recursive
```

This fetches the following dependencies as submodules in `dependencies/`:

- cyrus-sasl
- librdkafka
- cppkafka
- libpqxx
- kubernetes-c
- kubepp
- libyaml

## Step 2: Build and Install Dependencies

You will build and install each dependency into the default prefix `/usr/local`. This typically requires `sudo make install`.

### cyrus-sasl

```sh
cd dependencies/cyrus-sasl
./autogen.sh
./configure
make -j$(nproc)
sudo make install
git clean -fd
cd ../../
```

### librdkafka

```sh
cd dependencies/librdkafka
./configure
make -j$(nproc)
sudo make install
git checkout CONFIGURATION.md
cd ../../
```

### boost

```sh
cd dependencies/boost
git submodule update --init --recursive
./bootstrap.sh --prefix=/usr/local
./b2 -j$(nproc) --with-filesystem --with-system
sudo ./b2 install --with-filesystem --with-system
./b2 --show-libraries
cd ../../
```

### cppkafka

```sh
cd dependencies/cppkafka
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
cd ../../../
```

### libpqxx

```sh
cd dependencies/libpqxx
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
cd ..
rm -rf build
cd ../../
```

### spdlog

```sh
cd dependencies/spdlog
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
cd ../../
```

### libwebsockets

```sh
cd dependencies/libwebsockets
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DLWS_INSTALL_CMAKE_DIR=/usr/local/lib/cmake/libwebsockets ..
make -j$(nproc)
sudo make install
cd ../../..
```

### uncrustify

```sh
cd dependencies/uncrustify
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
cd ../../..
```

### libyaml

```sh
cd dependencies/libyaml
./bootstrap

mkdir build && cd build
cmake -DCMAKE_C_FLAGS="-Wall -Wextra -fPIC" -DCMAKE_INSTALL_PREFIX=/usr/local -DYAML_INSTALL_CMAKE_DIR=/usr/local/lib/cmake/yaml -DCMAKE_INSTALL_CMAKEDIR=/usr/local/lib/cmake/yaml ..
make -j$(nproc)
sudo make install
cd ..
rm -rf build
cd ../../
```

### kubernetes-c

```sh
# sudo apt-get install libssl-dev libcurl4-openssl-dev libwebsockets-dev uncrustify
cd dependencies/kubernetes-c/kubernetes
cmake -DCMAKE_INSTALL_PREFIX=/usr/local .
# If you want to use `gdb` to debug the C client library, add `-DCMAKE_BUILD_TYPE=Debug` to the cmake command line, e.g.
# cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
git clean -fd
cd ../../../
```


### kubepp

```sh
cd dependencies/kubepp/build
cmake ..
make -j$(nproc)
sudo make install
cd ../../../
```


At this point, all dependencies are installed system-wide.

## Step 3: Build and Install Jetstream

Once all dependencies are in place:

```sh
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

Jetstream and its dependencies should now be installed into `/usr/local`. You can run and link against Jetstream just like any other system-installed library.


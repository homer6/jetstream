from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, rmdir
import os

class JetstreamConan(ConanFile):
    name = "jetstream"
    version = "0.1.0"
    license = "MIT License"
    author = "Steve Sperandeo"
    url = "https://github.com/homer6/jetstream"
    description = "Jetstream is a C++ library for building high-performance, real-time streaming applications."
    topics = ("jetstream", "streaming", "kafka", "kubernetes", "postgresql")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "CMakeDeps", "CMakeToolchain"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("openssl/1.1.1q")
        self.requires("boost/1.81.0")
        self.requires("lz4/1.9.4")
        self.requires("zstd/1.5.2")
        self.requires("libpq/14.5")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")

    def source(self):
        # Initialize and update submodules
        self.run("git submodule update --init --recursive")

    def build(self):
        # Build libsasl2
        with self.chdir("dependencies/cyrus-sasl"):
            self.run("./autogen.sh")
            self.run("./configure --prefix=/usr/local")
            self.run("make -j$(nproc)")
            self.run("make install")

        # Build librdkafka
        with self.chdir("dependencies/librdkafka"):
            self.run("./configure")
            self.run("make -j$(nproc)")
            self.run("make install")

        # Build cppkafka
        with self.chdir("dependencies/cppkafka"):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            cmake.install()

        # Build libpqxx
        with self.chdir("dependencies/libpqxx"):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            cmake.install()

        # Build kubepp
        with self.chdir("dependencies/kubepp"):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            cmake.install()

        # Build kubernetes-c
        with self.chdir("dependencies/kubernetes-c"):
            cmake = CMake(self)
            cmake.configure()
            cmake.build()
            cmake.install()

        # Build Jetstream
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.libs = ["jetstream"]
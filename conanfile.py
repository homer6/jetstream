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

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")

    def requirements(self):
        self.requires("openssl/1.1.1q")
        self.requires("boost/1.81.0")
        self.requires("libsasl2/2.1.28")
        self.requires("lz4/1.9.4")
        self.requires("zstd/1.5.2")
        self.requires("libpq/14.5")

        # Local dependencies
        self.requires("librdkafka/2.1.1", headers=True, run=True)
        self.requires("cppkafka/0.4.0", headers=True, run=True)
        self.requires("kubepp/0.1.0", headers=True, run=True)
        self.requires("kubernetes-c/0.9.0", headers=True, run=True)
        self.requires("libpqxx/7.7.4", headers=True, run=True)

    def source(self):
        # Assuming the Jetstream source is in the same directory as conanfile.py
        # If not, adjust the source method accordingly
        copy(self, "*", self.source_folder, self.build_folder)

    def build(self):
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

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")
        
        # Build local dependencies
        self.build_requires("librdkafka/2.1.1", force_host_context=True)
        self.build_requires("cppkafka/0.4.0", force_host_context=True)
        self.build_requires("kubepp/0.1.0", force_host_context=True)
        self.build_requires("kubernetes-c/0.9.0", force_host_context=True)
        self.build_requires("libpqxx/7.7.4", force_host_context=True)

    def build(self):
        # Build local dependencies
        # self.run("conan create ../dependencies/librdkafka")
        # self.run("conan create ../dependencies/cppkafka")
        # self.run("conan create ../dependencies/kubepp")
        # self.run("conan create ../dependencies/kubernetes-c")
        # self.run("conan create ../dependencies/libpqxx")

        # Build Jetstream
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
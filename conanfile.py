from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, rmdir, chdir
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
        # Build cyrus-sasl
        cyrus_sasl_dir = os.path.join(self.source_folder, "dependencies", "cyrus-sasl")
        cyrus_sasl_prefix = os.path.join(self.build_folder, "cyrus-sasl-install")
        with chdir(self, cyrus_sasl_dir):
            self.run("./autogen.sh")
            self.run(f"./configure --prefix={cyrus_sasl_prefix}")
            self.run("make -j$(nproc)")
            self.run("make install")

        # Build librdkafka
        librdkafka_dir = os.path.join(self.source_folder, "dependencies", "librdkafka")
        librdkafka_prefix = os.path.join(self.build_folder, "librdkafka-install")
        with chdir(self, librdkafka_dir):
            self.run(f"./configure --prefix={librdkafka_prefix}")
            self.run("make -j$(nproc)")
            self.run("make install")

        from conan.tools.env import Environment
        env = Environment()
        pkgconfig_path_rdkafka = os.path.join(librdkafka_prefix, "lib", "pkgconfig")
        env.prepend_path("PKG_CONFIG_PATH", pkgconfig_path_rdkafka)

        # Build cppkafka
        cppkafka_dir = os.path.join(self.source_folder, "dependencies", "cppkafka")
        cppkafka_prefix = os.path.join(self.build_folder, "cppkafka-install")
        with chdir(self, cppkafka_dir):
            with env.vars(self).apply():
                cmake = CMake(self)
                cmake.configure(variables={"CMAKE_INSTALL_PREFIX": cppkafka_prefix})
                cmake.build()
                cmake.install()

        # Now that cppkafka is installed, add its prefix to CMAKE_PREFIX_PATH
        # This helps Jetstream's CMake find cppkafka.
        env.prepend_path("CMAKE_PREFIX_PATH", cppkafka_prefix)
        
        # Build libpqxx
        libpqxx_dir = os.path.join(self.source_folder, "dependencies", "libpqxx")
        libpqxx_prefix = os.path.join(self.build_folder, "libpqxx-install")
        with chdir(self, libpqxx_dir):
            with env.vars(self).apply():
                cmake = CMake(self)
                cmake.configure(variables={"CMAKE_INSTALL_PREFIX": libpqxx_prefix})
                cmake.build()
                cmake.install()

        # Similarly for kubepp, kubernetes-c
        kubepp_dir = os.path.join(self.source_folder, "dependencies", "kubepp")
        kubepp_prefix = os.path.join(self.build_folder, "kubepp-install")
        with chdir(self, kubepp_dir):
            with env.vars(self).apply():
                cmake = CMake(self)
                cmake.configure(variables={"CMAKE_INSTALL_PREFIX": kubepp_prefix})
                cmake.build()
                cmake.install()

        kubernetes_c_dir = os.path.join(self.source_folder, "dependencies", "kubernetes-c")
        kubernetes_c_prefix = os.path.join(self.build_folder, "kubernetes-c-install")
        with chdir(self, kubernetes_c_dir):
            with env.vars(self).apply():
                cmake = CMake(self)
                cmake.configure(variables={"CMAKE_INSTALL_PREFIX": kubernetes_c_prefix})
                cmake.build()
                cmake.install()

        # Finally, build Jetstream itself
        with env.vars(self).apply():
            cmake = CMake(self)
            cmake.configure()  # Now it can find cppkafka due to CMAKE_PREFIX_PATH
            cmake.build()
            cmake.install()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.libs = ["jetstream"]

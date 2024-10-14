
git submodule update --init --recursive

pip install conan

conan profile detect

conan install . --build=missing
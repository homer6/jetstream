
git submodule update --init --recursive

pip install conan

conan profile detect



conan source .
conan install . --build=missing
conan build .
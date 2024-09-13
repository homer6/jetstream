# Installing Jetstream on Ubuntu

This guide provides detailed instructions on how to set up and build the **Jetstream** application on Ubuntu.


## Prerequisites

- **Operating System**: Ubuntu 18.04 or later
- **Administrative Privileges**: Required for installing packages and libraries
- **Internet Connection**: Needed to download packages and clone repositories


## Steps

### 1. Clone the Jetstream Repository

Open Terminal and clone the Jetstream repository:

```bash
git clone https://github.com/homer6/jetstream.git
cd jetstream
```

### 2. Run the Setup Script

An automated script is provided to install all dependencies and build Jetstream.

#### Option 1: Run the Script Directly

Download and execute the script:

```bash
curl -O https://raw.githubusercontent.com/homer6/jetstream/master/install/build_ubuntu.sh
chmod +x build_ubuntu.sh
./build_ubuntu.sh
```

#### Option 2: Run the Script from the `install` Directory

If you have the repository cloned, navigate to the `install` directory and run the script:

```bash
cd install
chmod +x build_ubuntu.sh
./build_ubuntu.sh
```

**Note**: You may be prompted to enter your password during the installation process for commands that require `sudo`.

### 3. After Installation

Once the script completes successfully:

- The Jetstream application will be built and located in `jetstream/build/jetstream`.
- You can now run the application using:

  ```bash
  cd ../build
  ./jetstream
  ```



## Troubleshooting

- **Library Not Found Errors**:
  - Ensure that all dependencies are installed.
  - Verify that the libraries are in the correct locations.
- **Permission Denied Errors**:
  - Make sure you have the necessary permissions to install software.
  - Run the script with `sudo` if necessary.
- **Compilation Errors**:
  - Ensure you have a compatible version of `g++` that supports C++17.


## Additional Information

- **Cleanup**:
  - The script removes cloned repositories after installation to keep your workspace clean.
- **Customizing the Build**:
  - You can modify the `build_ubuntu.sh` script to suit your specific needs, such as changing installation directories or adding additional build options.


## Contact Information

For further assistance, support, or to report issues:

- **Email**: support@jetstreamapp.com
- **Issue Tracker**: [Jetstream GitHub Issues](https://github.com/homer6/jetstream/issues)
- **Documentation**: Refer to the official Jetstream documentation for more details.

---

**Disclaimer**: This script and instructions are provided "as is" without warranty of any kind. Use at your own risk.

# README

# Jetstream Setup Script for macOS

This README provides detailed instructions on how to set up and build the **Jetstream** application on macOS using the provided `setup_jetstream.sh` script. The script automates the installation of all necessary dependencies and the compilation of the Jetstream application, simplifying the setup process.

---

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Script Overview](#script-overview)
- [Installation Steps](#installation-steps)
  - [1. Download the Script](#1-download-the-script)
  - [2. Prepare the Jetstream Source Code](#2-prepare-the-jetstream-source-code)
  - [3. Make the Script Executable](#3-make-the-script-executable)
  - [4. Run the Script](#4-run-the-script)
- [Running the Jetstream Application](#running-the-jetstream-application)
- [Additional Information](#additional-information)
  - [Xcode Command Line Tools](#xcode-command-line-tools)
  - [Homebrew Installation](#homebrew-installation)
  - [Environment Variables](#environment-variables)
  - [CPU Core Utilization](#cpu-core-utilization)
  - [Permissions](#permissions)
  - [Cleanup](#cleanup)
  - [Error Handling](#error-handling)
  - [Customizing the Script](#customizing-the-script)
- [Troubleshooting](#troubleshooting)
- [Contact Information](#contact-information)

---

## Introduction

The `setup_jetstream.sh` script is designed to automate the setup process for the Jetstream application on macOS. It handles the installation of required packages, cloning and building of necessary libraries, and the compilation of the Jetstream application itself.

## Prerequisites

- **Operating System:** macOS
- **Administrative Privileges:** Required for installing packages and libraries
- **Internet Connection:** Needed to download packages and clone repositories

## Script Overview

The script performs the following actions:

1. **Xcode Command Line Tools:** Installs if not already present.
2. **Homebrew:** Installs if not already present and updates it.
3. **Dependencies Installation:** Installs required packages via Homebrew.
4. **Environment Variables:** Sets necessary environment variables for OpenSSL.
5. **Library Installation:**
   - Clones and installs `librdkafka`.
   - Clones and installs `cppkafka`.
   - Clones and installs `libpqxx`.
6. **Jetstream Compilation:** Builds the Jetstream application.
7. **Cleanup:** Removes cloned repositories to tidy up the workspace.

## Installation Steps

### 1. Run the Script

Save the script below as `build_macos.sh` in your preferred directory.


### 2. Prepare the Jetstream Source Code

Ensure that your `jetstream` source code directory exists and is located in the same directory as the `setup_jetstream.sh` script. Your directory structure should look like this:

```
your-directory/
├── jetstream/
│   ├── [jetstream source files]
├── setup_jetstream.sh
```

### 3. Make the Script Executable

Open Terminal, navigate to the directory containing the script, and run:

```bash
chmod +x setup_jetstream.sh
```

### 4. Run the Script

Execute the script with the following command:

```bash
./setup_jetstream.sh
```

**Note:** You may be prompted to enter your password during the installation process for commands that require `sudo`.

---

## Running the Jetstream Application

After the script completes successfully, you can run the Jetstream application:

1. Navigate to the `jetstream` directory (if not already there):

   ```bash
   cd jetstream
   ```

2. Run the Jetstream application:

   ```bash
   ./jetstream
   ```

---

## Additional Information

### Xcode Command Line Tools

- **Installation Check:** The script verifies if the Xcode Command Line Tools are installed.
- **Automatic Installation:** If not installed, it triggers the installation process.
- **User Interaction:** You might need to accept a prompt or agreement during installation.
- **Wait Mechanism:** The script waits until the tools are fully installed before proceeding.

### Homebrew Installation

- **Installation Check:** The script checks if Homebrew is installed.
- **Automatic Installation:** Installs Homebrew if missing.
- **PATH Adjustment:** Adds Homebrew to the `PATH` for the current session.
- **Apple Silicon Support:** Accounts for installation paths on M1/M2 Macs.

### Environment Variables

- **OpenSSL Variables:** Sets `OPENSSL_ROOT_DIR`, `OPENSSL_LIBRARIES`, and `OPENSSL_INCLUDE_DIR`.
- **Purpose:** Ensures that build systems locate the correct OpenSSL libraries provided by Homebrew.
- **Conflicts:** Adjust these variables if you have other OpenSSL installations.

### CPU Core Utilization

- **Parallel Builds:** Detects the number of CPU cores to speed up compilation.
- **Command Used:** `sysctl -n hw.ncpu`
- **Benefit:** Reduces build times for large codebases.

### Permissions

- **Administrative Rights:** `sudo` is required for certain installation steps.
- **User Prompts:** You will be prompted for your password during `sudo` operations.
- **Security:** Ensure you trust the sources before entering your password.

### Cleanup

- **Repository Removal:** Cloned repositories are deleted after installation.
- **Workspace Tidiness:** Keeps your directory clean and free of unnecessary files.
- **Optional:** Modify the script if you wish to keep the repositories.

### Error Handling

- **Immediate Exit:** The script uses `set -e` to exit on any command failure.
- **Error Messages:** Provides error messages for troubleshooting.
- **Build Verification:** Checks if the `jetstream` directory exists before attempting to build.

### Customizing the Script

- **Directory Paths:** Modify the script if your `jetstream` directory is in a different location.
- **Additional Libraries:** Add any extra dependencies or libraries as needed.
- **Build Options:** Tweak `cmake` and `make` commands to suit your build requirements.

---

## Troubleshooting

- **Installation Failures:**
  - Ensure you have a stable internet connection.
  - Verify that you have sufficient permissions.
- **Dependency Issues:**
  - Conflicting versions of libraries may cause build failures.
  - Use `brew doctor` to check for issues with your Homebrew installation.
- **Build Errors:**
  - Check the output logs for specific error messages.
  - Ensure all environment variables are correctly set.
- **Command Not Found Errors:**
  - Verify that all dependencies are installed.
  - Make sure that `/usr/local/bin` or `/opt/homebrew/bin` is in your `PATH`.
- **Permission Denied Errors:**
  - Confirm that you have executed the script with the necessary permissions.
  - Check file and directory permissions if you encounter access issues.

---

## Contact Information

For further assistance, support, or to report issues:

- **Email:** support@jetstreamapp.com
- **Issue Tracker:** [Jetstream GitHub Issues](https://github.com/yourusername/jetstream/issues)
- **Documentation:** Refer to the official Jetstream documentation for more details.

---

**Disclaimer:** This script and instructions are provided "as is" without warranty of any kind. Use at your own risk.

---

By following this guide, you should be able to set up and run the Jetstream application on your macOS system efficiently. If you encounter any issues or have suggestions for improvement, please feel free to reach out through the contact information provided.
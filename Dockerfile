# Use an official Ubuntu as a parent image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        python3-pip && \
    pip3 install conan && \
    rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the jetstream source code and dependencies
COPY . .

# Install packages using Conan
RUN conan install . --build=missing

# Build the project
RUN conan build .

# Expose the port (if applicable)
EXPOSE 8080

# Command to run the application
CMD ["./build/bin/jetstream"]
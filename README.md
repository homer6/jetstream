# Jetstream

Jetstream is a modern C++ application framework that provides a robust set of features for building scalable, high-performance applications. It offers support for Kafka, HTTP, ElasticSearch, TypeSense, Loggly/Logz.io, and more.

## Table of Contents

- [Features](#features)
- [Roadmap](#roadmap)
- [Performance](#performance)
- [Installation](#installation)
- [Building with Docker](#building-with-docker)
- [Running Jetstream](#running-jetstream)
- [Debugging](#debugging)
- [Development](#development)
- [License](#license)

---

## Features

1. **Persistent Kafka Producer/Consumer**: Acts as a reliable Kafka producer and consumer.
2. **HTTP to Kafka**: Transforms HTTP requests into Kafka messages.
3. **Kafka to HTTP with Batching**: Processes Kafka messages and sends them to HTTP endpoints with support for batching.
4. **ElasticSearch Sink**: Writes data to ElasticSearch clusters.
5. **TypeSense Sink**: Integrates with TypeSense for search capabilities.
6. **Loggly/Logz.io Sink**: Sends logs to Loggly or Logz.io for centralized logging.
7. **HTTP API**: Provides an HTTP API for interaction and control.
8. **HTTP Web Server**: Serves web content over HTTP.
9. **HTTP Client**: Makes HTTP requests to other services.
10. **Prometheus Exporter**: Exposes metrics for monitoring with Prometheus.
11. **Parallelized Parsing**: Parses XML/JSON/CSV data using a thread pool for high performance.
12. **Data Augmentation**: Enhances data with additional information.
13. **PostgreSQL Client**: Connects and interacts with PostgreSQL databases.
14. **TypeSense Client**: Interfaces with TypeSense for advanced search features.
15. **Federated Search and Queries**: Combines search and relational database queries into a single HTTP call (GraphQL-like functionality).
16. **Event/Message Router**: Routes Kafka events to multiple HTTP endpoints with batching support.

Jetstream optionally works with [Logport](https://github.com/homer6/logport), which monitors log files and sends changes to Kafka (one line per message). Logport enables your applications to easily ship logs to Kafka as they are written and log-rotated.

---

## Roadmap

1. **WebSocket Support**: Enable real-time communication using WebSockets.
2. **RocksDB Support**: Integrate RocksDB for local storage solutions.
3. **S3 Sink and Reader/Writer**: Add support for Amazon S3 as a data sink and source.
4. **HTML Templates with CrowCPP**: Incorporate CrowCPP for serving HTML templates.
5. **Web Crawler Implementation**: Develop a web crawler for data collection.
6. **LLM Client Support**: Integrate with OpenAI's Large Language Models.

---

## Performance

- **Message Processing**: Processes approximately **10,000 messages per second per partition**.
- **Memory Usage**: Utilizes around **50 MB** of runtime memory.
- **CPU Usage**: Typically consumes **below 3% CPU**, depending on the workload.

---

## Installation

For detailed installation instructions, please refer to the following guides:

- [Installation on macOS](install/build_macos.md)
- [Installation on Ubuntu](install/build_ubuntu.md)

---

## Building with Docker

You can build Jetstream using Docker to containerize the application:

```bash
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build -t jetstream:latest .
```

To build and push the Docker image:

```bash
make
docker build -t jetstream:latest -t homer6/jetstream:latest .
docker push homer6/jetstream:latest
```

---

## Running Jetstream

### Running with Docker

Example for sending logs to **Loggly**:

```bash
docker run -d \
    --restart unless-stopped \
    \
    --env LOGPORT_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env LOGPORT_TOPIC=my_logs_logger \
    --env LOGPORT_PRODUCT_CODE=prd4096 \
    --env LOGPORT_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env JETSTREAM_CONSUMER_GROUP=prd4096_mylogs \
    --env JETSTREAM_TOPIC=my_logs \
    --env JETSTREAM_PRODUCT_CODE=prd4096 \
    --env JETSTREAM_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_DESTINATION_TOKEN=my_loggly_token \
    \
    homer6/jetstream:latest loggly
```

**Notes:**

- **LOGPORT_TOPIC** and **JETSTREAM_TOPIC** must be different to avoid feedback loops.
- **LOGPORT** is used to ship Jetstream's own logs to Kafka.

### Running On-Premises

To run Jetstream and sink data to ElasticSearch:

```bash
./jetstream elasticsearch -t my_logs 192.168.1.91:9200
```

---

## Debugging

To debug Jetstream using GDB within Docker:

1. **Modify the Dockerfile**:

   - Change the `CMD` or `ENTRYPOINT` to `/bin/bash`.
   - Optionally, switch the base image to Ubuntu.
   - Install GDB by adding `RUN apt-get update && apt-get install -y gdb` to the Dockerfile.

2. **Build the Docker Image**:

   ```bash
   docker build -t jetstream_debug:latest .
   ```

3. **Run the Docker Container with Debugging Capabilities**:

   ```bash
   docker run -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
       --env JETSTREAM_TOPIC=my_logs jetstream_debug:latest
   ```

4. **Inside the Container, Start GDB**:

   ```bash
   gdb --args ./jetstream elasticsearch 127.0.0.1:9200
   ```

---

## Development

### Run Analysis

To run a data job and output samples:

```bash
mkdir analysis
make -j"$(nproc)" && time ./jetstream data-job-1 /archive/data > analysis/samples.txt
```

### Run API Locally

To run the API server locally:

```bash
make -j"$(nproc)" && ./jetstream api-server
```

---

## License

Jetstream is released under the [MIT License](LICENSE).

---

If you encounter any issues or have suggestions for improvement, please feel free to reach out through the project's [GitHub repository](https://github.com/homer6/jetstream/issues).

```

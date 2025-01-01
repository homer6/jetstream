# Jetstream Ingest Command: README

This document explains how to run Jetstream from the command line specifically for **ingest** operations (i.e., writing from a Kafka topic to an Ingest HTTP endpoint). It also documents the **environment variables** that can be used to override command-line options.

---

## Overview

Jetstream is a tool that reads from (or writes to) a Kafka topic and can route or transform data in various ways. One of the supported "writer" commands is `ingest`, which writes messages to an HTTP-based ingestion endpoint. 

Running this command involves setting up Kafka connection details (brokers, consumer group, etc.), specifying an HTTP ingestion endpoint, and configuring other optional parameters.

You can pass arguments via:
1. Command-line flags (e.g., `--topic my_topic`)
2. Environment variables (e.g., `JETSTREAM_TOPIC=my_topic`)
3. Relying on built-in defaults (e.g., default brokers: `localhost:9092`)

You can always inspect these configurations by running Jetstream in **dry-run** mode (append `--dry-run`), which will print out the loaded settings and exit without executing the ingestion process.

---

## Usage

```bash
jetstream ingest [OPTION]... [TARGET_INGEST]
```

Writes from a Kafka topic to an HTTP-based ingestion endpoint.

### Required vs. Optional

| Flag / Env Var                     | Description                                                                                               | Default (if any)                                   |
|------------------------------------|-----------------------------------------------------------------------------------------------------------|-----------------------------------------------------|
| `-b, --brokers [BROKERS]`         | A comma-separated list of Kafka brokers.<br>**Env Var**: `JETSTREAM_BROKERS`                               | `localhost:9092`                                    |
| `-c, --consumer-group [GROUP]`     | The Kafka consumer group shared among consumers.<br>**Env Var**: `JETSTREAM_CONSUMER_GROUP`               | `default_consumer_group`                            |
| `-t, --topic [TOPIC]`             | The Kafka topic to read from.<br>**Env Var**: `JETSTREAM_TOPIC`                                            | `jetstream_logs`                                    |
| `-p, --product-code [CODE]`        | A code identifying your organization or product.<br>**Env Var**: `JETSTREAM_PRODUCT_CODE`                 | `prd000`                                            |
| `-h, --hostname [HOSTNAME]`        | The name of this host that will appear in log entries.<br>**Env Var**: `JETSTREAM_HOSTNAME` or `HOSTNAME` | system hostname (e.g., `myhost`)                    |
| `-dh, --destination-hostname [H]`  | The destination ingest hostname (e.g., `localhost:9200`).<br>**Env Var**: `JETSTREAM_DESTINATION_HOSTNAME`| `localhost:9200`                                    |
| `-du, --destination-username [U]`  | The destination username (if required).<br>**Env Var**: `JETSTREAM_DESTINATION_USERNAME`                  | `username`                                          |
| `-dp, --destination-password [P]`  | The destination password (if required).<br>**Env Var**: `JETSTREAM_DESTINATION_PASSWORD`                  | `password`                                          |
| `-di, --destination-index [IDX]`   | The endpoint path or route for ingestion (analogous to `index` in Elasticsearch usage).<br>**Env Var**: `JETSTREAM_DESTINATION_INDEX`  | `my_logs`                 |
| `-ds, --destination-secure [S]`    | Whether the connection is secure (`true` or `false`).<br>**Env Var**: `JETSTREAM_DESTINATION_SECURE`      | `false`                                             |
| `-dau, --destination-auth-url [U]` | The full URL of the auth server to obtain JWT tokens (if required).<br>**Env Var**: `JETSTREAM_DESTINATION_AUTH_URL`                  | (empty)                                             |
| `--token`                          | A token if required by the destination endpoint (e.g., for Loggly).<br>**Env Var**: `JETSTREAM_DESTINATION_TOKEN`                       | `TOKEN`                                             |
| `--dry-run`                        | Print out configuration and exit without connecting.                                                      | (off by default)                                    |

**Additional Environment Variables** (not covered by the short options, but still recognized in the code):

| Env Var                               | Purpose                                                                                                      | Default Value                                                        |
|---------------------------------------|--------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------|
| `JETSTREAM_TASK`                      | An arbitrary "task" name if needed (used internally by Jetstream).                                          | `no_task`                                                            |
| `JETSTREAM_BUCKET`                    | S3 bucket name if used in S3 writer mode (not typically used for `ingest`).                                 | `BUCKET`                                                             |
| `JETSTREAM_ACCESS_KEY_ID`            | S3 access key ID if used in S3 writer mode (not typically used for `ingest`).                               | (empty)                                                              |
| `JETSTREAM_SECRET_ACCESS_KEY`         | S3 secret access key if used in S3 writer mode (not typically used for `ingest`).                           | (empty)                                                              |
| `JETSTREAM_DESTINATION_BROKERS`       | Additional Kafka brokers if used in a specialized scenario (not typically used for `ingest`).               | (empty)                                                              |
| `JETSTREAM_DESTINATION_TOPIC`         | Additional Kafka topic if used in a specialized scenario (not typically used for `ingest`).                 | (empty)                                                              |
| `JETSTREAM_PROM_HOSTNAME`            | Hostname for the Prometheus PushGateway if used for metrics.                                                | `localhost:9200`                                                     |
| `JETSTREAM_PROM_SECURE`              | Whether the Prometheus PushGateway connection is secure (`true` or `false`).                                | `false`                                                              |
| `JETSTREAM_POSTGRES_URL`             | Postgres connection string if used by certain custom subcommands or tasks.                                  | `postgresql://username@localhost/dbname?connect_timeout=10&application_name=myapp&ssl=true` |
| `JETSTREAM_HANDLER_NAME`             | A workflow or handler name, sometimes used to label or track the pipeline.                                  | `na`                                                                 |

> **Important**: Many of these environment variables are used by Jetstream’s other commands (e.g., `elasticsearch`, `loggly`, `s3`) but are listed here for completeness. The `ingest` command typically uses those related to Kafka input and HTTP output only.

---

## Basic Examples

### 1. Minimum viable command
```bash
jetstream ingest
```
- Reads from the default Kafka topic (`jetstream_logs`) at the default brokers (`localhost:9092`)  
- Writes to the default destination (`localhost:9200/my_logs`)  
- Non-secure connection by default (if you need HTTPS, specify `-ds true`).

### 2. Specify Kafka brokers and topic
```bash
jetstream ingest --brokers mykafka1:9092,mykafka2:9092 --topic my_topic
```
- Reads from `my_topic` across Kafka brokers at `mykafka1:9092` and `mykafka2:9092`  
- Uses other defaults for ingestion.

### 3. Use environment variables only
```bash
export JETSTREAM_BROKERS="mykafka1:9092,mykafka2:9092"
export JETSTREAM_TOPIC="another_topic"
export JETSTREAM_DESTINATION_HOSTNAME="ingest.example.org:443"
export JETSTREAM_DESTINATION_INDEX="endpoint"
export JETSTREAM_DESTINATION_SECURE="true"
export JETSTREAM_CONSUMER_GROUP="my_custom_consumer_group"

jetstream ingest
```
- Reads from `another_topic` at the configured brokers  
- Writes to the endpoint: `https://ingest.example.org/endpoint`  
- Consumer group is `my_custom_consumer_group`.

### 4. Dry-run mode
```bash
jetstream ingest --topic my_topic --destination-hostname ingest.example.org --dry-run
```
- Prints out all loaded configurations (from CLI + environment + defaults)  
- Exits without actually running the ingestion loop.

---

## Detailed Options

Below are the primary flags you might use for `jetstream ingest`:

| Option                               | Description                                                                                                      |
|--------------------------------------|------------------------------------------------------------------------------------------------------------------|
| `-b, --brokers [BROKERS]`           | A comma-separated list of Kafka brokers (e.g., `broker1:9092,broker2:9092`).                                    |
| `-c, --consumer-group [GROUP]`       | The Kafka consumer group. Multiple instances of Jetstream can share the same group for parallel processing.     |
| `-t, --topic [TOPIC]`               | The Kafka topic to read from.                                                                                   |
| `-p, --product-code [CODE]`          | A custom product or organizational code (used for internal labeling).                                           |
| `-h, --hostname [HOSTNAME]`          | Hostname used in logging or metadata. Defaults to system hostname or `JETSTREAM_HOSTNAME`.                       |
| `-dh, --destination-hostname [HOST]` | Host/port (or domain) for the target ingest endpoint (e.g., `my.api.com:443`).                                  |
| `-du, --destination-username [USER]` | The username for authentication if the ingest endpoint requires BasicAuth.                                       |
| `-dp, --destination-password [PASS]` | The password for authentication if the ingest endpoint requires BasicAuth.                                       |
| `-di, --destination-index [ENDPOINT]`| The path or route on the ingestion server (like an index in Elasticsearch, but here it’s just the route).        |
| `-ds, --destination-secure [BOOL]`   | `true` or `false`; determines whether to use HTTPS (`true`) or HTTP (`false`).                                  |
| `-dau, --destination-auth-url [URL]` | If the ingest endpoint requires a token-based auth, set the full URL to fetch tokens.                            |
| `--token [TOKEN]`                    | If the ingest endpoint or Loggly requires an auth token, pass it here.                                           |
| `--dry-run`                          | Print configurations and exit immediately (used for debugging).                                                 |

---

## Environment Variables Reference

Every command-line option has a corresponding environment variable. If you **omit a command-line option**, Jetstream will look for the corresponding environment variable. If neither is set, Jetstream falls back to the default values below.

| **Environment Variable**            | **Default Value**                                                     | **Description**                                                                                                 |
|------------------------------------|-----------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| **`JETSTREAM_BROKERS`**            | `localhost:9092`                                                      | Default Kafka brokers.                                                                                          |
| **`JETSTREAM_CONSUMER_GROUP`**     | `default_consumer_group`                                              | Default Kafka consumer group.                                                                                   |
| **`JETSTREAM_TOPIC`**              | `jetstream_logs`                                                      | Default Kafka topic from which to read.                                                                         |
| **`JETSTREAM_TASK`**               | `no_task`                                                             | Internal task label, if needed.                                                                                 |
| **`JETSTREAM_PRODUCT_CODE`**       | `prd000`                                                              | A code identifying your product or organization.                                                                |
| **`JETSTREAM_HOSTNAME`**           | system hostname                                                       | Used if `HOSTNAME` is not set.                                                                                  |
| **`HOSTNAME`**                     | system hostname                                                       | System-provided environment variable used if set.                                                               |
| **`JETSTREAM_DESTINATION_HOSTNAME`** | `localhost:9200`                                                     | Destination host for ingestion.                                                                                 |
| **`JETSTREAM_DESTINATION_USERNAME`** | `username`                                                           | Username for ingestion (if needed).                                                                             |
| **`JETSTREAM_DESTINATION_PASSWORD`** | `password`                                                           | Password for ingestion (if needed).                                                                             |
| **`JETSTREAM_DESTINATION_INDEX`**  | `my_logs`                                                             | Endpoint or “index” route for ingestion.                                                                        |
| **`JETSTREAM_DESTINATION_SECURE`** | `false`                                                               | Whether to use HTTPS (`true`) or HTTP (`false`).                                                                |
| **`JETSTREAM_DESTINATION_TOKEN`**  | `TOKEN`                                                               | An auth token for ingestion, if needed.                                                                         |
| **`JETSTREAM_DESTINATION_AUTH_URL`** | (empty)                                                              | A URL to retrieve tokens from an auth server, if required.                                                      |
| **`JETSTREAM_BUCKET`**             | `BUCKET`                                                              | For the S3 writer (not generally used for `ingest`).                                                            |
| **`JETSTREAM_ACCESS_KEY_ID`**      | (empty)                                                               | For the S3 writer’s access key ID (not generally used for `ingest`).                                            |
| **`JETSTREAM_SECRET_ACCESS_KEY`**  | (empty)                                                               | For the S3 writer’s secret key (not generally used for `ingest`).                                              |
| **`JETSTREAM_DESTINATION_BROKERS`**| (empty)                                                               | Additional Kafka brokers for specialized usage.                                                                 |
| **`JETSTREAM_DESTINATION_TOPIC`**  | (empty)                                                               | Additional Kafka topic for specialized usage.                                                                   |
| **`JETSTREAM_PROM_HOSTNAME`**      | `localhost:9200`                                                      | Hostname for Prometheus PushGateway if used.                                                                    |
| **`JETSTREAM_PROM_SECURE`**        | `false`                                                               | Whether the Prometheus PushGateway connection is secure (`true`/`false`).                                       |
| **`JETSTREAM_POSTGRES_URL`**       | `postgresql://username@localhost/dbname?connect_timeout=10&application_name=myapp&ssl=true` | If used by certain subcommands for Postgres-based features.                                     |
| **`JETSTREAM_HANDLER_NAME`**       | `na`                                                                  | If a custom handler/workflow name is needed.                                                                    |

---

## Example: Full Command with Flags

```bash
jetstream ingest \
  --brokers my-cluster-kafka-1:9092,my-cluster-kafka-2:9092 \
  --consumer-group ingest_cg \
  --topic my_ingest_topic \
  --product-code prd123 \
  --hostname myhost.prod.local \
  --destination-hostname ingest.mydomain.com:443 \
  --destination-username admin \
  --destination-password s3cr3t \
  --destination-index /api/v1/ingest \
  --destination-secure true \
  --destination-auth-url https://auth.mydomain.com/api/user/login \
  --token LONG_TOKEN_HERE
```

This will:
1. Connect to Kafka brokers `my-cluster-kafka-1:9092` and `my-cluster-kafka-2:9092`.
2. Use the consumer group `ingest_cg`.
3. Read from the topic `my_ingest_topic`.
4. Label logs with a product code `prd123`.
5. Report the local host as `myhost.prod.local`.
6. Send ingestion requests to `https://ingest.mydomain.com/api/v1/ingest`.
7. Use `admin` / `s3cr3t` basic authentication if required by the server.
8. Use an additional auth token from the `--token` argument or environment variable.

---

## Checking Your Configuration (Dry Run)

If you are unsure whether your CLI flags and environment variables are set correctly, you can do a **dry run**:

```bash
jetstream ingest --dry-run
```

This prints out the effective configuration (including overrides) and then exits without reading from Kafka or sending data to the destination.

---

## Help and Version

- **Help**:  
  ```bash
  jetstream --help
  ```  
  or  
  ```bash
  jetstream ingest --help
  ```

- **Version**:  
  ```bash
  jetstream --version
  ```
  Prints the Jetstream version.

---

## Further Reading

- Visit the [Jetstream GitHub repository](https://github.com/homer6/jetstream) for source code, issue tracking, and additional documentation.
- Other writer commands include `elasticsearch`, `loggly`, and `s3`.  
- Reader commands include `http`, and processor commands include `filter`.

---

**Enjoy ingesting your data with Jetstream! If you encounter any issues, please open a ticket on the GitHub repo or consult the Jetstream community for support.**
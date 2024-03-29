# jetstream

Jetstream is a general application framework in modern c++.

## Features

1. Act as a persistent kafka producer/consumer
1. HTTP -> Kafka
1. Kafka -> HTTP (able to batch)
1. Sink to ElasticSearch
1. Sink to TypeSense
1. Sink to Loggly/Logz.io
1. HTTP API
1. HTTP Web Serving
1. HTTP Client
1. Prometheus Exporter for each of the above
1. Parallelized XML/JSON/CSV parser (with thread pool)
1. Create data augmentation
1. Postgres Client
1. Typesense Client
1. HTTP endpoint to federate search and relation DB queries into a single HTTP call (GraphQL-like)
1. Event/message router (splits kafka events from a single topic to multiple HTTP endpoints, batched)


Jetstream optionally works with [Logport](https://github.com/homer6/logport) watches log files and sends changes to kafka (one line per message). Logport enables your applications to easily ship logs to kafka (as they are written to and logrotated).

See the [Getting Started Guides](https://github.com/homer6/jetstream/tree/master/docs) to jump right in.


## Roadmap

1. Websocket support
1. RocksDB support for local storage
1. S3 sink
1. S3 reader/writer
1. HTML Templates with CrowCPP integration
1. Web Crawler
1. LLM Client (OpenAI)


## Performance

1. Each consumer instance processes around 10k messages per second (per partition)
1. 50 MB runtime memory
1. Low CPU usage (often below 3%, depending on workload)


## Installing
```
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build -t jetstream:latest .
```

## Building with docker
```
make
docker build -t jetstream:latest -t homer6/jetstream:latest .
docker push homer6/jetstream:latest
```


## Running with docker
```
# loggly
# the topic that jetstream is watching's logs will go to loggly from JETSTREAM_TOPIC(my_logs)
# jestream uses logport to ship it's own logs
# jetstream's logs will go to LOGPORT_TOPIC(my_logs_logger)
# jetstream's logs cannot go to the topic it is consumer (ie. the two values above cannot be the same or it'll create a feedback loop) 

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

## Running on prem
```
./build/jetstream elasticsearch -t my_logs 192.168.1.91:9200
```



## Building

```
apt install -y g++ cmake
```

### Install librdkafka
```
sudo apt install -y libssl-dev libboost-all-dev libsasl2-dev liblz4-dev libzstd-dev
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
sudo ./configure --install-deps
make -j$(nproc)
sudo make install
sudo ldconfig
cd ..
```

### Install cppkafka
```
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka
cmake .
make -j$(nproc)
sudo make install
sudo ldconfig
cd ..
```

### Install libpqxx
```
sudo apt install -y libssl-dev libpq-dev
git clone --recursive https://github.com/jtv/libpqxx.git
cd libpqxx
rm -rf build
mkdir build
cd build
cmake -DPostgreSQL_TYPE_INCLUDE_DIR:STRING=/usr/include/postgresql ..
make -j$(nproc)
sudo make install
sudo ldconfig
cd ../..
```


### make jetstream
```
git clone https://github.com/homer6/jetstream.git
cd jetstream
cmake .
make -j$(nproc)
cd ..
```

### make jetstream docker image (optional)
```
docker build .
```


## Installing Elasticsearch (ubuntu)
```
wget -qO - https://artifacts.elastic.co/GPG-KEY-elasticsearch | sudo apt-key add -
sudo apt-get install apt-transport-https
echo "deb https://artifacts.elastic.co/packages/7.x/apt stable main" | sudo tee -a /etc/apt/sources.list.d/elastic-7.x.list
sudo apt-get update && sudo apt-get install elasticsearch
sudo systemctl enable elasticsearch
sudo systemctl start elasticsearch
```

## Debug
```
# change the cmd/entrypoint to /bin/bash; change image to ubuntu; add gdb (uncomment) to Dockerfile
docker run -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --env JETSTREAM_TOPIC=my_logs jetstream:latest
gdb --args jetstream elasticsearch 127.0.0.1:9200
```



## Run Analysis
```
mkdir analysis
make -j32 && time ./build/jetstream data-job-1 /archive/data > analysis/samples.txt
```

## Run API Locally
```
make -j32 && ./build/jetstream api-server
```

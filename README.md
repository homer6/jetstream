# jetstream
Kafka swiss army knife.

Logport empowers application developers and system administrators with modern observability. This is a turn-key solution for stable, performant, and scalable introspection into what your applications are doing, right now.

[Logport](https://github.com/homer6/logport) watches log files and sends changes to kafka (one line per message). Logport enables your applications to easily produce observability types (obtypes): Metrics, application Events, Telemetry, Traces, and Logs (METTL). Once in Kafka, [Jetstream](https://github.com/homer6/jetstream) can ship your obtypes to compatible "heads" (indices or dashboards) such as Elasticsearch, Logz.io, or Loggly.

See the [Getting Started Guides](https://github.com/homer6/jetstream/tree/master/docs) to jump right in.

## Architecture Overview

![Architecture](https://raw.githubusercontent.com/homer6/logport/master/docs/resources/logport_architecture.jpg)


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

### Install librdkafka
apt install -y libssl-dev libboost-all-dev libsasl2-dev liblz4-dev libzstd-dev
git clone https://github.com/edenhill/librdkafka.git
cd librdkafka
git checkout e241df750a4785f66947df29999d9a2ac1313042
./configure --install-deps
make
make install
ldconfig
cd ..


### Update cmake to a later version for cppkafka (if cmake is old)
sudo apt remove --purge --auto-remove cmake
export version=3.16
export build=2
mkdir ~/temp
cd ~/temp
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
cd cmake-$version.$build/
./bootstrap
make -j$(nproc)
sudo make install
hash -r


### install cppkafka
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka
git checkout 7d097df34dd678c4dfdc1ad07027af13f5635863
cmake .
make -j$(nproc)
make install
ldconfig
cd ..


### make jetstream
git clone https://github.com/homer6/jetstream.git
cd jetstream
cmake .
make -j$(nproc)
cd ..

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
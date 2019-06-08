# jetstream
Kafka swiss army knife.

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
# logz.io
# the topic that jetstream is watching's logs will go to logz.io from JETSTREAM_TOPIC(my_logs)
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
    --env JETSTREAM_LOGZIO_TOKEN=my_logz_token \
    \
    homer6/jetstream:latest logzio
```

## Running on prem
```
./build/jetstream elasticsearch -t my_logs 192.168.1.91:9200
```



## Building

```
apt install librdkafka-dev libssl-dev libboost-all-dev
git clone https://github.com/mfontanini/cppkafka.git
cd cppkafka
git checkout v0.3.1
cmake .
make
make install
ldconfig
cd ..

git clone https://github.com/homer6/jetstream.git
cd jetstream
cmake .
make
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
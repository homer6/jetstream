# jetstream
Kafka swiss army knife.

## Installing
```
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build -t jetstream:latest .
```

## Running with docker
```
docker run -d --env JETSTREAM_TOPIC=my_logs jetstream:latest elasticsearch 192.168.1.91:9200
# docker run -it --env JETSTREAM_TOPIC=my_logs jetstream:latest elasticsearch
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
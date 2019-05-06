# jetstream
Kafka swiss army knife.

## Installing
```
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build --no-cache -t jetstream:latest .
```

## Running with docker
```
docker run -d --env JETSTREAM_TOPIC=my_logs jetstream:latest elasticsearch 127.0.0.1:9200
```


## Running on prem
```
./build/jetstream elasticsearch -t my_logs 127.0.0.1:9200
```




## Building

```
apt install librdkafka-dev libboost-all-dev
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
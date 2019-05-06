# jetstream
Kafka swiss army knife.

## Installing
```
git clone https://github.com/homer6/jetstream.git
cd jetstream
docker build .

docker run c5e18b890d26
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
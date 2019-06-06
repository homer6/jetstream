
# Getting Started (cloud.elastic.co)

Great! You've decided that you want to push all of your observability types (obtypes) through kafka. Now what?

[Logport](https://github.com/homer6/logport) enables your applications to easily produce observability types (obtypes): Metrics, application Events, Telemetry, Traces, and Logs (METTL). Once in Kafka, [Jetstream](https://github.com/homer6/jetstream) can ship your obtypes to compatible "heads" (indices or dashboards) such as Elasticsearch, Logz.io, Snowflake, Lightstep, S3, or Prometheus.


## High level steps:

1. Create your cloud.elastic.co account and create an Elasticsearch deployment.
2. Create your kafka topics.
3. Ship your obtypes to kafka with logport.
4. Ship your obtypes to cloud.elastic.co with jetstream.
5. View your obtypes in cloud.elastic.co.

### Step 1: Create an cloud.elastic.co account and create an Elasticsearch deployment

https://cloud.elastic.co/deployments/create


### Step 2: Create your kafka topics

Most applications will have all obtypes in one topic. Though, you may chose to split each obtype into its own topic.

#### Topic design (all obtypes in one topic)

```
# `logger` is the topic for jetstream's and logport's logs

observability.prd4096.prod.application_name
observability.prd4096.prod.application_name.logger
```

#### Topic design (separate topics for each obtype)

```
observability.prd4096.prod.application_name.logs
observability.prd4096.prod.application_name.metrics
observability.prd4096.prod.application_name.events
observability.prd4096.prod.application_name.traces
observability.prd4096.prod.application_name.logger
```

#### Create the topics

```
# Download Kafka
wget https://www-us.apache.org/dist/kafka/2.2.0/kafka_2.12-2.2.0.tgz
tar -xzf kafka_2.12-2.2.0.tgz
mv kafka_2.12-2.2.0 /usr/local/kafka
alias kt='/usr/local/kafka/bin/kafka-topics.sh'

# Create topic for your application's logs (1 day retention or 50GB, whichever comes first)
kt --create --bootstrap-server 192.168.1.91:9092 \
            --replication-factor 3 \
            --partitions 1 \
            --config retention.ms=86400000 \
            --config retention.bytes=50000000000 \
            --topic observability.prd4096.prod.application_name

# Create topic for jetstream's and logport's logs (1 day retention or 50GB, whichever comes first)
kt --create --bootstrap-server 192.168.1.91:9092 \
            --replication-factor 3 \
            --partitions 1 \
            --config retention.ms=86400000 \
            --config retention.bytes=50000000000 \
            --topic observability.prd4096.prod.application_name.logger

kt --create --bootstrap-server 192.168.1.91:9092 \
            --replication-factor 3 \
            --partitions 1 \
            --config retention.ms=86400000 \
            --config retention.bytes=50000000000 \
            --topic observability.prd4096.prod.application_name.logger2
````



### Step 3: Ship your obtypes to kafka

In order to get your obtype into kafka, you can do it one of three ways:

#### Step 3a: Write directly to kafka from your application.

In this method, you'd use a kafka client library to write directly to a topic from your application.

#### Step 3b: Use logport to read from an application's log file (OnPrem/VM)

```
wget -O librdkafka.so.1 https://github.com/homer6/logport/blob/master/build/librdkafka.so.1?raw=true
wget -O logport https://github.com/homer6/logport/blob/master/build/logport?raw=true
chmod ugo+x logport
sudo ./logport install
rm librdkafka.so.1
rm logport
logport enable
logport set default.brokers 192.168.1.91,192.168.1.92,192.168.1.93
logport set default.topic observability.prd4096.prod.application_name
logport set default.product_code prd4096
logport set rdkafka.producer.queue.buffering.max.messages 1000

# [THIS VALUE DEFAULTS TO SYSTEM HOSTNAME -- ONLY SPECIFY TO OVERRIDE] 
# logport set default.hostname my.sample.hostname

logport watch /var/log/my_application.log /var/log/syslog
logport watch -t observability.prd4096.prod.application_name.logger /usr/local/logport/*.log /var/log/syslog
logport watches
logport start
ps aux | grep logport
logport watches
```

#### Step 3c: Bake logport into your docker image and have it read from your application's stdout and stderr (Docker/Kubernetes)

Add logport to your Dockerfile (see this Dockerfile for details) with the 'adopt' subcommand.

`logport adopt` will fork/exec your application, capturing all stdout/stderr and sending it to kafka. It also
writes your application's stdout/stderr to its own stdout/stderr, so you can still use `docker logs` if you
need to.


### Step 4: Ship your obtypes to Elasticsearch with jetstream

#### Step 4a: Running jetstream within docker
```
# cloud.elastic.co
# You'll have two indices in Elasticsearch: one for the application logs (observability.prd4096.prod.application_name), and one for the logging infrastructure's own logs (observability.prd4096.prod.application_name.logger)
# jetstream will ship both
# jetstream's logs cannot go to the topic it is consuming (ie. the two values above cannot be the same or it'll create a feedback loop)
# Topic( observability.prd4096.prod.application_name.logger2 ) will not be shipped, but can be viewed in kafka.

# no scheme (https) on endpoint
export ELASTICSEARCH_ENDPOINT=zzzzzzzzzz.us-west-2.aws.found.io:9243
export ELASTICSEARCH_USERNAME=elastic
export ELASTICSEARCH_PASSWORD=mypassword


docker run -d \
    --restart unless-stopped \
    --network host \
    \
    --env LOGPORT_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env LOGPORT_TOPIC=observability.prd4096.prod.application_name.logger \
    --env LOGPORT_PRODUCT_CODE=prd4096 \
    --env LOGPORT_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env JETSTREAM_CONSUMER_GROUP=prd4096_mylogs \
    --env JETSTREAM_TOPIC=observability.prd4096.prod.application_name \
    --env JETSTREAM_PRODUCT_CODE=prd4096 \
    --env JETSTREAM_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_DESTINATION_HOSTNAME=$ELASTICSEARCH_ENDPOINT \
    --env JETSTREAM_DESTINATION_USERNAME=$ELASTICSEARCH_USERNAME \
    --env JETSTREAM_DESTINATION_PASSWORD=$ELASTICSEARCH_PASSWORD \
    --env JETSTREAM_DESTINATION_INDEX=mylogs \
    --env JETSTREAM_DESTINATION_SECURE=true \
    \
    homer6/jetstream:latest elasticsearch $ELASTICSEARCH_ENDPOINT


docker run -d \
    --restart unless-stopped \
    --network host \
    \
    --env LOGPORT_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env LOGPORT_TOPIC=observability.prd4096.prod.application_name.logger2 \
    --env LOGPORT_PRODUCT_CODE=prd4096 \
    --env LOGPORT_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
    --env JETSTREAM_CONSUMER_GROUP=prd4096_mylogs \
    --env JETSTREAM_TOPIC=observability.prd4096.prod.application_name.logger \
    --env JETSTREAM_PRODUCT_CODE=prd4096 \
    --env JETSTREAM_HOSTNAME=my.hostname.com \
    \
    --env JETSTREAM_DESTINATION_HOSTNAME=$ELASTICSEARCH_ENDPOINT \
    --env JETSTREAM_DESTINATION_USERNAME=$ELASTICSEARCH_USERNAME \
    --env JETSTREAM_DESTINATION_PASSWORD=$ELASTICSEARCH_PASSWORD \
    --env JETSTREAM_DESTINATION_INDEX=mylogs \
    --env JETSTREAM_DESTINATION_SECURE=true \
    \
    homer6/jetstream:latest elasticsearch


```


#### Step 4b: Running jetstream within kubernetes

```
LOGPORT_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
LOGPORT_TOPIC=my_logs_logger \
LOGPORT_PRODUCT_CODE=prd4096 \
LOGPORT_HOSTNAME=my.hostname.com \
JETSTREAM_BROKERS=192.168.1.91,192.168.1.92,192.168.1.93 \
JETSTREAM_CONSUMER_GROUP=prd4096_mylogs \
JETSTREAM_TOPIC=my_logs \
JETSTREAM_PRODUCT_CODE=prd4096 \
JETSTREAM_HOSTNAME=my.hostname.com \
JETSTREAM_DESTINATION_HOSTNAME=$ELASTICSEARCH_ENDPOINT \
JETSTREAM_DESTINATION_USERNAME=$ELASTICSEARCH_USERNAME \
JETSTREAM_DESTINATION_PASSWORD=$ELASTICSEARCH_PASSWORD \
JETSTREAM_DESTINATION_INDEX=mylogs \
JETSTREAM_DESTINATION_SECURE=true \
./build/jetstream kube elasticsearch | kubectl apply -f -
```


#### Step 5: View your obtypes in cloud.elastic.co

ES Endpoint: https://zzzzz.us-west-2.aws.found.io:9243

Kibana Endpoint: https://zzzzzz.us-west-2.aws.found.io:9243

Create (Kibana) Index Pattern: https://zzzzzz.us-west-2.aws.found.io:9243/app/kibana#/management/kibana/index_patterns


# Getting Started (logz.io)

Great! You've decided that you want to push all of your observability types (obtypes) through kafka. Now what?


## High level steps:

1. Create your logz.io account. Get the account token.
2. Create your kafka topics.
3. Ship your obtypes to kafka with logport.
4. Ship your obtypes to logz.io with jetstream.
5. View your obtypes in logz.io.

Step 1 and 5 are straight forward enough. You can get the account token at https://app.logz.io/#/dashboard/settings/general  ( 'gear icon' > 'Settings' > 'General' )

### Step 2: Create your kafka topics

Most applications will have all obtypes in one topic. Though, you may chose to split each obtype into its own topic.

#### Topic design (all obtypes in one topic)

```
# `logger` is the topic for jetstream's and logport's logs

observability.prd4096.application_name.logs
observability.prd4096.application_name.logger
```

#### Topic design (separate topics for each obtype)

```
observability.prd4096.application_name.logs
observability.prd4096.application_name.metrics
observability.prd4096.application_name.events
observability.prd4096.application_name.traces
observability.prd4096.application_name.logger
```

#### Create the topics

```
# Download Kafka
wget https://www-us.apache.org/dist/kafka/2.2.0/kafka_2.12-2.2.0.tgz
tar -xzf kafka_2.12-2.2.0.tgz
mv kafka_2.12-2.2.0 /usr/local/kafka
alias kt='/usr/local/kafka/bin/kafka-topics.sh'

# Create topic for your application's logs (1 day retention or 50GB, whichever comes first)
kt --create --bootstrap-server 192.168.1.91:9092 --replication-factor 3 --partitions 1 --config retention.ms=86400000 --config retention.bytes=50000000000 --topic observability.prd4096.application_name.logs

# Create topic for jetstream's and logport's logs (1 day retention or 50GB, whichever comes first)
kt --create --bootstrap-server 192.168.1.91:9092 --replication-factor 3 --partitions 1 --config retention.ms=86400000 --config retention.bytes=50000000000 --topic observability.prd4096.application_name.logger
````



### Step 3: Ship your obtypes to kafka

In order to get your obtype into kafka, you can do it one of three ways:

a. Write directly to kafka from your application.
b. Use logport to read from an application's log file.
c. Bake logport into your docker image and have it read from your application's stdout and stderr.




### Step 4: Ship your obtypes to logz.io with jetstream

#### Step 4a: Running jetstream within docker
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


#### Step 4b: Running jetstream within kubernetes

Use the helm chart.
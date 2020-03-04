# FROM ubuntu
FROM busybox:1.30.1-glibc

# busybox missing items
RUN mkdir -p /usr/local/bin
RUN mkdir -p /etc/init.d
RUN mkdir -p /usr/local/lib/logport
ADD build/libdl.so.2 /lib/x86_64-linux-gnu/libdl.so.2
ADD build/libstdc++.so.6 /lib/x86_64-linux-gnu/libstdc++.so.6
ADD build/libgcc_s.so.1 /lib/x86_64-linux-gnu/libgcc_s.so.1
ADD build/librt.so.1 /lib/x86_64-linux-gnu/librt.so.1
ADD build/libssl.so.1.0.0 /usr/local/lib/logport/libssl.so.1.0.0
ADD build/libcrypto.so.1.0.0 /usr/local/lib/logport/libcrypto.so.1.0.0
ADD build/libz.so.1 /usr/local/lib/logport/libz.so.1
ADD build/liblz4.so.1 /usr/local/lib/logport/liblz4.so.1


# RUN apt update && apt install -y libssl-dev && apt clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/{apt,dpkg,cache,log}/


# install logport
ADD build/logport /usr/local/lib/logport/install/logport
ADD build/librdkafka.so.1 /usr/local/lib/logport/install/librdkafka.so.1
ADD build/liblz4.so.1 /usr/local/lib/logport/install/liblz4.so.1
WORKDIR /usr/local/lib/logport/install
RUN /usr/local/lib/logport/install/logport install


ENV LOGPORT_BROKERS 192.168.1.91
ENV LOGPORT_TOPIC my_logs
ENV LOGPORT_PRODUCT_CODE prd4096
ENV LOGPORT_HOSTNAME my.sample.hostname

ENV PATH /usr/local/bin:/usr/local/sbin:/usr/sbin:/usr/bin:/sbin:/bin



# install jetstream
ADD build/jetstream /usr/local/bin/jetstream
RUN chmod ugo+x /usr/local/bin/jetstream
ADD build/libcppkafka.so.0.2 /usr/local/lib/logport/libcppkafka.so.0.2


ENV JETSTREAM_BROKERS 192.168.1.91
ENV JETSTREAM_CONSUMER_GROUP my_consumer_group
ENV JETSTREAM_TOPIC my_source_topic
ENV JETSTREAM_PRODUCT_CODE prd4096
ENV JETSTREAM_HOSTNAME my.sample.hostname

# for debug
# RUN apt update
# RUN apt install -y gdb
# ENTRYPOINT [ "/bin/sh" ]
# ENTRYPOINT [ "gdb", "--args", "jetstream" ]


# for runtime
ENTRYPOINT [ "logport", "adopt", "jetstream" ]
CMD [ "elasticsearch" ]

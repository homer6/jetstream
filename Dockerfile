FROM ubuntu:22.04
#FROM busybox:glibc

# busybox missing items
RUN mkdir -p /usr/local/bin
RUN mkdir -p /etc/init.d
RUN mkdir -p /usr/local/lib/logport
ADD build/libdl.so.2 /lib/x86_64-linux-gnu/libdl.so.2
ADD build/libstdc++.so.6 /lib/x86_64-linux-gnu/libstdc++.so.6
ADD build/libgcc_s.so.1 /lib/x86_64-linux-gnu/libgcc_s.so.1
#ADD build/librt.so.1 /lib/x86_64-linux-gnu/librt.so.1
ADD build/libssl.so.3 /lib/x86_64-linux-gnu/libssl.so.3
ADD build/libcrypto.so.3 /lib/x86_64-linux-gnu/libcrypto.so.3
ADD build/libk5crypto.so.3 /lib/x86_64-linux-gnu/libk5crypto.so.3
ADD build/libz.so.1 /lib/x86_64-linux-gnu/libz.so.1
ADD build/liblz4.so.1 /lib/x86_64-linux-gnu/liblz4.so.1
ADD build/libzstd.so.1 /lib/x86_64-linux-gnu/libzstd.so.1
ADD build/libsasl2.so.2 /lib/x86_64-linux-gnu/libsasl2.so.2
ADD build/libm.so.6 /lib/x86_64-linux-gnu/libm.so.6

ADD build/librdkafka.so.1 /usr/local/lib/librdkafka.so.1
ADD build/libcppkafka.so.0.4.0 /usr/local/lib/libcppkafka.so.0.4.0

ADD build/liblz4.so.1 /lib/x86_64-linux-gnu/liblz4.so.1
ADD build/libm.so.6 /lib/x86_64-linux-gnu/libm.so.6
ADD build/libcurl.so.4 /lib/x86_64-linux-gnu/libcurl.so.4
ADD build/libzstd.so.1 /lib/x86_64-linux-gnu/libzstd.so.1
ADD build/libsasl2.so.2 /lib/x86_64-linux-gnu/libsasl2.so.2
ADD build/libssl.so.3 /lib/x86_64-linux-gnu/libssl.so.3
ADD build/libcrypto.so.3 /lib/x86_64-linux-gnu/libcrypto.so.3
ADD build/libz.so.1 /lib/x86_64-linux-gnu/libz.so.1
#ADD build/libc.so.6 /lib/x86_64-linux-gnu/libc.so.6
ADD build/libnghttp2.so.14 /lib/x86_64-linux-gnu/libnghttp2.so.14
ADD build/libidn2.so.0 /lib/x86_64-linux-gnu/libidn2.so.0
ADD build/librtmp.so.1 /lib/x86_64-linux-gnu/librtmp.so.1
ADD build/libssh.so.4 /lib/x86_64-linux-gnu/libssh.so.4
ADD build/libpsl.so.5 /lib/x86_64-linux-gnu/libpsl.so.5
ADD build/libgssapi_krb5.so.2 /lib/x86_64-linux-gnu/libgssapi_krb5.so.2
ADD build/libldap-2.5.so.0 /lib/x86_64-linux-gnu/libldap-2.5.so.0
ADD build/liblber-2.5.so.0 /lib/x86_64-linux-gnu/liblber-2.5.so.0
ADD build/libbrotlidec.so.1 /lib/x86_64-linux-gnu/libbrotlidec.so.1
ADD build/libunistring.so.2 /lib/x86_64-linux-gnu/libunistring.so.2
ADD build/libgnutls.so.30 /lib/x86_64-linux-gnu/libgnutls.so.30
ADD build/libhogweed.so.6 /lib/x86_64-linux-gnu/libhogweed.so.6
ADD build/libnettle.so.8 /lib/x86_64-linux-gnu/libnettle.so.8
ADD build/libgmp.so.10 /lib/x86_64-linux-gnu/libgmp.so.10
ADD build/libkrb5.so.3 /lib/x86_64-linux-gnu/libkrb5.so.3
ADD build/libk5crypto.so.3 /lib/x86_64-linux-gnu/libk5crypto.so.3
ADD build/libcom_err.so.2 /lib/x86_64-linux-gnu/libcom_err.so.2
ADD build/libkrb5support.so.0 /lib/x86_64-linux-gnu/libkrb5support.so.0
ADD build/libbrotlicommon.so.1 /lib/x86_64-linux-gnu/libbrotlicommon.so.1
ADD build/libp11-kit.so.0 /lib/x86_64-linux-gnu/libp11-kit.so.0
ADD build/libtasn1.so.6 /lib/x86_64-linux-gnu/libtasn1.so.6
ADD build/libkeyutils.so.1 /lib/x86_64-linux-gnu/libkeyutils.so.1
#ADD build/libresolv.so.2 /lib/x86_64-linux-gnu/libresolv.so.2
ADD build/libffi.so.8 /lib/x86_64-linux-gnu/libffi.so.8

ADD build/libpq.so.5 /lib/x86_64-linux-gnu/libpq.so.5


# RUN apt update && apt install -y libssl-dev && apt clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/{apt,dpkg,cache,log}/


# install logport
#ADD build/logport /usr/local/lib/logport/install/logport
#ADD build/librdkafka.so.1 /usr/local/lib/logport/install/librdkafka.so.1
#WORKDIR /usr/local/lib/logport/install
#RUN /usr/local/lib/logport/install/logport install


#ENV LOGPORT_BROKERS 192.168.1.91
#ENV LOGPORT_TOPIC my_logs
#ENV LOGPORT_PRODUCT_CODE prd4096
#ENV LOGPORT_HOSTNAME my.sample.hostname

#ENV PATH /usr/local/bin:/usr/local/sbin:/usr/sbin:/usr/bin:/sbin:/bin



# install jetstream
ADD build/jetstream /usr/local/bin/jetstream
RUN chmod ugo+x /usr/local/bin/jetstream
ADD build/librdkafka.so.1 /usr/local/lib/logport/librdkafka.so.1
ADD build/libcppkafka.so.0.4.0 /usr/local/lib/logport/libcppkafka.so.0.4.0

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
# ENTRYPOINT [ "logport", "adopt", "jetstream" ]
# CMD [ "elasticsearch" ]
CMD [ "/bin/sh" ]

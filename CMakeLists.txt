cmake_minimum_required( VERSION "2.6.4" )

project( jetstream CXX C )


include_directories(
    include
    /usr/local/include/librdkafka
    /usr/local/include/cppkafka
)

FIND_LIBRARY(RDKAFKA rdkafka /usr/local/lib NO_DEFAULT_PATH)
FIND_LIBRARY(CPPKAFKA cppkafka /usr/local/lib NO_DEFAULT_PATH)

link_libraries(${RDKAFKA} ${CPPKAFKA})

set( JETSTREAM_SOURCES
    src/Common.cc
    src/Observer.cc
    src/JetStream.cc
    src/JetStreamConfig.cc
    src/ElasticsearchWriterConfig.cc
    src/LogzioWriterConfig.cc
    src/LogglyWriterConfig.cc
    src/KubeLauncherConfig.cc
    src/KafkaProducer.cc
)

set( JETSTREAM_LIBS
    rdkafka
    cppkafka
    pthread
    dl
    ssl
    crypto
    z
)

set( JETSTREAM_COMPILE_OPTIONS
    -Wall
    -Wextra
    -O3
    -std=c++17
    -DCPPHTTPLIB_OPENSSL_SUPPORT
    -DCPPHTTPLIB_ZLIB_SUPPORT
    # -g
)

add_executable( build/jetstream ${JETSTREAM_SOURCES} src/jetstream_main.cc )

target_link_libraries( build/jetstream -Wl,-rpath=/usr/local/lib/logport:. ${JETSTREAM_LIBS} )

set_target_properties( build/jetstream
    PROPERTIES
    COMPILE_OPTIONS  "${JETSTREAM_COMPILE_OPTIONS}"
)


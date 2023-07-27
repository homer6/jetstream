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

    src/config/JetStreamConfig.cc
    src/config/ElasticSearchWriterConfig.cc
    src/config/LogglyWriterConfig.cc
    src/config/KubeWriterConfig.cc
    src/config/KafkaWriterConfig.cc
    src/config/IngestWriterConfig.cc
    src/config/S3WriterConfig.cc

    src/writer/ElasticSearchWriter.cc
    src/writer/LogglyWriter.cc
    src/writer/KubeWriter.cc
    src/writer/KafkaWriter.cc
    src/writer/IngestWriter.cc
    src/writer/S3Writer.cc

    src/task/RekeyTask.cc
    src/task/TaskFactory.cc

    src/ingest/LogSender.cc
    src/ingest/LogSenderAttempt.cc
    src/ingest/LogSplitter.cc
    src/ingest/IngestWriterExporter.cc

    src/parser/GizmoParser.cc

    src/client/S3Client.cc
    src/client/KafkaProducer.cc
    src/client/HttpConnection.cc
    src/client/TypesenseClient.cc
    src/client/PostgresClient.cc

    src/server/HttpServer.cc
    src/server/ApiServer.cc
    src/server/Session.cc

    src/server/model/FieldSet.cc
    src/server/model/Model.cc

    src/server/model/Gizmo.cc

    src/server/collection/GizmoSet.cc
    src/server/collection/ModelSet.cc
    src/server/collection/SearchableModelSet.cc

    src/server/event/EventRouter.cc

    src/server/service/TypesenseService.cc
    src/server/service/PostgresService.cc

    src/server/search/SearchQuery.cc

    src/frontend/view/SearchView.cc

    src/Url.cc
    src/UrlList.cc
    src/stopwatch.cc

    src/observability/prometheus/Metric.cc
    src/observability/prometheus/Registry.cc

    src/data/DataAugmenter.cc
    src/data/GizmoDataAugmenter.cc

    src/pugixml.cpp

)

set( JETSTREAM_LIBS
    rdkafka
    cppkafka
    pthread
    dl
    ssl
    crypto
    z
    pqxx
    pq
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


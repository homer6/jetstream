cmake_minimum_required(VERSION 3.10)

project(jetstream CXX C)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'include' directory
include_directories(
    include
)

# Set compile options
set(JETSTREAM_COMPILE_OPTIONS
    -Wall
    -Wextra
    -O3
    -DCPPHTTPLIB_OPENSSL_SUPPORT
    -DCPPHTTPLIB_ZLIB_SUPPORT
    # -g
)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    message(STATUS "Found OpenSSL: ${OPENSSL_VERSION}")
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(FATAL_ERROR "OpenSSL not found")
endif()

# Find librdkafka
find_path(RDKAFKA_INCLUDE_DIR NAMES rdkafka.h PATHS /usr/local/include /usr/include /opt/homebrew/include)
find_library(RDKAFKA_LIBRARY NAMES rdkafka PATHS /usr/local/lib /usr/lib /opt/homebrew/lib)

if(RDKAFKA_INCLUDE_DIR AND RDKAFKA_LIBRARY)
    message(STATUS "Found librdkafka")
    include_directories(${RDKAFKA_INCLUDE_DIR})
else()
    message(FATAL_ERROR "librdkafka not found")
endif()

# Find cppkafka
find_path(CPPKAFKA_INCLUDE_DIR NAMES cppkafka/cppkafka.h PATHS /usr/local/include /usr/include /opt/homebrew/include)
find_library(CPPKAFKA_LIBRARY NAMES cppkafka PATHS /usr/local/lib /usr/lib /opt/homebrew/lib)

if(CPPKAFKA_INCLUDE_DIR AND CPPKAFKA_LIBRARY)
    message(STATUS "Found cppkafka")
    include_directories(${CPPKAFKA_INCLUDE_DIR})
else()
    message(FATAL_ERROR "cppkafka not found")
endif()

# Find libpqxx
find_path(PQXX_INCLUDE_DIR NAMES pqxx/pqxx PATHS /usr/local/include /usr/include /opt/homebrew/include)
find_library(PQXX_LIBRARY NAMES pqxx PATHS /usr/local/lib /usr/lib /opt/homebrew/lib)

if(PQXX_INCLUDE_DIR AND PQXX_LIBRARY)
    message(STATUS "Found libpqxx")
    include_directories(${PQXX_INCLUDE_DIR})
else()
    message(FATAL_ERROR "libpqxx not found")
endif()

# Additional include directories if necessary
include_directories(
    ${OPENSSL_INCLUDE_DIR}
    ${RDKAFKA_INCLUDE_DIR}
    ${CPPKAFKA_INCLUDE_DIR}
    ${PQXX_INCLUDE_DIR}
)

# Source files
set(JETSTREAM_SOURCES
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

    src/jetstream_main.cc
)

# Libraries
set(JETSTREAM_LIBS
    ${RDKAFKA_LIBRARY}
    ${CPPKAFKA_LIBRARY}
    pthread
    dl
    ${OPENSSL_LIBRARIES}
    z
    ${PQXX_LIBRARY}
)

# Create executable
add_executable(jetstream ${JETSTREAM_SOURCES})

# Link libraries to the executable
target_link_libraries(jetstream PRIVATE ${JETSTREAM_LIBS})

# Set compile options for the target
target_compile_options(jetstream PRIVATE ${JETSTREAM_COMPILE_OPTIONS})

# Set rpath if necessary
set_target_properties(jetstream PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

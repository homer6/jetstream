cmake_minimum_required(VERSION 3.10)

project(jetstream CXX C)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)  # Provides standard installation directories

# Handle CMP0167 policy for FindBoost
# if(POLICY CMP0167)
#   cmake_policy(SET CMP0167 OLD)
# endif()

# Include the 'include' directory
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    include
)

# Set compile options
set(JETSTREAM_COMPILE_OPTIONS
    -Wall
    -Wextra
    -O3
    -DCPPHTTPLIB_OPENSSL_SUPPORT
    -DCPPHTTPLIB_ZLIB_SUPPORT
)

# Find pkg-config
# find_package(PkgConfig REQUIRED)

# # Function to set PKG_CONFIG_PATH for Homebrew packages on macOS
# function(set_pkg_config_path brew_package_name pkgconfig_module_name)
#     if(APPLE)
#         execute_process(
#             COMMAND brew --prefix ${brew_package_name}
#             OUTPUT_VARIABLE PREFIX
#             OUTPUT_STRIP_TRAILING_WHITESPACE
#         )
#         set(ENV{PKG_CONFIG_PATH} "${PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
#     endif()
# endfunction()

# Set PKG_CONFIG_PATH for necessary packages
# set_pkg_config_path(librdkafka rdkafka)
# set_pkg_config_path(libpq libpq)
# set_pkg_config_path(openssl@3 openssl)

# Find Boost libraries
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# if(Boost_FOUND)
#     message(STATUS "Found Boost: ${Boost_VERSION}")
#     include_directories(${Boost_INCLUDE_DIRS})
# else()
#     message(FATAL_ERROR "Boost not found")
# endif()

message( STATUS "BOOST_INCLUDE_DIRS: ${BOOST_INCLUDE_DIRS}" )
message( STATUS "BOOST_LIBRARIES: ${BOOST_LIBRARIES}" )


# Find OpenSSL
# find_package(OpenSSL REQUIRED)
# if(OPENSSL_FOUND)
#     message(STATUS "Found OpenSSL: ${OPENSSL_VERSION}")
#     include_directories(${OPENSSL_INCLUDE_DIR})
# else()
#     message(FATAL_ERROR "OpenSSL not found")
# endif()

message( STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}" )
message( STATUS "OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}" )

# Find librdkafka
# pkg_check_modules(RDKAFKA REQUIRED rdkafka)
# if(RDKAFKA_FOUND)
#     message(STATUS "Found librdkafka")
#     message(STATUS "RDKAFKA_LIBRARIES: ${RDKAFKA_LIBRARIES}")
#     message(STATUS "RDKAFKA_LIBRARY_DIRS: ${RDKAFKA_LIBRARY_DIRS}")
#     include_directories(${RDKAFKA_INCLUDE_DIRS})
#     link_directories(${RDKAFKA_LIBRARY_DIRS})
#     find_library(RDKAFKA_LIB NAMES rdkafka HINTS ${RDKAFKA_LIBRARY_DIRS})
#     if(RDKAFKA_LIB)
#         message(STATUS "Found rdkafka library at: ${RDKAFKA_LIB}")
#         set(RDKAFKA_LIBRARIES ${RDKAFKA_LIB})
#     else()
#         message(FATAL_ERROR "Could not find rdkafka library")
#     endif()
# else()
#     message(FATAL_ERROR "librdkafka not found")
# endif()

message( STATUS "RDKAFKA_INCLUDE_DIRS: ${RDKAFKA_INCLUDE_DIRS}" )
message( STATUS "RDKAFKA_LIBRARIES: ${RDKAFKA_LIBRARIES}" )



# Find cppkafka
# find_path(CPPKAFKA_INCLUDE_DIRS cppkafka/cppkafka.h)
# find_library(CPPKAFKA_LIBRARIES cppkafka)
# if(CPPKAFKA_INCLUDE_DIRS AND CPPKAFKA_LIBRARIES)
#     message(STATUS "Found cppkafka")
#     include_directories(${CPPKAFKA_INCLUDE_DIRS})
# else()
#     message(FATAL_ERROR "cppkafka not found")
# endif()

message( STATUS "CPPKAFKA_INCLUDE_DIRS: ${CPPKAFKA_INCLUDE_DIRS}" )
message( STATUS "CPPKAFKA_LIBRARIES: ${CPPKAFKA_LIBRARIES}" )


# Improved libpqxx detection
# find_package(PkgConfig QUIET)
# if(PKG_CONFIG_FOUND)
#   pkg_check_modules(PQXX REQUIRED libpqxx)
#   if(PQXX_FOUND)
#     message(STATUS "Found libpqxx via pkg-config: ${PQXX_VERSION}")
#     include_directories(${PQXX_INCLUDE_DIRS})
#     link_directories(${PQXX_LIBRARY_DIRS})
#   endif()
# else()
#   # Fallback method
#   find_path(PQXX_INCLUDE_DIRS pqxx/pqxx)
#   find_library(PQXX_LIBRARIES pqxx)
#   if(PQXX_INCLUDE_DIRS AND PQXX_LIBRARIES)
#     message(STATUS "Found libpqxx via find_library")
#     include_directories(${PQXX_INCLUDE_DIRS})
#   else()
#     message(FATAL_ERROR "libpqxx not found")
#   endif()
# endif()

message( STATUS "PQXX_INCLUDE_DIRS: ${PQXX_INCLUDE_DIRS}" )
message( STATUS "PQXX_LIBRARIES: ${PQXX_LIBRARIES}" )


# Find libpq
# pkg_check_modules(LIBPQ REQUIRED libpq)
# if(LIBPQ_FOUND)
#     message(STATUS "Found libpq")
#     message(STATUS "LIBPQ_LIBRARIES: ${LIBPQ_LIBRARIES}")
#     message(STATUS "LIBPQ_LIBRARY_DIRS: ${LIBPQ_LIBRARY_DIRS}")
#     include_directories(${LIBPQ_INCLUDE_DIRS})
#     link_directories(${LIBPQ_LIBRARY_DIRS})
#     find_library(LIBPQ_LIB NAMES pq HINTS ${LIBPQ_LIBRARY_DIRS})
#     if(LIBPQ_LIB)
#         message(STATUS "Found libpq library at: ${LIBPQ_LIB}")
#         set(LIBPQ_LIBRARIES ${LIBPQ_LIB})
#     else()
#         message(FATAL_ERROR "Could not find libpq library")
#     endif()
# else()
#     message(FATAL_ERROR "libpq not found")
# endif()

message(STATUS "Found libpq library at: ${LIBPQ_LIBRARY_DIRS}")


# find AWS SDK
# find_package(AWSSDK REQUIRED COMPONENTS s3)

message( STATUS "AWS_SDK_CPP_INCLUDE_DIRS: ${AWS_SDK_CPP_INCLUDE_DIRS}" )
message( STATUS "AWS_SDK_CPP_LIBRARIES: ${AWS_SDK_CPP_LIBRARIES}" )


# Additional include directories
include_directories(
    ${OPENSSL_INCLUDE_DIR}
    ${RDKAFKA_INCLUDE_DIRS}
    ${CPPKAFKA_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRS}
    ${LIBPQ_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    ${AWS_SDK_CPP_INCLUDE_DIRS}
    ${CPP_HTTPLIB_INCLUDE_DIRS}
)


# Source files
set(JETSTREAM_SOURCES
    src/Common.cc
    src/Observer.cc
    src/JetStream.cc
    src/args_holder.cc
    src/CommandExecutor.cc
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
    src/workflow/WorkflowRunContext.cc
    src/workflow/WorkflowRun.cc
    src/workflow/WorkflowFactory.cc
    src/workflow/WorkflowRunStep.cc
    src/workflow/WorkflowRunStepCommand.cc
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
    ${RDKAFKA_LIBRARIES}
    ${CPPKAFKA_LIBRARIES}
    ${BOOST_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${PQXX_LIBRARIES}
    ${LIBPQ_LIBRARIES}
    ${AWS_SDK_CPP_LIBRARIES}
    ${CPP_HTTPLIB_LIBRARIES}
)



# Handle platform-specific settings
# if(WIN32)
#     # Windows-specific libraries
#     list(APPEND JETSTREAM_LIBS ws2_32)
#     set(PLATFORM_COMPILE_OPTIONS)
# else()
#     # POSIX platforms (Linux, macOS)
#     find_package(Threads REQUIRED)
#     list(APPEND JETSTREAM_LIBS Threads::Threads ${CMAKE_DL_LIBS} z)
#     set(PLATFORM_COMPILE_OPTIONS)
# endif()

# Add debugging output for library paths
message(STATUS "PQXX_LIBRARIES: ${PQXX_LIBRARIES}")
message(STATUS "PQXX_LIBRARY_DIRS: ${PQXX_LIBRARY_DIRS}")

# Create the executable
add_executable(jetstream ${JETSTREAM_SOURCES})

# Link libraries
target_link_libraries(jetstream PRIVATE ${JETSTREAM_LIBS})

# Set compile options
target_compile_options(jetstream PRIVATE ${JETSTREAM_COMPILE_OPTIONS} ${PLATFORM_COMPILE_OPTIONS})

# # Set rpath for macOS
# if(APPLE)
#   set_target_properties(jetstream PROPERTIES
#     INSTALL_RPATH "@executable_path/../lib;@loader_path/../lib"
#     BUILD_WITH_INSTALL_RPATH TRUE
#     MACOSX_RPATH TRUE
#   )
# # Set rpath for other UNIX platforms
# elseif(UNIX AND NOT APPLE)
#   set_target_properties(jetstream PROPERTIES
#     INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
#     BUILD_WITH_INSTALL_RPATH TRUE
#   )
# endif()

# Add an install rule to place the jetstream binary in the standard bin directory
install(TARGETS jetstream RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Add a compile definition to handle libpqxx version differences
# add_compile_definitions(LIBPQXX_VERSION_7_10)

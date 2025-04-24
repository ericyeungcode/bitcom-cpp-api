cmake_minimum_required(VERSION 3.14)
project(bit_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost
find_package(Boost 1.88 REQUIRED COMPONENTS system thread)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Collect source files manually or via glob (with caution)
file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
# Define executable target
add_executable(${PROJECT_NAME}
    main.cpp
    ${SRC_FILES}
)

# Include directories
# add /Users/ericyeung/private/github/cplusplus/include for BoostWsClient.hpp 
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty
)


# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${Boost_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)
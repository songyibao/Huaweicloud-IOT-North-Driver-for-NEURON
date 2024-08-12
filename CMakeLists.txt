cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(plugin-hw-iot)

# 指定C标准
set(CMAKE_C_STANDARD 99)

# 添加源文件
set(SOURCES
        hwiot_plugin.c
        client/client.c
)

# 添加可执行文件
add_library(${PROJECT_NAME} SHARED
        utils/utils.c)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
# 添加所有包含目录
include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/*
        ${CMAKE_CURRENT_SOURCE_DIR}/include/agentlite
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util
        ${CMAKE_CURRENT_SOURCE_DIR}/include/third_party/cjson
        ${CMAKE_CURRENT_SOURCE_DIR}/include/service
        ${CMAKE_CURRENT_SOURCE_DIR}/include/third_party/libboundscheck)
# add the path to the neuron header
include_directories(${PROJECT_NAME} PRIVATE /usr/local/include /usr/local/include/neuron)
# add the path to the neuron library
target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib /usr/local/lib/neuron /opt/huaweicloud-iot-device-sdk-c-1.2.0/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE /usr/local/lib/libboundscheck.so /usr/local/lib/libHWMQTT.so /usr/local/lib/neuron/libneuron-base.so)

#add_executable(test client.c)
#target_link_directories(test PRIVATE /usr/local/lib /usr/local/lib/neuron /opt/huaweicloud-iot-device-sdk-c-1.2.0/lib)
#
#target_link_libraries(test PRIVATE /opt/huaweicloud-iot-device-sdk-c-1.2.0/lib/libHWMQTT.so /usr/local/lib/neuron/libneuron-base.so)
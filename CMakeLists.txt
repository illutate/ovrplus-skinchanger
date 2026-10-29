cmake_minimum_required(VERSION 3.20)
project(ovr-skinchanger LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Minhook/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs
    ${CMAKE_CURRENT_SOURCE_DIR}/Resource/include
)

file(GLOB_RECURSE SRC_CHANGER CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Changer/*.cpp")
file(GLOB_RECURSE SRC_HOOKS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Hooks/*.cpp")
file(GLOB_RECURSE SRC_SDK CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/SDK/*.cpp")
file(GLOB_RECURSE SRC_MINHOOK CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Minhook/src/*.c")

set(SRC_PROTOBUF
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/base_gcmessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/dota_commonmessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/dota_shared_enums.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/dota_usermessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/econ_gcmessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/econ_shared_enums.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/event_gcmessages_common.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/events.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/gcsdk_gcmessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/netmessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/network_connection.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/networkbasetypes.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/source2_steam_stats.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/steammessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/steammessages_steamlearn.steamworkssdk.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/steammessages_unified_base.steamworkssdk.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/usermessages.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Resource/Protobufs/valveextensions.pb.cc"
)

add_library(ovr-skinchanger SHARED
    Main.cpp
    ${SRC_CHANGER}
    ${SRC_HOOKS}
    ${SRC_SDK}
    ${SRC_MINHOOK}
    ${SRC_PROTOBUF}
)

target_link_libraries(ovr-skinchanger PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Resource/libprotobuf.a
    user32
    shell32
    ole32
    advapi32
)

target_link_options(ovr-skinchanger PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
)

set_target_properties(ovr-skinchanger PROPERTIES
    OUTPUT_NAME "ovr-skinchanger"
    PREFIX ""
)

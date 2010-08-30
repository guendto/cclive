

project (quvicpp)

set (quvicpp_SRC
    src/error.cpp
    src/link.cpp
    src/misc.cpp
    src/options.cpp
    src/query.cpp
    src/video.cpp
)

add_library (quvicpp ${quvicpp_SRC})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries (quvicpp ${quvi_LIBRARIES})

link_directories (${quvicpp_BINARY_DIR}/quvicpp)



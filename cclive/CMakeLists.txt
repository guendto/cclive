
set (cclive_SRC
    src/application.cpp
    src/background.cpp
    src/error.cpp
    src/exec.cpp
    src/file.cpp
    src/get.cpp
    src/license.cpp
    src/log.cpp
    src/main.cpp
    src/options.cpp
    src/progressbar.cpp
    src/re.cpp
)

add_executable (cclive ${cclive_SRC})

add_dependencies (cclive quvicpp)

include_directories(
    BEFORE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${quvicpp_SOURCE_DIR}/include
)

target_link_libraries (cclive
    quvicpp # See lib/quvicpp/CMakeLists.txt
    ${curl_LIBRARIES}
    ${pcrecpp_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Install.

install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/cclive DESTINATION bin)



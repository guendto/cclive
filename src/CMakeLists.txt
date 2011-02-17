
set (cclive_SRC
    cclive/application.cpp
    cclive/background.cpp
    cclive/error.cpp
    cclive/exec.cpp
    cclive/file.cpp
    cclive/get.cpp
    cclive/license.cpp
    cclive/log.cpp
    cclive/main.cpp
    cclive/options.cpp
    cclive/progressbar.cpp
    cclive/re.cpp
    cclive/wait.cpp
)

set (quvicpp_SRC
    quvicpp/error.cpp
    quvicpp/link.cpp
    quvicpp/misc.cpp
    quvicpp/options.cpp
    quvicpp/query.cpp
    quvicpp/video.cpp
)

add_executable (cclive ${quvicpp_SRC} ${cclive_SRC})

include_directories(
    BEFORE
    ${CMAKE_BINARY_DIR}
)

target_link_libraries (cclive
    ${quvi_LIBRARIES}
    ${curl_LIBRARIES}
    ${pcrecpp_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Install.

install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/cclive DESTINATION bin)



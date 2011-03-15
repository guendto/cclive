
set (cclive_SRC
    application.cpp
    background.cpp
    error.cpp
    exec.cpp
    file.cpp
    get.cpp
    license.cpp
    log.cpp
    main.cpp
    options.cpp
    progressbar.cpp
    re.cpp
    wait.cpp
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



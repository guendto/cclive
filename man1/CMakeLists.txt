
# Install.

if (NOT WITHOUT_DOC)

    set (man_1   "cclive.1")
    set (man_src "${CMAKE_CURRENT_SOURCE_DIR}/${man_1}")

    if (GZIP_PROG)

        set (man_gz "${CMAKE_CURRENT_BINARY_DIR}/${man_1}.gz")

        execute_process (
            COMMAND
            "${GZIP_PROG}"
            "${man_src}"
            "-c"
            OUTPUT_FILE "${man_gz}"
            RESULT_VARIABLE gzip_rc)

        if (gzip_rc EQUAL 0)
            set (man_inst "${man_gz}")
        endif ()

    else () # no gzip
        set (man_inst "${man_src}")
    endif ()

    install (FILES "${man_inst}" DESTINATION share/man/man1)

endif () # not WITHOUT_DOC



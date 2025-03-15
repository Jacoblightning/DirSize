message("Enabling tests")

add_test(
        NAME checkSuccessfulCompile
        COMMAND $<TARGET_FILE:DirSize> -V
)

if (WIN32)
    add_test(
            NAME checkDSizeWorks
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testDSizeWorks.bat "$<TARGET_FILE:DirSize>"
    )
else ()
    add_test(
            NAME checkDSizeWorks
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testDSizeWorks.sh "$<TARGET_FILE:DirSize>"
    )
endif ()
macro(option_define option define)
    if (${option})
        add_definitions(-D${define})
    endif ()
endmacro()
macro(set_custom_compiler_options)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Use the highest warning level for Visual Studio.
        set(CMAKE_CXX_WARNING_LEVEL 4)
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()

        # Disable C++ exceptions.
        #string(REGEX REPLACE "/EH[^ ]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs-c-")
        #add_definitions(-D_HAS_EXCEPTIONS=0)

        # Disable RTTI.
        string(REGEX REPLACE "/GR[-]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    else()
        # Use -Wall for clang and gcc.
        if(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
        endif(NOT CMAKE_CXX_FLAGS MATCHES "-Wall")

        # Use -Wextra for clang and gcc.
        if(NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
        endif(NOT CMAKE_CXX_FLAGS MATCHES "-Wextra")

        # Disable C++ exceptions.
        string(REGEX REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        if(NOT CMAKE_CXX_FLAGS MATCHES "-fno-exceptions")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
        endif()

        # Disable RTTI.
        string(REGEX REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        if(NOT CMAKE_CXX_FLAGS MATCHES "-fno-rtti")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
        endif()
    endif()
endmacro()

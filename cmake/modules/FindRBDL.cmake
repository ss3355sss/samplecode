#if (NOT DEFINED RBDL_ROOT)
#    message(FATAL_ERROR "RBDL_ROOT is not defined")
#endif ()
#
#find_path(RBDL_INCLUDE_DIR
#            gtest/gtest.h
#          PATHS
#            ${RBDL_ROOT}/include
#          DOC
#            "GOOGLETEST Include directory"
#        )
#
#if(WIN32)
#    message(FATAL_ERROR "GoogleTest is not support WIN32")
#
#elseif(UNIX)
#    find_path(GOOGLETEST_LIBRARY_DIR
#            libgmock.a
#            libgmock_main.a
#            libgtest.a
#            libgtest_main.a
#
#            PATHS
#                ${GOOGLETEST_ROOT}/lib
#                ${GOOGLETEST_ROOT}/lib64
#            DOC
#                "GOOGLETEST Librarires directory"
#            )
#else()
#    message(FATAL_ERROR "unknown platform for GoogleTest")
#endif()
#
#foreach(GOOGLETEST_LIB
#            gmock
#            gmock_main
#            gtest
#            gtest_main
#        )
#    find_library(GOOGLETEST_${GOOGLETEST_LIB}_LIBRARY
#            ${GOOGLETEST_LIB}
#            HINTS
#                "${GOOGLETEST_ROOT}"
#            PATH_SUFFIXES
#                lib/
#                lib64/
#            DOC
#                "GOOGLETEST's ${GOOGLETEST_LIB} library path"
#            )
#
#    if (GOOGLETEST_${GOOGLETEST_LIB}_LIBRARY)
#        list(APPEND GOOGLETEST_LIBRARIES ${GOOGLETEST_${GOOGLETEST_LIB}_LIBRARY})
#    endif()
#
#endforeach(GOOGLETEST_LIB)
#
#
#include(FindPackageHandleStandardArgs)
#find_package_handle_standard_args(GoogleTest
#        REQUIRED_VARS
#            GOOGLETEST_INCLUDE_DIR
#    	    GOOGLETEST_LIBRARY_DIR
#		    GOOGLETEST_LIBRARIES
#)

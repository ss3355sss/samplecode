if (NOT DEFINED ZELOS_ROOT)
    message(FATAL_ERROR "ZELOS_ROOT is not defined")
endif ()

find_path(ZELOS_INCLUDE_DIR
            ZelosBase.h
          PATHS
            ${ZELOS_ROOT}/header
          DOC
            "ZELOS Include directory"
        )

if(WIN32)
    message(FATAL_ERROR "Zelos is not support WIN32")

elseif(UNIX)
    find_path(ZELOS_LIBRARY_DIR
                #libZArachneBase.so
                #libZarVisBase.so
                libZelosBase.so
                #libZelosCameraBase.so
                #libZelosClothBase.so
                #libZelosCudaBase.so
                #libZelosFurBase.so
                #libZelosMayaBase2017.so
                #libZelosMayaBase2018.so
                #libZelosMeshBase.so
                #libZelosWaterBase.so
                #libZENNBase.so
                #libZENVBase.so
                #libZeomBase.so
                #libZFXBase.so
                #_Zelos.so
            PATHS
                ${ZELOS_ROOT}/lib
            DOC
                "ZELOS Librarires directory"
            )
endif()

foreach(ZELOS_LIB
        ZelosBase
        )
    find_library(ZELOS_${GOOGLETEST_LIB}_LIBRARY
            ${ZELOS_LIB}
            HINTS
                "${ZELOS_ROOT}"
            PATH_SUFFIXES
                lib/
                lib64/
            DOC
                "Zelos's ${ZELOS_LIB} library path"
            )

    if (ZELOS_${GOOGLETEST_LIB}_LIBRARY)
        list(APPEND ZELOS_LIBRARIES ${ZELOS_${GOOGLETEST_LIB}_LIBRARY})
    endif()

endforeach(ZELOS_LIB)


include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Zelos
    REQUIRED_VARS
    	ZELOS_INCLUDE_DIR
    	ZELOS_LIBRARY_DIR
		ZELOS_LIBRARIES
)

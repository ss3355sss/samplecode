#----------------------------------------------- set compiler
set(CMAKE_C_COMPILER    ${MAYA_C_COMPILER})
set(CMAKE_CXX_COMPILER  ${MAYA_CXX_COMPILER})

#--------------------------------------------- baseMaya.a
build_library(baseMaya STATIC
        SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}

        RESOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/resource

        INCLUDE_DIR
            #--------------------------- local
            #${CUDA_INCLUDE_DIR}
            ${MAYAAPI_INCLUDE_DIR}
            ${PYTHON_INCLUDE_DIR}

            #--------------------------- backstage
            ${ALEMBIC_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR}
            ${HDF5_INCLUDE_DIR}
            ${OPENEXR_INCLUDE_DIR}
            #${OIIO_INCLUDE_DIR}
            ${USD_INCLUDE_DIR}
            #${USD_MAYA_INCLUDE_DIR}

            #--------------------------- 3rdparty
            ${EIGEN3_INCLUDE_DIR}

            #--------------------------- src/extra
            ${CMAKE_HOME_DIRECTORY}/extra

            #--------------------------- src
            ${CMAKE_HOME_DIRECTORY}/src
            #--------------------------- src/lib
            ${CMAKE_HOME_DIRECTORY}/src/lib

        EXTERNAL_LIBRARIES
            #--------------------------- local
            #${CUDA_LIBRARIES}
            ${MAYAAPI_LIBRARIES}
            ${PYTHON_LIBRARIES}

            #--------------------------- backstage
            ${ALEMBIC_LIBRARIES}
            ${BOOST_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${OPENEXR_LIBRARIES}
            #${OIIO_LIBRARIES}
            ${USD_LIBRARIES}
            #${USD_MAYA_LIBRARIES}

        STATIC_LIBRARIES
            ${CMAKE_BINARY_DIR}/src/lib/base/base.a
            ${CMAKE_BINARY_DIR}/src/lib/baseUsd/baseUsd.a
        SHARED_LIBRARIES
            ""

        PUBLISH_DIR
            ${PUBLISH_ROOT}

        #UNITTEST_REQUIRED
        )

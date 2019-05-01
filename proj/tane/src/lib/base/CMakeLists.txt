#--------------------------------------------- base.a
build_library(base STATIC
        SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}

        RESOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/resource

        INCLUDE_DIR
            #--------------------------- local
            ${CUDA_INCLUDE_DIR}
            ${PYTHON_INCLUDE_DIR}

            #--------------------------- backstage
            ${ALEMBIC_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR}
            ${HDF5_INCLUDE_DIR}
            ${OPENEXR_INCLUDE_DIR}
            #${OIIO_INCLUDE_DIR}
            #${TBB_INCLUDE_DIR}
            ${USD_INCLUDE_DIR}

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
            ${CUDA_LIBRARIES}
            ${PYTHON_LIBRARIES}

            #--------------------------- backstage
            ${ALEMBIC_LIBRARIES}
            ${BOOST_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${OPENEXR_LIBRARIES}
            #${OIIO_LIBRARIES}
            #${TBB_LIBRARIES}
            ${USD_LIBRARIES}

        STATIC_LIBRARIES
            ""
        SHARED_LIBRARIES
            ""
        PUBLISH_DIR
            ${PUBLISH_ROOT}

        #UNITTEST_REQUIRED
        )

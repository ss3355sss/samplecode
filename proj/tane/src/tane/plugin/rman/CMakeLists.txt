add_subdirectory(AlembicRiProcedural2)
#add_subdirectory(SphereProc)


build_executable(taneProcedruals_unitTest
        SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/unitTest

        RESOURCE_DIR
            ""
        INCLUDE_DIR
            #--------------------------- local
            ${CUDA_INCLUDE_DIR}
            ${PYTHON_INCLUDE_DIR}
            ${RMAN_INCLUDE_DIR}
            #--------------------------- backstage
            ${ALEMBIC_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR}
            ${HDF5_INCLUDE_DIR}
            ${OPENEXR_INCLUDE_DIR}
            ${OIIO_INCLUDE_DIR}
            #${USD_INCLUDE_DIR}

            #--------------------------- 3rdparty
            ${EIGEN3_INCLUDE_DIR}
            ${GOOGLETEST_INCLUDE_DIR}

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
            ${RMAN_LIBRARIES}
            #--------------------------- backstage
            ${ALEMBIC_LIBRARIES}
            ${BOOST_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${OPENEXR_LIBRARIES}
            ${OIIO_LIBRARIES}
            #${USD_LIBRARIES}

            #--------------------------- 3rdparty
            ${GOOGLETEST_LIBRARIES}

        STATIC_LIBRARIES
            #--------------------------- src/lib
            ${CMAKE_BINARY_DIR}/src/lib/base/base.a

        SHARED_LIBRARIES

        PUBLISH_DIR
        )

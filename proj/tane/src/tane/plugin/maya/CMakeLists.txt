#--------------------------------------------- TaneForMaya.so
build_library(TaneForMaya MAYA_PLUGIN
        SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}

        RESOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/resource

        INCLUDE_DIR
            #--------------------------- local
            ${CUDA_INCLUDE_DIR}
            ${GLEW_INCLUDE_DIR}
            ${MAYAAPI_INCLUDE_DIR}
            ${OPENGL_INCLUDE_DIR}
            ${PYTHON_INCLUDE_DIR}

            #--------------------------- backstage
            ${ALEMBIC_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR}
            ${HDF5_INCLUDE_DIR}
            ${OPENEXR_INCLUDE_DIR}
            #${OIIO_INCLUDE_DIR}
            ${USD_INCLUDE_DIR}
            ${USD_MAYA_INCLUDE_DIR}
            ${ZELOSBASE_INCLUDE_DIR}

            #--------------------------- 3rdparty
            ${EIGEN3_INCLUDE_DIR}

            #--------------------------- src/extra
            ${CMAKE_HOME_DIRECTORY}/extra

            #--------------------------- src
            ${CMAKE_HOME_DIRECTORY}/src
            #--------------------------- src/lib
            ${CMAKE_HOME_DIRECTORY}/src/lib
            #--------------------------- src/${proj}/plugin
            ${CMAKE_HOME_DIRECTORY}/src/tane/plugin

        EXTERNAL_LIBRARIES
            #--------------------------- local
            ${CUDA_LIBRARIES}
            ${GLEW_LIBRARIES}
            ${OPENGL_LIBRARIES}
            ${MAYAAPI_LIBRARIES}
            ${PYTHON_LIBRARIES}

            #--------------------------- backstage
            ${ALEMBIC_LIBRARIES}
            ${BOOST_LIBRARIES}
            ${HDF5_LIBRARIES}
            ${OPENEXR_LIBRARIES}
            #${OIIO_LIBRARIES}
            ${USD_LIBRARIES}
            ${USD_MAYA_LIBRARIES}
            ${ZELOSBASE_LIBRARIES}

        STATIC_LIBRARIES
            ${CMAKE_BINARY_DIR}/src/lib/base/base.a
            ${CMAKE_BINARY_DIR}/src/lib/baseGl/baseGl.a
            ${CMAKE_BINARY_DIR}/src/lib/baseUsd/baseUsd.a
            ${CMAKE_BINARY_DIR}/src/lib/baseMaya/baseMaya.a

            ${CMAKE_BINARY_DIR}/src/tane/tane.a

        SHARED_LIBRARIES
            ""

        PUBLISH_DIR
            ${PUBLISH_ROOT}/maya${MAYA_VER}


        #UNITTEST_REQUIRED
        )

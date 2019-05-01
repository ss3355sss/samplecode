#--------------------------------------------------------------------- decl packages
macro(configure_alembic)
    set(ALEMBIC_ROOT ${ARGV0})
    #find_package(Alembic)
    set(ALEMBIC_INCLUDE_DIRS 	/netapp/backstage/pub/lib/extern/include)
    set(ALEMBIC_LIBRARIES 		/netapp/backstage/pub/lib/extern/lib/libAlembic.so)
endmacro()

#macro(configure_atlas )
#endmacro()

#macro(configure_blas )
#endmacro()

macro(configure_boost)
    set(BOOST_ROOT ${ARGV0})
    find_package(Boost 1.55 REQUIRED
            COMPONENTS
            date_time
            filesystem
            system
            )
    set(BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
    set(BOOST_LIBRARIES ${Boost_LIBRARIES})
endmacro()

macro(configure_bullet)
    set(BULLET_ROOT ${ARGV0})
    find_package(Bullet REQUIRED
            )
    set(BULLET_INCLUDE_DIRS ${BULLET_INCLUDE_DIRS})
    set(BULLET_LIBRARIES ${BULLET_LIBRARIES})
endmacro()

macro(configure_cuda)
    find_package(CUDA REQUIRED)
    set(CUDA_INCLUDE_DIRS ${CUDA_INCLUDE_DIRS})
    set(CUDA_LIBRARIES ${CUDA_LIBRARIES})
endmacro()

#macro(configure_caffe )
#endmacro()

macro(configure_eigen3)
    set(ENV{EIGEN3_ROOT} ${ARGV0})
    find_package(Eigen3 REQUIRED)
    set(EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
    set(EIGEN3_LIBRARIES)
endmacro()

macro(configure_googletest)
    set(GOOGLETEST_ROOT ${ARGV0})
    find_package(GoogleTest REQUIRED
            )
    set(GOOGLETEST_INCLUDE_DIRS ${GOOGLETEST_INCLUDE_DIR})
    set(GOOGLETEST_LIBRARIES ${GOOGLETEST_LIBRARIES})
endmacro()

#macro(configure_glm )
#endmacro()

macro(configure_glew)
    find_package(GLEW REQUIRED
            )
    set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    set(GLEW_LIBRARIES ${GLEW_LIBRARIES})
endmacro()

macro(configure_glut)
    find_package(GLUT REQUIRED
            )
    set(GLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIR})
    set(GLUT_LIBRARIES ${GLUT_LIBRARIES})

    #message(${GLUT_INCLUDE_DIRS})
    #message(${GLUT_LIBRARIES})

endmacro()

macro(configure_hdf5)
    set(HDF5_ROOT ${ARGV0})
    find_package(HDF5 REQUIRED)

    set(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS})
    set(HDF5_LIBRARIES ${HDF5_LIBRARIES})
endmacro()

macro(configure_hdk)
    set(HDF5_ROOT ${ARGV0})
    #find_package(HDK REQUIRED)

    set(HDK_INCLUDE_DIRS )
    set(HDK_LIBRARIES    )

endmacro()

macro(configure_ilmBase)
    set(ILMBase_ROOT ${ARGV0})
    find_package(ILMBase REQUIRED)
    set(ILMBASE_INCLUDE_DIRS ${ILMBASE_INCLUDE_DIRS})
    set(ILMBASE_LIBRARIES ${ILMBASE_LIBRARIES})
endmacro()

#macro(configure_jsoncpp )
#endmacro()

#macro(configure_lapack )
#endmacro()

macro(configure_mayaAPI)
    set(MAYA_BASE_DIR /usr/autodesk/maya${ARGV1})
    find_package(Maya REQUIRED)
    set(MAYAAPI_INCLUDE_DIRS ${MAYA_INCLUDE_DIR})
    set(MAYAAPI_LIBRARIES ${MAYA_LIBRARIES})
endmacro()

#macro(configure_openblas )
#endmacro()

macro(configure_openexr)
    set(OPENEXR_LOCATION ${ARGV0})
    find_package(OpenEXR REQUIRED)
    set(OPENEXR_INCLUDE_DIR ${OPENEXR_INCLUDE_DIR})
    set(OPENEXR_LIBRARIES ${OPENEXR_LIBRARIES})

endmacro()

#macro(configure_opencv )
#endmacro()

macro(configure_opengl)
    find_package(OpenGL REQUIRED
            )
    set(OPENGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES})


endmacro()

macro(configure_openmesh)
    set(OPENMESH_LIBRARY_DIR ${ARGV0})
    find_package(OpenMesh REQUIRED
            )
    set(OPENMESH_INCLUDE_DIRS ${OPENMESH_INCLUDE_DIRS})
    set(OPENMESH_LIBRARIES ${OPENMESH_LIBRARIES})
endmacro()

#macro(configure_oiio )
#endmacro()

#macro(configure_openexr )
#endmacro()

macro(configure_pythoninterp)
    if (${ARGV1})
        find_package(PythonInterp ${ARGV1} REQUIRED
                )
    else ()
        find_package(PythonInterp 2.7 REQUIRED
                )
    endif ()
endmacro()

macro(configure_pythonlibs)
    if (${ARGV1})
        find_package(PythonLibs ${ARGV1} REQUIRED)
    else ()
        find_package(PythonLibs 2.7 REQUIRED)
    endif ()
    set(${PYTHON_INCLUDE_DIRS})
    set(${PYTHON_LIBRARIES})
endmacro()

macro(configure_rbdl)
    set(RBDL_INCLUDE_DIRS ${ARGV0}/include)
    set(RBDL_LIBRARIES
            ${ARGV0}/lib/librbdl.a
            ${ARGV0}/lib/librbdl_geometry.a
            ${ARGV0}/lib/librbdl_muscle.a
            )
endmacro()

#macro(configure_redshift )
#endmacro()

macro(configure_renderman)
    set(RENDERMAN_LOCATION ${ARGV0}/RenderManProServer-${ARGV1})
    find_package(Renderman REQUIRED)
    set(RENDERMAN_INCLUDE_DIRS ${RENDERMAN_INCLUDE_DIR})
    set(RENDERMAN_LIBRARY ${RENDERMAN_LIBRARY})
endmacro()

macro(configure_tbb)
    #find_package(TBB REQUIRED)
    set(TBB_INCLUDE_DIRS ${ARGV0}/include)
    set(TBB_LIBRARIES
            ${ARGV0}/lib/libtbb.so
            ${ARGV0}/lib/libtbbmalloc.so
            ${ARGV0}/lib/libtbbmalloc_proxy.so
            )
endmacro()

macro(configure_usd)
    set(USD_ROOT ${ARGV0})
    find_package(USD REQUIRED)
    set(USD_INCLUDE_DIR ${USD_INCLUDE_DIR})
    set(USD_LIBRARY_DIR ${USD_LIBRARY_DIR})
endmacro()

#macro(configure_usdMaya )
#endmacro()

#macro(configure_usdHoudini )
#endmacro()

#macro(configure_usdKatana )
#endmacro()

macro(configure_zelos)
    set(ZELOS_ROOT ${ARGV0})
    find_package(Zelos REQUIRED)
    set(ZELOS_INCLUDE_DIR ${ZELOS_INCLUDE_DIR})
    set(ZELOS_LIBRARIES ${ZELOS_LIBRARIES})
endmacro()



#--------------------------------------------------------------------- macros
MACRO(_add_package_properties)
    foreach (package ${args_PACKAGE})
        string(TOUPPER ${package} upperCase)
        set(include_dirs ${${upperCase}_INCLUDE_DIRS})
        set(libraries ${${upperCase}_LIBRARIES})
        if (NOT include_dirs)
            message(FATAL_ERROR "Failed to get ${upperCase}_INCLUDE_DIRS")
        endif ()
        #if (NOT libraries)
        #    message(FATAL_WARNNING "Failed to get ${upperCase}_LIBRARIES")
        #endif ()

        list(APPEND INCLUDE_DIRS ${include_dirs})
        list(APPEND LIBRARIES ${libraries})
    endforeach ()
endMACRO()

MACRO(_set_target_properties)
    #------------------------ add dependencies
    if (args_ADDITIONAL_LIBRARIES)
        foreach (LIB ${args_ADDITIONAL_LIBRARIES})
            get_filename_component(NAME ${LIB} NAME_WE)
            add_dependencies(${TARGET_NAME} ${NAME})
        endforeach ()
    endif ()

    #------------------------ set definitions to target
    set(DEFINITIONS ${args_ADDITIONAL_DEFINITION})
    target_compile_definitions(${TARGET_NAME} PUBLIC
            ${DEFINITIONS}
            )


    #------------------------ set package to target
    set(INCLUDE_DIRS ${args_ADDITIONAL_INCLUDE_DIR})
    set(LIBRARIES ${args_ADDITIONAL_LIBRARIES})
    if (args_PACKAGE)
        foreach (package ${args_PACKAGE})
            string(TOUPPER ${package} upperCase)
            set(include_dirs ${${upperCase}_INCLUDE_DIRS})
            set(libraries ${${upperCase}_LIBRARIES})
            if (NOT include_dirs)
                message(FATAL_ERROR "Failed to get ${upperCase}_INCLUDE_DIRS")
            endif ()
            #if (NOT libraries)
            #    message(FATAL_WARNNING "Failed to get ${upperCase}_LIBRARIES")
            #endif ()

            list(APPEND INCLUDE_DIRS ${include_dirs})
            list(APPEND LIBRARIES ${libraries})
        endforeach ()
    endif ()

    #------------------------ set include directories to target
    target_include_directories(${TARGET_NAME} PUBLIC
            ${INCLUDE_DIRS}
            )
    #message(${INCLUDE_DIRS})
    #------------------------ set link libraries to target
    target_link_libraries(${TARGET_NAME}
            ${LIBRARIES}
            )

    #------------------------ apply cotire to target
    if (__COTIRE_INCLUDED)
        cotire(${TARGET_NAME})
    endif ()

endMACRO()

MACRO(_set_target_unittest)
    #----------------------------------------------- set sources
    set(UNITTEST_SOURCE_DIR
            ${CMAKE_HOME_DIRECTORY}/extra
            ${CMAKE_CURRENT_SOURCE_DIR}/unitTest
            )
    foreach (DIR ${UNITTEST_SOURCE_DIR})
        file(GLOB UNITTEST_CXX_FILES ${UNITTEST_CXX_FILES} ${DIR}/*.h ${DIR}/*.cpp)
        file(GLOB UNITTEST_NVCC_FILES ${UNITTEST_NVCC_FILES} ${DIR}/*.cuh ${DIR}/*.cu)
    endforeach ()

    #---------------------------------------------set unit test executable
    if (${CUDA_FOUND})
        cuda_add_executable(${TARGET_NAME}_unitTest
                ${UNITTEST_CXX_FILES}
                ${UNITTEST_NVCC_FILES}
                )
    else ()
        add_executable(${TARGET_NAME}_unitTest
                ${UNITTEST_CXX_FILES}
                ${UNITTEST_NVCC_FILES}
                )
    endif ()

    #-------------------------------------------- add unit test dependencies
    add_dependencies(${TARGET_NAME}_unitTest
            ${TARGET_NAME}
            )

    #-------------------------------------------- set include directory to unit test target
    target_include_directories(${TARGET_NAME}_unitTest PUBLIC
            ${INCLUDE_DIRS}

            # include opengl for unit test
            ${GLEW_INCLUDE_DIR}
            ${GLUT_INCLUDE_DIR}
            ${OPENGL_INCLUDE_DIR}

            # include google for unit test
            ${GOOGLETEST_INCLUDE_DIR}

            # glxFramework
            /home/dohyeon.yang/work/extra
            )

    #-------------------------------------------- set link libraries to unit test target
    if (BUILD_TYPE)
        if (${BUILD_TYPE} STREQUAL STATIC)
            set(TARGET_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.a)
        elseif (${BUILD_TYPE} STREQUAL SHARED)
            set(TARGET_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.so)
        endif ()
    endif ()
    target_link_libraries(${TARGET_NAME}_unitTest
            ${LIBRARIES}

            # link current target
            ${TARGET_LIBRARY}

            # link opengl for unit test
            ${GLEW_LIBRARIES}
            ${GLUT_LIBRARIES}
            ${OPENGL_LIBRARIES}

            # link google for unit test
            ${GOOGLETEST_LIBRARIES}

            # link libX11 for glx framework
            /lib64/libX11.so

            )
    #------------------------ set definitions libraries to target

endMACRO()


#--------------------------------------------------------------------- functions
function(build_executable TARGET_NAME)
    set(options
            UNITTEST_REQUIRED
            )
    set(oneValueArgs

            )
    set(multiValueArgs
            SOURCE_DIR
            RESOURCE_DIR
            PACKAGE
            ADDITIONAL_DEFINITION
            ADDITIONAL_INCLUDE_DIR
            ADDITIONAL_LIBRARIES
            )
    cmake_parse_arguments(args
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN}
            )

    #----------------------------------------------- configure target sources
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_HEADER_FILES ${CXX_HEADER_FILES} ${DIR}/*.h)
        file(GLOB NVCC_HEADER_FILES ${NVCC_HEADER_FILES} ${DIR}/*.cuh)
    endforeach ()
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_SOURCE_FILES ${CXX_SOURCE_FILES} ${DIR}/*.cpp)
        file(GLOB NVCC_SOURCE_FILES ${NVCC_SOURCE_FILES} ${DIR}/*.cu)
    endforeach ()

    #----------------------------------------------- add executable
    if (${CUDA_FOUND})
        cuda_add_executable(${TARGET_NAME}
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    else ()
        add_executable(${TARGET_NAME}
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    endif ()


    #----------------------------------------------- configure target property
    _set_target_properties()

    #------------------------ set install
    install(TARGETS
            ${TARGET_NAME}
            RUNTIME DESTINATION
            ${TARGET_NAME}
            ARCHIVE DESTINATION
            ${TARGET_NAME}/lib
            LIBRARY DESTINATION
            ${TARGET_NAME}/lib
            )
    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/resource
            DESTINATION
            ${CMAKE_INSTALL_PREFIX}
            )

    #------------------------ set unit test
    if (${args_UNITTEST_REQUIRED})
        _set_target_unittest()
    endif ()
endfunction()

function(build_library TARGET_NAME BUILD_TYPE)
    set(options
            UNITTEST_REQUIRED
            )
    set(oneValueArgs

            )
    set(multiValueArgs
            SOURCE_DIR
            RESOURCE_DIR
            PACKAGE
            ADDITIONAL_DEFINITION
            ADDITIONAL_INCLUDE_DIR
            ADDITIONAL_LIBRARIES
            )
    cmake_parse_arguments(args
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN}
            )

    #----------------------------------------------- configure target sources
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_HEADER_FILES ${CXX_HEADER_FILES} ${DIR}/*.h)
        file(GLOB NVCC_HEADER_FILES ${NVCC_HEADER_FILES} ${DIR}/*.cuh)
    endforeach ()
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_SOURCE_FILES ${CXX_SOURCE_FILES} ${DIR}/*.cpp)
        file(GLOB NVCC_SOURCE_FILES ${NVCC_SOURCE_FILES} ${DIR}/*.cu)
    endforeach ()

    #----------------------------------------------- add executable
    #message(${TARGET_NAME})
    if (${CUDA_FOUND})
        cuda_add_library(${TARGET_NAME} ${BUILD_TYPE}
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    else ()
        add_library(${TARGET_NAME} ${BUILD_TYPE}
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    endif ()

    #----------------------------------------------- set target prefix, suffix
    if (${BUILD_TYPE} STREQUAL STATIC)
        set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".a")
    elseif (${BUILD_TYPE} STREQUAL SHARED)
        set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".so")
    endif ()


    #----------------------------------------------- configure target property
    _set_target_properties()

    #------------------------ set install
    install(TARGETS
            ${TARGET_NAME}
            RUNTIME DESTINATION
            ${TARGET_NAME}
            ARCHIVE DESTINATION
            ${TARGET_NAME}/lib
            LIBRARY DESTINATION
            ${TARGET_NAME}/lib
            )
    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/resource
            DESTINATION
            ${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}
            )

    #------------------------ set unit test
    if (${args_UNITTEST_REQUIRED})
        _set_target_unittest()
    endif ()
endfunction()

function(build_maya_plugin TARGET_NAME)
    set(options
            UNITTEST_REQUIRED
            )
    set(oneValueArgs
            MAYA_VER
            )
    set(multiValueArgs
            SOURCE_DIR
            RESOURCE_DIR
            PACKAGE
            ADDITIONAL_DEFINITION
            ADDITIONAL_INCLUDE_DIR
            ADDITIONAL_LIBRARIES
            )
    cmake_parse_arguments(args
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN}
            )

    #----------------------------------------------- configure target sources
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_HEADER_FILES ${CXX_HEADER_FILES} ${DIR}/*.h)
        file(GLOB NVCC_HEADER_FILES ${NVCC_HEADER_FILES} ${DIR}/*.cuh)
    endforeach ()
    foreach (DIR ${args_SOURCE_DIR})
        file(GLOB CXX_SOURCE_FILES ${CXX_SOURCE_FILES} ${DIR}/*.cpp)
        file(GLOB NVCC_SOURCE_FILES ${NVCC_SOURCE_FILES} ${DIR}/*.cu)
    endforeach ()

    #----------------------------------------------- add executable
    if (${CUDA_FOUND})
        cuda_add_library(${TARGET_NAME} SHARED
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    else ()
        add_library(${TARGET_NAME} SHARED
                ${CXX_HEADER_FILES}
                ${NVCC_HEADER_FILES}
                ${CXX_SOURCE_FILES}
                ${NVCC_SOURCE_FILES}
                )
    endif ()
    #----------------------------------------------- set target prefix, suffix
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "" SUFFIX ".so")

    #----------------------------------------------- configure target property
    _set_target_properties()

    #------------------------ set install
    install(TARGETS
            ${TARGET_NAME}
            ARCHIVE DESTINATION
            maya${args_MAYA_VER}/plug-ins
            LIBRARY DESTINATION
            maya${args_MAYA_VER}/plug-ins
            )
    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/resource
            DESTINATION
            ${CMAKE_INSTALL_PREFIX}/maya${args_MAYA_VER}
            )

    #------------------------ set unit test
    #if (${args_UNITTEST_REQUIRED})
    #    _set_target_unittest()
    #endif ()
endfunction()


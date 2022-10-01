#Add zlib library

# We first try to find zlib in the system
if (NOT BOTCRAFT_FORCE_LOCAL_ZLIB)
    find_package(ZLIB QUIET)
endif()

# If not found, build from sources
if(NOT TARGET ZLIB::ZLIB)
    message(STATUS "Can't find Zlib, cloning and building it from sources")
    
    file(GLOB RESULT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib/examples")
    list(LENGTH RESULT RES_LEN)
    if(RES_LEN EQUAL 0)
        message(STATUS "Zlib not found, cloning it...")
        execute_process(COMMAND git submodule update --init -- 3rdparty/zlib WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(ZLIB_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib")
    set(ZLIB_BUILD_PATH "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib")

    file(MAKE_DIRECTORY "${ZLIB_BUILD_PATH}")

    execute_process(
        COMMAND "${CMAKE_COMMAND}" "${ZLIB_SRC_PATH}" "-G" "${CMAKE_GENERATOR}" "-A" "${CMAKE_GENERATOR_PLATFORM}" "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=install" "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        WORKING_DIRECTORY "${ZLIB_BUILD_PATH}")

    execute_process(COMMAND "${CMAKE_COMMAND}" "--build" "." "--target" "install" "--parallel" "2" "--config" "Release" WORKING_DIRECTORY "${ZLIB_BUILD_PATH}")

    # Find the freshly built library

    # From 3.24 there is a cmake option to find zlib static
    # but before, we need to do it in a more manual way
    if(${CMAKE_VERSION} VERSION_LESS "3.24.0")
        set(_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
        set(CMAKE_FIND_LIBRARY_SUFFIXES "static.lib" ".a")
    else()
        set(ZLIB_USE_STATIC_LIBS "ON")
    endif()

    set(ZLIB_ROOT "${ZLIB_BUILD_PATH}/install")
    find_package(ZLIB QUIET)

    # Revert library suffix to what they were
    if(${CMAKE_VERSION} VERSION_LESS "3.24.0")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${_CMAKE_FIND_LIBRARY_SUFFIXES}) 
        unset(_CMAKE_FIND_LIBRARY_SUFFIXES)
    endif()
endif()

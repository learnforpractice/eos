add_subdirectory(boost)

set (BOOST_VERSION  1.71.0)

message (STATUS "Adding Boost ${BOOST_VERSION} as an external project.")

set (BOOST_BOOTSTRAP_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/boostbuild)
set (BOOST_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/boostbuild)
set (BOOST_BUILD_PREFIX ${BOOST_BOOTSTRAP_PREFIX}/boost)
set (BOOST_GIT_REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/boost)

if (WIN32)
    set (BOOST_BOOTSTRAP_CMD ${BOOST_GIT_REPO_DIR}/bootstrap.bat)
    # It was bjam at one point; it is now b2
    set (BOOST_BUILD_CMD ${BOOST_GIT_REPO_DIR}/b2.exe)
elseif (UNIX)
    set (BOOST_BOOTSTRAP_CMD ./${BOOST_GIT_REPO_DIR}/bootstrap.sh)
    set (BOOST_BUILD_CMD ./${BOOST_GIT_REPO_DIR}/b2)
endif ()

#  TODO: TBD: For now, assuming only 64-bit address models.
set (BOOST_ADDR_MODEL 64)

# TODO: TBD: align with the toolchain that we want to build through
if (MSVC_VERSION EQUAL 1900)
    set (BOOST_TOOLSET msvc-14.0)
endif ()

ExternalProject_Add (boost
    # PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/repos/boost
    GIT_REPOSITORY git at github.com:boostorg/boost.git
    GIT_TAG boost-${BOOST_VERSION}
    GIT_PROGRESS ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:STRING=${BOOST_INSTALL_PREFIX}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/repos/boost
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_CMD} --prefix=${BOOST_BOOTSTRAP_PREFIX}
    BUILD_COMMAND ${BOOST_BUILD_CMD} --with-test toolset=${BOOST_TOOLSET} variant=debug link=static stage --prefix=${BOOST_BUILD_PREFIX}
    LOG_DOWNLOAD ON
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)

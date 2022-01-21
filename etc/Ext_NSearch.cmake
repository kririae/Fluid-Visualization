include(ExternalProject)

set(ExternalInstallDir "${CMAKE_BINARY_DIR}/extern/install")
set(USE_DOUBLE_PRECISION OFF)

ExternalProject_Add(
        Ext_CompactNSearch
        PREFIX "${CMAKE_BINARY_DIR}/extern/CompactNSearch"
        GIT_REPOSITORY https://github.com/InteractiveComputerGraphics/CompactNSearch.git
        GIT_TAG "3f11ece16a419fc1cc5795d6aa87cb7fe6b86960"
        INSTALL_DIR ${ExternalInstallDir}/NeighborhoodSearch
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${EXT_CMAKE_BUILD_TYPE} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE} -DCMAKE_INSTALL_PREFIX:PATH=${ExternalInstallDir}/NeighborhoodSearch -DUSE_DOUBLE_PRECISION:BOOL=${USE_DOUBLE_PRECISION} -DBUILD_DEMO:BOOL=OFF
)

ExternalProject_Get_Property(
        Ext_CompactNSearch
        INSTALL_DIR
)

set(NEIGHBORHOOD_SEARCH_LIBRARIES ${INSTALL_DIR}/lib/${LIB_PREFIX}${NEIGHBORHOOD_ASSEMBLY_NAME}${LIB_SUFFIX})
set(NEIGHBORHOOD_ASSEMBLY_NAME CompactNSearch)
set(NEIGHBORHOOD_SEARCH_INCLUDE_DIR ${INSTALL_DIR}/include/)

unset(INSTALL_DIR)

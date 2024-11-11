
aux_source_directory(src SRC_FILES)
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

include_directories(include)
include_directories("../3dparty")

find_package(OpenMP REQUIRED)

add_executable(volumeRendering ${SRC_FILES} ${HEADER_FILES})

target_link_libraries(volumeRendering
  PUBLIC
      OpenMP::OpenMP_CXX
 )
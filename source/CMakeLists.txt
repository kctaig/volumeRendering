
aux_source_directory(src SRC_FILES)
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

include_directories(include)
include_directories("../3dparty")

add_executable(volumeRendering ${SRC_FILES} ${HEADER_FILES})
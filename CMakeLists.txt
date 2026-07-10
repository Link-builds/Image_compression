cmake_minimum_required( VERSION 3.5.0 )
project( MASTER )
cmake_policy( SET CMP0072 NEW )

set( src )
add_subdirectory( "src" )

include_directories( "${PROJECT_SOURCE_DIR}/include" )
include_directories( "${PROJECT_SOURCE_DIR}/vendor" )

add_executable( "${PROJECT_NAME}" "${src}" )

# For assets
set( SOURCE_DIR_RSC "${PROJECT_SOURCE_DIR}/resource" )
set( DEST_DIR_RSC "${CMAKE_BINARY_DIR}/resource" )

add_custom_target(
                    copy_resource ALL
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR_RSC} ${DEST_DIR_RSC}
                 )

add_custom_command(
                    TARGET "${PROJECT_NAME}" PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${SOURCE_DIR_RSC} ${DEST_DIR_RSC}
                  )

# For output directory
set( OUTPUT_DIR "${CMAKE_BINARY_DIR}/output" )
file( MAKE_DIRECTORY ${OUTPUT_DIR} )


# For external libs

# stb_image
add_library( stb_image STATIC vendor/stb_image/stb_image.c vendor/stb_image/stb_image_write.c )
target_include_directories( stb_image PUBLIC vendor/stb_image )
target_link_libraries( ${PROJECT_NAME} PRIVATE stb_image )

# Dependencies
add_dependencies( "${PROJECT_NAME}" copy_resource )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
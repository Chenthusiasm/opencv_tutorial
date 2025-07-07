cmake_minimum_required( VERSION 2.8 )
project( opencv-tutorial LANGUAGES C CXX)

set( CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -Wuninitialized \
	-Wunused-variable -Wno-double-promotion -Wmissing-prototypes \
	-Wmissing-declarations -Werror=undef  ${CMAKE_C_FLAGS}" )
set( CMAKE_CXX_FLAGS "-std=gnu++11 -pthread -Wall -Wextra -Wuninitialized -Wno-attributes \
	-Wunused-variable -Wno-double-promotion -Wno-missing-field-initializers \
	-Wmissing-declarations -Werror=undef  ${CMAKE_CXX_FLAGS}" )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# tell the linker not to worry about missing symbols in libraries
set( CMAKE_C_FLAGS   "-Wl,--unresolved-symbols=ignore-in-shared-libs ${CMAKE_C_FLAGS}" )
set( CMAKE_CXX_FLAGS "-Wl,--unresolved-symbols=ignore-in-shared-libs ${CMAKE_CXX_FLAGS}" )

# link against all possible opencv libs
file( GLOB OpenCV_LIBS "/usr/local/lib/libopencv*.so" )
find_package( OpenCV REQUIRED )

# project
set( TARGET run )
file( GLOB all_src_files "src/*.c*" )
add_executable( ${TARGET} ${all_src_files} )
include_directories( ${OpenCV_INCLUDE_DIRS}
    include/
)
target_link_libraries( ${TARGET} ${OpenCV_LIBS} )
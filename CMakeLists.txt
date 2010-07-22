# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.


project( watershedtin )

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
 
if ( COMMAND cmake_policy )
    cmake_policy( SET CMP0003 NEW )  
endif()
 
find_package(CGAL QUIET COMPONENTS Core )

if ( CGAL_FOUND )

    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS "-Wall -march=native -mtune=generic -pipe -frounding-math")

    include( ${CGAL_USE_FILE} )

    add_executable( reader utils.cpp reader.cpp )
    add_to_cached_list( CGAL_EXECUTABLE_TARGETS reader)

    # Link the executable to CGAL and third-party libraries
    if ( CGAL_AUTO_LINK_ENABLED )    
        target_link_libraries(reader ${CGAL_3RD_PARTY_LIBRARIES} )
    else()
        target_link_libraries(reader ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )
    endif()

    # create_single_source_cgal_program( "reader.cpp" )
    # create_single_source_cgal_program( "tri_reader.cpp" )
    # create_single_source_cgal_program( "union_find.cpp" )

else()
  
    message(STATUS "This program requires the CGAL library, and will not be compiled.")
  
endif()


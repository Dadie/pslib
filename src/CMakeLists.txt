cmake_minimum_required(VERSION 3.8)

# Source
set (SOURCES
	pslib/main.cpp
)
include_directories("./")

add_library (${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "./")
target_link_libraries(${PROJECT_NAME} PUBLIC pthread ${Boost_LIBRARIES})

add_library (${PROJECT_NAME}_s STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME}_s PUBLIC "./")
target_link_libraries(${PROJECT_NAME}_s PUBLIC pthread ${Boost_LIBRARIES})

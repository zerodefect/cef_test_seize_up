
cmake_minimum_required(VERSION 3.9)


project(cef_test_seize_up)

message(STATUS "Project:    ${PROJECT_NAME}")
message(STATUS "Platform:   ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM})")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

################################################################
# Find Thread - used implicitly by GTest
find_package(Threads REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

link_directories(${CMAKE_BINARY_DIR}/bin)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include(ExternalProject)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

find_package(Boost REQUIRED COMPONENTS
	filesystem
	)

################################################################
# Cef 3
ExternalProject_Add(
	cef3
	URL http://opensource.spotify.com/cefbuilds/cef_binary_86.0.18%2Bgd3ead8b%2Bchromium-86.0.4240.111_linux64_minimal.tar.bz2
	URL_HASH SHA1=9f7c2e728e1573297ba247d153baf323852f175a

	UPDATE_DISCONNECTED 0

	CMAKE_ARGS -DCMAKE_BUILD_TYPE=RELWITHDEBINFO -DCMAKE_CXX_FLAGS="-Wno-undefined-var-template" -DCMAKE_POSITION_INDEPENDENT_CODE=ON

	# Yes, we have to next the directories to keep cef3 happy
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/include ${EXTERNAL_INSTALL_LOCATION}/include/include &&
	${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/Release ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} &&
	${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/Resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} &&
	mkdir -p ${EXTERNAL_INSTALL_LOCATION}/lib &&
	${CMAKE_COMMAND} -E copy <BINARY_DIR>/libcef_dll_wrapper/libcef_dll_wrapper.a ${EXTERNAL_INSTALL_LOCATION}/lib/
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

# Local header files here ONLY
set(CEF_TEST_TARGET_HPP
	app.hpp
	browser.hpp
	browser_client.hpp
	browser_process_handler.hpp
	render_handler.hpp
    )

# Local source files here
set(CEF_TEST_TARGET_CPP
    main.cpp
	app.cpp
	browser.cpp
	browser_client.cpp
	browser_process_handler.cpp
    )

# Define an executable
add_executable(cef_test ${CEF_TEST_TARGET_HPP} ${CEF_TEST_TARGET_CPP})

# External dependencies
add_dependencies(cef_test cef3)

target_link_libraries(cef_test
	Boost::filesystem
	cef_dll_wrapper
	cef
	event
	event_pthreads
	X11
    pthread
	dl
    )

# Local header files here ONLY
set(CEF_TEST_WORKER_TARGET_HPP
	app_worker.hpp
	)

# Local source files here
set(CEF_TEST_WORKER_TARGET_CPP
	app_worker.cpp
	worker.cpp
	)

# Define an executable
add_executable(cef_test_worker ${CEF_TEST_WORKER_TARGET_HPP} ${CEF_TEST_WORKER_TARGET_CPP})

# External dependencies
add_dependencies(cef_test_worker cef3)

target_link_libraries(cef_test_worker
	cef_dll_wrapper
	cef
	)

cmake_minimum_required(VERSION 3.8)

include_directories (
	"./"
)

function(add_test_helper test_name exec_name text_cpp)

	add_executable (${exec_name} ${text_cpp})
	target_link_libraries (
		${exec_name}
		${PROJECT_NAME}_s
		${Boost_LIBRARIES}
	)

	add_test (NAME ${test_name} COMMAND ${exec_name})

endfunction(add_test_helper)

add_test_helper ("PSLIB_V1_0_SAVE_PSI"  "PSLIB_V1_0_SAVE_PSI"  "./pslib/v1_0/test.save_psi.cpp")
add_test_helper ("PSLIB_V1_0_LOAD_PSI"  "PSLIB_V1_0_LOAD_PSI"  "./pslib/v1_0/test.load_psi.cpp")
add_test_helper ("PSLIB_V1_0_SAVE_AND_LOAD_PSI"  "PSLIB_V1_0_SAVE_AND_LOAD_PSI"  "./pslib/v1_0/test.save_and_load_psi.cpp")
add_test_helper ("PSLIB_V1_0_SAVE_SAMPLES"  "PSLIB_V1_0_SAVE_SAMPLES"  "./pslib/v1_0/test.save_samples.cpp")
add_test_helper ("PSLIB_V1_0_LOAD_SAMPLES"  "PSLIB_V1_0_LOAD_SAMPLES"  "./pslib/v1_0/test.load_samples.cpp")
add_test_helper ("PSLIB_V1_0_SAVE_AND_LOAD_SAMPLES"  "PSLIB_V1_0_SAVE_AND_LOAD_SAMPLES"  "./pslib/v1_0/test.save_and_load_samples.cpp")

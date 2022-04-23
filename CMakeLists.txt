cmake_minimum_required(VERSION 3.20)
project(c_1_Nomad192 C)
set(CMAKE_C_STANDARD 99)

add_library(my_ptr_unchained STATIC my_ptr_unchained.c my_ptr_unchained.h)
add_library(my_logging STATIC my_logging.c my_logging.h return_codes.h)
add_library(fp_comp STATIC fp_comp.c fp_comp.h my_logging)
add_library(gauss_jordan_elimination_algorithm STATIC gauss_jordan_elimination_algorithm.c gauss_jordan_elimination_algorithm.h fp_comp my_logging)

add_executable(c_1_Nomad192 main.c return_codes.h)

target_link_libraries(c_1_Nomad192 my_ptr_unchained)
target_link_libraries(c_1_Nomad192 my_logging)
target_link_libraries(c_1_Nomad192 fp_comp)
target_link_libraries(c_1_Nomad192 gauss_jordan_elimination_algorithm)

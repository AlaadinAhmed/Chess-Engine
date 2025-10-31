# CMake generated Testfile for 
# Source directory: /home/aladdin/Chess-Engine
# Build directory: /home/aladdin/Chess-Engine/out/Debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[comprehensive_debug]=] "/home/aladdin/Chess-Engine/out/Debug/bin/comprehensive_debug")
set_tests_properties([=[comprehensive_debug]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[debug_a3b4]=] "/home/aladdin/Chess-Engine/out/Debug/bin/debug_a3b4")
set_tests_properties([=[debug_a3b4]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[debug_movegen]=] "/home/aladdin/Chess-Engine/out/Debug/bin/debug_movegen")
set_tests_properties([=[debug_movegen]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[eval_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/eval_test")
set_tests_properties([=[eval_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[fen_illegal_move_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/fen_illegal_move_test")
set_tests_properties([=[fen_illegal_move_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[fen_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/fen_test")
set_tests_properties([=[fen_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[illegal_move_debug]=] "/home/aladdin/Chess-Engine/out/Debug/bin/illegal_move_debug")
set_tests_properties([=[illegal_move_debug]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[illegal_move_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/illegal_move_test")
set_tests_properties([=[illegal_move_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[movegen_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/movegen_test")
set_tests_properties([=[movegen_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[perft_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/perft_test")
set_tests_properties([=[perft_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[simple_search_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/simple_search_test")
set_tests_properties([=[simple_search_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[test_runner]=] "/home/aladdin/Chess-Engine/out/Debug/bin/test_runner")
set_tests_properties([=[test_runner]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
add_test([=[utils_test]=] "/home/aladdin/Chess-Engine/out/Debug/bin/utils_test")
set_tests_properties([=[utils_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/aladdin/Chess-Engine/CMakeLists.txt;72;add_test;/home/aladdin/Chess-Engine/CMakeLists.txt;0;")
subdirs("extern/googletest")

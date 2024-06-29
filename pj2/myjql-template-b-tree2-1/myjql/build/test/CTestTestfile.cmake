# CMake generated Testfile for 
# Source directory: E:/database2/pj2/myjql-template-b-tree2-1/myjql/test
# Build directory: E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(test_hash_map "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Debug/test_hash_map.exe")
  set_tests_properties(test_hash_map PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;17;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(test_hash_map "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Release/test_hash_map.exe")
  set_tests_properties(test_hash_map PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;17;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(test_hash_map "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/MinSizeRel/test_hash_map.exe")
  set_tests_properties(test_hash_map PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;17;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(test_hash_map "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/RelWithDebInfo/test_hash_map.exe")
  set_tests_properties(test_hash_map PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;17;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
else()
  add_test(test_hash_map NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(test_str "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Debug/test_str.exe")
  set_tests_properties(test_str PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;22;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(test_str "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Release/test_str.exe")
  set_tests_properties(test_str PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;22;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(test_str "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/MinSizeRel/test_str.exe")
  set_tests_properties(test_str PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;22;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(test_str "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/RelWithDebInfo/test_str.exe")
  set_tests_properties(test_str PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;22;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
else()
  add_test(test_str NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(test_b_tree "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Debug/test_b_tree.exe")
  set_tests_properties(test_b_tree PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;27;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(test_b_tree "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/Release/test_b_tree.exe")
  set_tests_properties(test_b_tree PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;27;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(test_b_tree "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/MinSizeRel/test_b_tree.exe")
  set_tests_properties(test_b_tree PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;27;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(test_b_tree "E:/database2/pj2/myjql-template-b-tree2-1/myjql/build/test/RelWithDebInfo/test_b_tree.exe")
  set_tests_properties(test_b_tree PROPERTIES  _BACKTRACE_TRIPLES "E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;27;add_test;E:/database2/pj2/myjql-template-b-tree2-1/myjql/test/CMakeLists.txt;0;")
else()
  add_test(test_b_tree NOT_AVAILABLE)
endif()

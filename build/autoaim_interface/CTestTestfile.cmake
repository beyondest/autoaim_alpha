# CMake generated Testfile for 
# Source directory: /home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_interface
# Build directory: /home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(lint_cmake "/usr/local/bin/python3" "-u" "/opt/ros/dashing/share/ament_cmake_test/cmake/run_test.py" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/test_results/autoaim_interface/lint_cmake.xunit.xml" "--package-name" "autoaim_interface" "--output-file" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/ament_lint_cmake/lint_cmake.txt" "--command" "/opt/ros/dashing/bin/ament_lint_cmake" "--xunit-file" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/test_results/autoaim_interface/lint_cmake.xunit.xml")
set_tests_properties(lint_cmake PROPERTIES  LABELS "lint_cmake;linter" TIMEOUT "60" WORKING_DIRECTORY "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_interface")
add_test(xmllint "/usr/local/bin/python3" "-u" "/opt/ros/dashing/share/ament_cmake_test/cmake/run_test.py" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/test_results/autoaim_interface/xmllint.xunit.xml" "--package-name" "autoaim_interface" "--output-file" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/ament_xmllint/xmllint.txt" "--command" "/opt/ros/dashing/bin/ament_xmllint" "--xunit-file" "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/build/autoaim_interface/test_results/autoaim_interface/xmllint.xunit.xml")
set_tests_properties(xmllint PROPERTIES  LABELS "xmllint;linter" TIMEOUT "60" WORKING_DIRECTORY "/home/liyuxuan/vscode/pywork_linux/autoaim_ws/src/autoaim_interface")
subdirs("autoaim_interface__py")

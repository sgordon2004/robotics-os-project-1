cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)

project(the-basix)

find_package(Threads REQUIRED)

add_library(robot src/robot/robot.c)
target_link_libraries(robot m Threads::Threads)
target_include_directories(robot PRIVATE include/)

add_executable(drive_square src/drive_square.c src/util.c)
target_link_libraries(drive_square robot)
target_include_directories(drive_square PRIVATE include/)

add_executable(cmd_receiver src/cmd_receiver.c src/util.c)
target_link_libraries(cmd_receiver robot)
target_include_directories(cmd_receiver PRIVATE include/)

add_executable(cmd_parser src/cmd_parser.c src/util.c)
target_link_libraries(cmd_parser robot)
target_include_directories(cmd_parser PRIVATE include/)

add_executable(teleop src/teleop.c src/util.c)
target_link_libraries(teleop robot)
target_include_directories(teleop PRIVATE include/)

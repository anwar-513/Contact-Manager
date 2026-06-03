cmake_minimum_required(VERSION 3.16)

project(ContactManager)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(ContactManager
    contact_manager_qt.cpp
)

target_link_libraries(ContactManager PRIVATE Qt6::Widgets)
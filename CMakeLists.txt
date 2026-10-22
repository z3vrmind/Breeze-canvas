cmake_minimum_required(VERSION 3.21)
project(BreezeCanvas VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2)

qt_standard_project_setup()

qt_add_executable(appBreezeCanvas
    main.cpp
    CanvasBackend.h
    CanvasBackend.cpp
)

qt_add_qml_module(appBreezeCanvas
    URI BreezeCanvas
    VERSION 1.0
    QML_FILES
        Main.qml
)

set_target_properties(appBreezeCanvas PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    QT_ANDROID_TARGET_SDK_VERSION 34
    QT_ANDROID_MIN_SDK_VERSION 26
)

target_link_libraries(appBreezeCanvas PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
)

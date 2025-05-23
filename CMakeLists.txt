cmake_minimum_required(VERSION 3.16)

project(seawar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(appseawar
    main.cpp
    GameModel.h
    GameModel.cpp
    media.qrc
)

qt_add_qml_module(appseawar
    URI seawar
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES ShipsLocation.h
    QML_FILES MainMenu.qml
    QML_FILES Explosion.qml
    QML_FILES Splashing.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appseawar PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appseawar
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appseawar
    PRIVATE Qt6::Quick; Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS appseawar
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

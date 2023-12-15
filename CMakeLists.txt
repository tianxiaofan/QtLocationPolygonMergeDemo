cmake_minimum_required(VERSION 3.16)

project(QtLocationPolygonMergeDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Location Positioning)

include(vcpkg/scripts/buildsystems/vcpkg.cmake)
find_package(GEOS CONFIG REQUIRED)
find_package(PROJ CONFIG REQUIRED)

qt_standard_project_setup()
qt6_policy(SET QTP0001 NEW)

qt_add_executable(appuntitled
    main.cpp
    fogview.cpp
    fogview.h
    projconvert.cpp
    projconvert.h
)

qt_add_qml_module(appuntitled
    URI QtLocationPolygonMergeDemo
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES
        fogview.cpp fogview.h
#    IMPORTS qmldir
)

list(APPEND qml_qtcreator_path ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/QtLocationPolygonMergeDemo )

set(QML_IMPORT_PATH
    qml_qtcreator_path
    CACHE STRING "" FORCE)

set(QML2_IMPORT_PATH
    ${qml_qtcreator_path}
    CACHE STRING "" FORCE)

message(----${CMAKE_BINARY_DIR}----)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appuntitled PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appuntitled
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appuntitled
    PRIVATE Qt6::Quick Qt::Location Qt6::Positioning  GEOS::geos PROJ::proj
)

include(GNUInstallDirs)
install(TARGETS appuntitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

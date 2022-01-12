set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_executable(quetzalcoatl
  main.cpp
  mainwindow.cpp
  mainwindow.h
  resources.qrc
)


target_link_libraries(quetzalcoatl PRIVATE controller Qt5::Widgets Qt5::Gui PkgConfig::LIBMPDCLIENT)
target_include_directories(quetzalcoatl PUBLIC "../controller" ${LIBMPDCLIENT_INCLUDE_DIRS})


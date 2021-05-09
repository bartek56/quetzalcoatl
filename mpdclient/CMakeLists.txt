add_library(mpdclient SHARED
  connection.cpp
  playlist.cpp
  song.cpp
  status.cpp
  settings.cpp
)

target_include_directories(mpdclient PRIVATE ${LIBMPDCLIENT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(mpdclient PRIVATE MPDCLIENT_LIBRARY)
target_link_libraries(mpdclient PRIVATE PkgConfig::LIBMPDCLIENT)
set_property(TARGET mpdclient PROPERTY CXX_STANDARD 20)

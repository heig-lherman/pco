
project(labo6_lib)

set(LIBRARY_NAME labo6_lib)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)

add_library(labo6_lib
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(labo6_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(labo6_lib PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test -lpcosynchro)

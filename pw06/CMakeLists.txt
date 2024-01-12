cmake_minimum_required(VERSION 3.0)
project(PCO_lab06)

# Ajoutez les sources à la racine du projet
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Ajoutez ici les sous-dossiers à compiler
add_subdirectory(src)
add_subdirectory(labo6_gui)
add_subdirectory(labo6_tests)

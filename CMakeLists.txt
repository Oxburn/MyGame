cmake_minimum_required(VERSION 3.15)
project(raylib_game LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib 5.5 REQUIRED)                                           # Trouver Raylib 
include_directories(${CMAKE_SOURCE_DIR}/include)                            # Inclure les headers
file(GLOB_RECURSE SOURCES "src/*.cpp")                                      # Récupérer tous les fichiers source
add_executable(${PROJECT_NAME} ${SOURCES})                                  # Créer l'exécutable
target_link_libraries(${PROJECT_NAME} raylib)                               # Lier avec Raylib
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)     # Activer les warnings
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})                           # Copie des assets dans le dossier build
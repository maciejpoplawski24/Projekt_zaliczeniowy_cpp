cmake_minimum_required(VERSION 3.5)
project(TestSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Wskazujemy GŁÓWNY folder SFML (ten, w którym są foldery bin, lib, include)
set(CMAKE_PREFIX_PATH "D:/SFML2/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1")

# Szukamy biblioteki SFML w wersji 2
find_package(SFML 2 REQUIRED COMPONENTS graphics window system)

add_executable(Projektoficjalnie2 GraPajak.cpp TestSFML main.cpp)

# Klasyczne linkowanie, 100% pewności w wersji 2.6.1
target_link_libraries(TestSFML PRIVATE sfml-graphics sfml-window sfml-system)

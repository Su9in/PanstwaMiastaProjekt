# Minimalna wersja CMake
cmake_minimum_required(VERSION 3.10)

# Nazwa projektu
project(Serwer)

# Ustawienie standardu C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Źródła projektu
set(SOURCES
    serwer.cpp
)

# Tworzenie pliku wykonywalnego
add_executable(serwer ${SOURCES})

# Jeśli masz dodatkowe zależności, np. biblioteki, dodaj je poniżej
# target_link_libraries(my_project some_library)
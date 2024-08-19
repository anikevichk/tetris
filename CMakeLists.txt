cmake_minimum_required(VERSION 3.28)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

# Подключение библиотеки GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

# Поиск и подключение библиотеки raylib
find_package(raylib CONFIG REQUIRED)

# Создание исполняемого файла
add_executable(main main.cpp)

# Линковка с библиотекой raylib
target_link_libraries(main PRIVATE raylib)

# Дополнительные настройки для сборки под Emscripten
if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # Эта строка используется для задания расширения исполняемого файла на HTML для сборки под Emscripten.
endif ()

cmake_minimum_required(VERSION 3.10)
project(SvyatPlusPlus VERSION 0.1.0 LANGUAGES CXX)

# Используем современный стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл из исходников
add_executable(svyat++
    src/main.cpp
    src/lexer.cpp
	src/parser.cpp
	src/codegen.cpp
)

# Включаем строгие предупреждения для GCC/Clang (полезно для качества кода)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(svyat++ PRIVATE -Wall -Wextra -Wpedantic)
endif()

cmake_minimum_required(VERSION 3.23)
project(Faxdawn)

set(CMAKE_CXX_STANDARD 20)

include_directories(source)

set(source_files
    "source/faxdawn/faxdawn.cpp"
    "source/faxdawn/syntax.cpp"
    "source/file_io/file_io.cpp"
    "source/lexer/lexer.cpp"
    "source/lexer/token.cpp"
    "source/machine/machine.cpp"
    "source/parser/parser.cpp"
)

add_executable(Faxdawn ${source_files})

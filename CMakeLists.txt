cmake_minimum_required(VERSION 3.23)
project(Faxdawn)

set(CMAKE_CXX_STANDARD 20)

include_directories(source)

aux_source_directory(source/file_io source_files)
aux_source_directory(source/preprocessor source_files)
aux_source_directory(source/logic source_files)
aux_source_directory(source/lexer source_files)
aux_source_directory(source/parser source_files)
aux_source_directory(source/compiler source_files)
aux_source_directory(source/machine source_files)
aux_source_directory(source/faxdawn source_files)

add_executable(Faxdawn source/faxdawn/faxdawn.cpp ${source_files})

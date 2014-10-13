ADD_LIBRARY(configparser Configuration.cpp Tokenizer.cpp)
TARGET_INCLUDE_DIRECTORIES(configparser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

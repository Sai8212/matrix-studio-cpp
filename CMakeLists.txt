cmake_minimum_required(VERSION 3.20)
project(MatrixStudioWebServer LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cpp_httplib)

add_executable(MatrixStudioServer server.cpp)
target_compile_features(MatrixStudioServer PRIVATE cxx_std_17)
target_link_libraries(MatrixStudioServer PRIVATE httplib::httplib)

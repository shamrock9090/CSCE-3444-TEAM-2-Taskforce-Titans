cmake_minimum_required(VERSION 3.20)
project(HomeworkTrackerBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  cpp_httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.18.3
)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(cpp_httplib nlohmann_json)

add_executable(homework_tracker_backend main.cpp)

target_link_libraries(homework_tracker_backend
  PRIVATE httplib::httplib nlohmann_json::nlohmann_json
)

if(WIN32)
  target_link_libraries(homework_tracker_backend PRIVATE ws2_32)
endif()

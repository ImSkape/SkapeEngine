# editor/CMakeLists.txt
add_executable(Editor
    src/main.cpp
)

target_link_libraries(Editor PRIVATE Engine)
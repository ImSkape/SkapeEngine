# engine/CMakeLists.txt
add_library(Engine STATIC
    src/Engine.cpp
    ${CMAKE_SOURCE_DIR}/vendors/glad/src/glad.c
)

target_link_libraries(Engine PUBLIC 
    glfw 
    glm::glm
    )

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendors/glad/include
)

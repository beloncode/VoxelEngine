cmake_minimum_required(VERSION 3.22)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 17)

#if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
#    set(CMAKE_CXX_FLAGS -fsanitize=address)
#endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

include(FindGLEW)
include(FindOpenGL)
include(FindPNG)
include(FindGLEW)

find_package(Threads)
find_package(glfw3 3.3 REQUIRED)

add_executable(VoxelEngine
    src/files/Files.cpp
    src/files/Files.h
    src/files/WorldFiles.cpp
    src/files/WorldFiles.h
    src/graphics/Batch2D.cpp
    src/graphics/Batch2D.h
    src/graphics/LineBatch.cpp
    src/graphics/LineBatch.h
    src/graphics/Mesh.cpp
    src/graphics/Mesh.h
    src/graphics/Shader.cpp
    src/graphics/Shader.h
    src/graphics/Texture.cpp
    src/graphics/Texture.h
    src/graphics/VoxelRenderer.cpp
    src/graphics/VoxelRenderer.h
    src/lighting/Lighting.cpp
    src/lighting/Lighting.h
    src/lighting/Lightmap.cpp
    src/lighting/Lightmap.h
    src/lighting/LightSolver.cpp
    src/lighting/LightSolver.h
    src/loaders/PNGLoading.cpp
    src/loaders/PNGLoading.h
    src/maths/FastNoiseLite.h
    src/objects/Player.cpp
    src/objects/Player.h
    src/physics/Hitbox.cpp
    src/physics/Hitbox.h
    src/physics/PhysicsSolver.cpp
    src/physics/PhysicsSolver.h
    src/voxels/Block.cpp
    src/voxels/Block.h
    src/voxels/Chunk.cpp
    src/voxels/Chunk.h
    src/voxels/Chunks.cpp
    src/voxels/Chunks.h
    src/voxels/ChunksController.cpp
    src/voxels/ChunksController.h
    src/voxels/ChunksLoader.cpp
    src/voxels/ChunksLoader.h
    src/voxels/Voxel.cpp
    src/voxels/Voxel.h
    src/voxels/WorldGenerator.cpp
    src/voxels/WorldGenerator.h
    src/window/Camera.cpp
    src/window/Camera.h
    src/window/Events.cpp
    src/window/Events.h
    src/window/Window.cpp
    src/window/Window.h
    src/Assets.cpp
    src/Assets.h
    src/Declarations.h
    src/VoxelEngine.cpp
    src/WorldRender.h)

target_link_libraries(VoxelEngine GLEW::glew OpenGL::GL stdc++fs Threads::Threads PNG::PNG GLEW::glew glfw)

# Copying resources files for build directory, and so on we can run the engine inside the build folder
# and with a IDE is in use, just lets the IDE auto execute VoxelEngine executable
file(COPY ${CMAKE_HOME_DIRECTORY}/res DESTINATION ${CMAKE_BINARY_DIR})

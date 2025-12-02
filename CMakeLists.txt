cmake_minimum_required(VERSION 3.16)
project(DimuonAnalysis VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find ROOT package
# ROOT must be installed and the environment set up (source thisroot.sh)
find_package(ROOT REQUIRED COMPONENTS
    Core
    RIO
    Tree
    Hist
    Gpad
    Graf
    ROOTDataFrame
    ROOTVecOps
    Physics
)

# Print ROOT info
message(STATUS "Found ROOT version: ${ROOT_VERSION}")
message(STATUS "ROOT include dirs: ${ROOT_INCLUDE_DIRS}")
message(STATUS "ROOT libraries: ${ROOT_LIBRARIES}")

# Include ROOT headers and use ROOT compile definitions
include(${ROOT_USE_FILE})

# Define the executable
add_executable(dimuon_analysis
    src/main.cpp
    src/Analysis.cpp
)

# Include directories
target_include_directories(dimuon_analysis PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

# Link ROOT libraries
target_link_libraries(dimuon_analysis PRIVATE
    ${ROOT_LIBRARIES}
    ROOT::ROOTDataFrame
    ROOT::ROOTVecOps
    ROOT::Physics
)

# Installation rules (optional)
install(TARGETS dimuon_analysis
    RUNTIME DESTINATION bin
)

# Print build summary
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "===========================")
message(STATUS "")

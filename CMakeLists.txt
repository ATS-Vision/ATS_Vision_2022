CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(AIIT-RM-CV)
SET(CMAKE_CXX_STANDARD 11)
set (CMAKE_BUILD_TYPE debug)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPATH=\"\\\"${PROJECT_SOURCE_DIR}\\\"\"")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CMAKE_SYSTEM_NAME}")
SET(YAML_CPP_LIBRARIES libyaml-cpp.so)  # 设置yaml-cpp库
SET(DAHENG_CAMERA_DIR "${PROJECT_SOURCE_DIR}/camera/GxCamera/include")
SET(BIN_NAME "run")


FIND_FILE(CONFIG_FOUND "config.h" "others/include/config")
if (CONFIG_FOUND)
    MESSAGE("Found config.h")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_CONFIG")
endif()

FIND_PROGRAM(CCACHE_FOUND ccache) # ccache编译器缓存
IF(CCACHE_FOUND)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    MESSAGE("< Use ccache for compiler >")
ENDIF()

FIND_PACKAGE(Eigen3 REQUIRED)

set(OpenCV_DIR /home/aiit/env/opencv-4.5.0/build)
FIND_PACKAGE(OpenCV 4 REQUIRED) # 数字根据实际使用opencv版本更改,例如opencv3.X就改为3，opencv4.X就改为4

FIND_PACKAGE(Threads)




LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/others)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/energy/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/armor/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/others/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/camera)
FILE(GLOB_RECURSE sourcefiles "others/src/*.cpp" "energy/src/*cpp" "armor/src/*.cpp" "others/src/camera/*.cpp")

FILE(GLOB_RECURSE daheng "${PROJECT_SOURCE_DIR}/camera/GxCamera/*.cpp")
ADD_EXECUTABLE(${BIN_NAME} main.cpp ${sourcefiles} ${daheng} main.h energy/src/energy/mark.cpp)
add_subdirectory(tools/fmt)
# Add fmtlib include to EXTRA_INCLUDES
list(APPEND EXTRA_INCLUDES tools/fmt/include)

TARGET_LINK_LIBRARIES(${BIN_NAME} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(${BIN_NAME} ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(${BIN_NAME} gxiapi)
TARGET_LINK_LIBRARIES(${BIN_NAME} -lX11)
TARGET_LINK_LIBRARIES(${BIN_NAME} ${YAML_CPP_LIBRARIES})
TARGET_LINK_LIBRARIES(${BIN_NAME} fmt::fmt)
#TARGET_LINK_LIBRARIES(${BIN_NAME} "others/include")



ADD_CUSTOM_TARGET(create-startup COMMAND "${PROJECT_SOURCE_DIR}/tools/create-startup.sh" "${PROJECT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")
ADD_CUSTOM_TARGET(train-cnn COMMAND "gnome-terminal" "--" "bash" "-c" "${PROJECT_SOURCE_DIR}/tools/TrainCNN/backward.py")

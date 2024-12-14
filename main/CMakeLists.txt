#IDF-CMAKE
#添加*.c文件

file(GLOB CORE "core/*.c")              #核心

file(GLOB COMMON "common/*.c")              #公共
file(GLOB DRIVERS "drivers/*.c")    #驱动程序
file(GLOB MOTOR "motor/*.c")    #电机
file(GLOB SENSOR "sensor/*.c")      #传感器
file(GLOB UTILS "utils/*.c")      #工具包

file(GLOB DEMO "demo/*.c")  #测试demo

#添加工程文件
set(PROJECT_SOURCES
        "main.c"

        ${CORE}

        ${COMMON}
        ${DRIVERS}
        ${MOTOR}
        ${SENSOR}
        ${UTILS}

        ${DEMO}
)

#添加包含目录
list(APPEND INC_DIR
      ${CMAKE_CURRENT_SOURCE_DIR}
)

#IDF注册
idf_component_register(
                    SRCS  ${PROJECT_SOURCES}
                    INCLUDE_DIRS ${INC_DIR}
                    )

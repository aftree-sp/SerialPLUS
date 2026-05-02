cmake_minimum_required(VERSION 3.20)
project(SerialPLUS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Widgets SerialPort Charts Network PrintSupport
)

# Optional: Lua scripting
find_package(Lua QUIET)

set(SOURCES
    src/main.cpp

    # Core
    src/core/SerialManager.cpp
    src/core/SerialManager.h
    src/core/DataLogger.cpp
    src/core/DataLogger.h
    src/core/MacroManager.cpp
    src/core/MacroManager.h
    src/core/AutoSender.cpp
    src/core/AutoSender.h
    src/core/ScriptEngine.cpp
    src/core/ScriptEngine.h

    # Protocol
    src/protocol/ProtocolParser.cpp
    src/protocol/ProtocolParser.h
    src/protocol/ModbusParser.cpp
    src/protocol/ModbusParser.h
    src/protocol/CustomFrameParser.cpp
    src/protocol/CustomFrameParser.h

    # UI
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/ui/SerialTab.cpp
    src/ui/SerialTab.h
    src/ui/PortConfigWidget.cpp
    src/ui/PortConfigWidget.h
    src/ui/TerminalWidget.cpp
    src/ui/TerminalWidget.h
    src/ui/MacroPanel.cpp
    src/ui/MacroPanel.h
    src/ui/PlotWidget.cpp
    src/ui/PlotWidget.h
    src/ui/ProtocolPanel.cpp
    src/ui/ProtocolPanel.h
    src/ui/ScriptPanel.cpp
    src/ui/ScriptPanel.h
    src/ui/LogPanel.cpp
    src/ui/LogPanel.h
    src/ui/ThemeManager.cpp
    src/ui/ThemeManager.h

    # Widgets
    src/ui/widgets/MaterialButton.cpp
    src/ui/widgets/MaterialButton.h
    src/ui/widgets/MaterialCard.cpp
    src/ui/widgets/MaterialCard.h
    src/ui/widgets/MaterialTabBar.cpp
    src/ui/widgets/MaterialTabBar.h
    src/ui/widgets/HexEditor.cpp
    src/ui/widgets/HexEditor.h
    src/ui/widgets/StatusBar.cpp
    src/ui/widgets/StatusBar.h
)

set(RESOURCES
    resources/resources.qrc
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${RESOURCES}
    resources/app.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/core
    src/ui
    src/ui/widgets
    src/protocol
    src/scripting
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
    Qt6::Network
    Qt6::PrintSupport
)

if(Lua_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_LUA=1)
endif()

# Windows deployment
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build complete: $<TARGET_FILE:${PROJECT_NAME}>"
    )
endif()

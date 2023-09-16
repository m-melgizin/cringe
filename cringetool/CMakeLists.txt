set(
    CRINGE_TOOL_SOURCES
    argparse/argparse.c
    argparse/argparse.h
    cringe_tool.c
)

include_directories(../cringelib)

add_executable(cringe ${CRINGE_TOOL_SOURCES})
target_link_libraries(cringe cringelib)

set(
    CRINGELIB_SOURCES
    cringe.c
    cringe.h
    kuznyechik.c
    kuznyechik.h
)

add_library(
    cringelib STATIC
    ${CRINGELIB_SOURCES}
)

add_executable(kuznyechik_test kuznyechik_test.c)
target_link_libraries(kuznyechik_test cringelib)

add_executable(cringe_test cringe_test.c)
target_link_libraries(cringe_test cringelib)

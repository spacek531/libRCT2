cmake_minimum_required (VERSION 3.1)
project (RCT2 C)

add_library(RCT2 STATIC
    src/chunk.c
    src/object.c
    src/image.c
    src/ride.c
    src/small_scenery.c
    src/large_scenery.c
    src/wall.c
    src/path_banner.c
    src/path.c
    src/path_object.c
    src/scenery_group.c
    src/park_entrance.c)
set_property(TARGET RCT2 PROPERTY C_STANDARD 99)

add_executable(TestRCT2 src/main.c)
set_property(TARGET TestRCT2 PROPERTY C_STANDARD 99)
target_link_libraries (TestRCT2 RCT2)

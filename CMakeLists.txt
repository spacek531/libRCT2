cmake_minimum_required (VERSION 3.1)
project (RCT2 C)

add_library(RCT2 STATIC chunk.c object.c image.c ride.c small_scenery.c large_scenery.c wall.c path_banner.c path.c path_object.c scenery_group.c park_entrance.c)
set_property(TARGET RCT2 PROPERTY C_STANDARD 99)

add_executable(TestRCT2 main.c)
set_property(TARGET TestRCT2 PROPERTY C_STANDARD 99)
target_link_libraries (TestRCT2 RCT2)

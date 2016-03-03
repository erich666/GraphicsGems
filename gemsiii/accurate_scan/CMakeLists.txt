add_library(accurate_scan dblfixpoint.c fixpoint.c tri.c)
add_executable(accurate_exhaust exhaust.c)
target_link_libraries(accurate_exhaust accurate_scan m)
add_executable(accurate_test test.c)
target_link_libraries(accurate_test accurate_scan m)


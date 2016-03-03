add_library(Peano peano.c)
add_executable(PeanoMain main.c)
target_link_libraries(PeanoMain Peano m)
add_executable(PeanoMapply mapply.c)
target_link_libraries(PeanoMapply Peano)


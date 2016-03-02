add_library(Peano peano.c)
add_executable(PeanoMain main.c)
target_link_libraries(PeanoMain Peano)
add_executable(PeanoMapply mapply.c Peano)
target_link_libraries(PeanoMapply Peano)


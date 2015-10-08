#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {

  // input parameters (basic block size, memory length)

  // init_allocator(basic block size, memory length)
  init_allocator(8, 1000000);
  print_free_nodes();
  ackerman_main();
  
  release_allocator();
}

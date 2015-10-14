#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv)
{

  int returnVal = 0;
  // input parameters (basic block size, memory length)
  int option;
  int block_size = 128;
  int memsize = 512000;
  int n = 1;
  int m = 1;

  atexit(release_allocator);

  //Specifying the expected options
  while ((option = getopt(argc, argv,"b:s:n:m:")) != -1)
  {
      switch (option) {
            case 'b' : block_size = atoi(optarg);
                break;
            case 's' : memsize = atoi(optarg);
                break;
            case 'n' : n = atoi(optarg);
                break;
            case 'm' : m = atoi(optarg);
                break;
            case '?':
                if (optarg == 'b' || optarg == 's' || optarg == 'n' || optarg == 'm')
                  fprintf (stderr, "Option -%c requires an argument.\n", optarg);
                else if (isprint (optarg))
                  fprintf (stderr, "Unknown option `-%c'.\n", optarg);
                else
                  fprintf (stderr, "Unknown option character `\\x%x'.\n",  optarg);
                return 1;
            default:
                abort ();
        }
  }

  printf("Allocator initialized with a block size: %d bytes and memsize: %d bytes\n\n",
        block_size, memsize);
  // init_allocator(basic block size, memory length)
  init_allocator(block_size, memsize);
  print_free_nodes();
  ackerman_main(n, m);

  return returnVal;
}

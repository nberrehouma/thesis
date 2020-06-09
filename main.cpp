#include "systemc.h"

#include "initiator.h"
#include "memory.h"
#include "top.h"
int sc_main(int argc, char* argv[])
{

  top top_module("top");
  sc_start();  // run forever
  return 0;
}



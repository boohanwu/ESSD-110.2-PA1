#include "system.h"

/*****************************************************
Compiling: make part#.out 

e.g.       make part1.out 
           make part2.out 
           make part3_fifo.out 
           make part3_rr.out 

Execute	: ./part#.out <Input file>

e.g.      ./part1.out ./input/part1_Input.txt 
          ./part2.out ./input/part2_Input_10.txt
          ./part2.out ./input/part2_Input_20.txt
          sudo ./part3_fifo.out ./input/part3_Input.txt 
          sudo ./part3_rr.out ./input/part3_Input.txt 
******************************************************/

int main(int argc, char** argv)
{
  System sys( argv[1] );
  sys.creatAnswer();              // Single thread convolution

#if (PART == 1)
  sys.globalMultiCoreConv();      // Global multi-thread convolution
  sys.partitionMultiCoreConv();   // Partition multi-thread convolution

#else
  sys.partitionFirstFit();
  sys.partitionBestFit();
  sys.partitionWorstFit(); 
#endif

  return 0;
}
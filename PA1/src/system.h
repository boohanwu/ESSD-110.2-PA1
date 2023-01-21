#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

#include "config.h"
#include "../libs/check.h"
#include "cpu.h"
#include "thread.h"

typedef void* (*THREADFUNCPTR)(void*);

class System
{
  public:
    /* Constructrue */ /* Part 1 & Part 3 */
    System (char*);                       // Construct a new System:: System object and initial parameter.
    ~System();

    /* Baseline */
    void creatAnswer ();                  // Caculate answer inside the checker.

    /* Part 1 */
    void globalMultiCoreConv ();
    void partitionMultiCoreConv ();

    /* Part 2 */
    void partitionFirstFit ();
    void partitionBestFit ();
    void partitionWorstFit ();

    /* Other Function */
    void cleanMultiResult ();             // Clear the result store in multiResutl
    void loadInput (char*);               // load input parameter
    void setEndTime ();
    void setStartTime ();
    void setUpMatrix (int);               // initial matrix dimension
    double period () { return _timeUse; };

  private:
    int numThread;          // thread number of current system
    CPU* cpuSet;            // list of cpu
    Check* check;
    Thread* threadSet;      // list of thread

    struct timeval start;
    struct timeval end;
    double _timeUse;

  float ***mask;            // convolution mask
  float ***matrix;          // data matirx
  float ***multiResult;     // result matrix


};

#endif

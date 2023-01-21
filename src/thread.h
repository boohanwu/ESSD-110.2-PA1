#ifndef _THREAD_H_
#define _THREAD_H_

#include <cstring>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>

#include <pthread.h>

#include "config.h"
#include "../libs/check.h"

static pthread_mutex_t count_Mutex;
static int current_PID = -1;

class Thread
{
  public:

    /* Constructrue */
    ~Thread();
    void init (float**, float**, float**);

    /* Part 1 *//* Part 3 */
    static void* convolution(void*);      // Perform convolution

    /* Part 1 */
    void setUpCPUAffinityMask (int);      // Pined the thread to core

    /* Part 3 */
    void setUpScheduler ();               // Set up the scheduler for current thread

    /* Other */
    void printThreadInfo ();

    void setCheck (Check*);
    void setCore (int);                   // Set the core where thread to be executed
    void setMatrixSize (int);
    void setEndCalculatePoint (int);      // Convolution start point
    void setStartCalculatePoint (int);    // Convolution end point
    void setSchedulingPolicy (int);       // Choose schedule policy
    void setThreadID (int);

    int ID () { return _ID;}
    int matrixSize () { return _matrixSize;}
    int schedulingPolicy () { return _schedulingPolicy;}
    float utilization () { return _utilization;}


  public:
    pthread_t _thread;

  private:
    int _ID;
    int _matrixSize;
    int _schedulingPolicy = SCHED_OTHER;  // SCHED_OTHER is the default policy
    float _utilization;

    int cur_core;       // Run-time core executing this thread
    int PID;
    int core = -1;      // The core want to assign to

    Check* check;

    int startCalculatePoint;
    int endCalculatePoint;

  float **mask;
	float **matrix;
	float **multiResult;	 
};


#endif

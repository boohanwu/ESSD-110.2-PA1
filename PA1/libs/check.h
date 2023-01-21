/**
 * Here is a check library that you don't need to modify.
 * 
 * All the function referenced in this project are not recommended
 * to change there caller position.
 * 
 * If your modify cause the wrong solution, we won't take the 
 * responsibility for this.
 * 
 */

#ifndef _CHECK_H_
#define _CHECK_H_

#include <cassert>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>

enum CheckState{
    GLOBAL,
    PARTITION,
    PARTITION_FF,
    PARTITION_BF,
    PARTITION_WF,
    PARTITION_RR,
    PARTITION_FIFO
};

class Check{

  public:

    /* Constructure */
    Check(int = 0, int = 0, int = 0, float*** = NULL);
    ~Check();
    void init(int, int, int, float***);

    /* Setup */
    void dataGenerator (int, float***, float***);
    void setThreadWithIndex(int, pthread_t*);
    void setMatrixSizeWithIndex(int, int);
    
    /* Checking */
    void checkCorrectness();
    void creatAnswer(int);
    void setCheckState(CheckState);

  
  private:
    /* Each part checking method */
    void compareResult ();
    void checkPart1Global();
    void checkPart1Partition();
    void checkPart2Partition();
    void checkPart3Scheduler();

    /* Other */
    void singleConvolution (int);

    void setEndTime ();
    void setStartTime ();
    double period () { return _timeUse; };

  private:
    int checkCount;
    int maskSize;
    int numCore;
    int numThread;
    int schedulingPolicy;
    int *matrixSize;
    int *partitionList;  

    bool correctnessResult;
    bool correctnessScheduling;
    bool *cpuList;

    float ***Answer;
    float ***matrix;
    float ***mask;
    float ***multiResult;

    struct timeval start;
    struct timeval end;
    double _timeUse;

    CheckState checkState;
    pthread_t **threadList;

};

#endif
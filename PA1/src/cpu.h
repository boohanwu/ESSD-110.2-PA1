/**
 * @file cpu.h
 * @brief A structure provide for keeping the partition result,
 *        you can dicide wheater to use this structure or defined 
 *        by yourself.
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <cstring>
#include <iostream>

class CPU{

  public:
    /* Constructure */
    CPU ();
    void init(int, int);

    /* Other */
    void emptyCPU ();                 // Clear thread list
    void printCPUInformation ();      // Print the thread list
    void push_thread (int, float);    // Push thread into this core by thread id and utilization

    int cpuId () {return _cpuId;};
    float utilization () { return _utilization; }

  private:
    int _cpuId;
    int threadCount;        // Number of thread pinned in this core
    int* threadList;        // The list of thread id pinned this core
    float _utilization;     // Core total utilization

};

#endif
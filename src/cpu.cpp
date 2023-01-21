#include "cpu.h"

CPU::CPU ()
{
  threadCount = 0;
  _utilization = 0;
}

void
CPU::init(int cpu_id, int thread_num)
{
  _cpuId = cpu_id;
  threadList = new int[thread_num];
}

void
CPU::emptyCPU ()
{
  memset(threadList, 0, sizeof(int) * threadCount);
  threadCount = 0;
  _utilization = 0;

}

void
CPU::printCPUInformation ()
{
  std::cout << "Core Number : " << _cpuId << std::endl;
    std::cout << "[ " ;
	for (int i = 0; i < threadCount; i++)
        std::cout << threadList[i] << ", " ;
    std::cout << "]" << std::endl;
    std::cout << "Total Utilization : " << _utilization << std::endl;
    std::cout << std::endl;
}

void
CPU::push_thread (int tid, float utili)
{
  threadList[threadCount++] = tid;
	_utilization += utili;
}
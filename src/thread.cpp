#include "thread.h"

Thread::~Thread()
{
  for (int i = 0; i < MASK_SIZE; i++)
    delete[] mask[i];
  
  for (int i = 0; i < _matrixSize; i++)
  {
    delete[] matrix[i];
    delete[] multiResult[i];	
  }
  delete[] mask;
  delete[] matrix;
  delete[] multiResult;

  delete check;
}

void
Thread::init (float **multi_result, float **input_matrix, float **conv_mask)
{
  _utilization = float( _matrixSize / float(UTILIZATION_DIVIDER) );

    startCalculatePoint = 0;
    endCalculatePoint = _matrixSize;

    multiResult = multi_result;
    matrix = input_matrix;
    mask = conv_mask;
}

void*
Thread::convolution(void* args)
{
  Thread *obj = (Thread*) args;

#if (PART == 3)
    obj->setUpScheduler();
#endif
  /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  // Set up the affinity mask
  if(obj->core != -1)
    obj->setUpCPUAffinityMask(obj->core);
  obj->cur_core = sched_getcpu();
  obj->PID = syscall(SYS_gettid);

  if(PART != 3){
    pthread_mutex_lock(&count_Mutex);
    obj->printThreadInfo();
    pthread_mutex_unlock(&count_Mutex);
  }
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

  /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  // Edit the function into partial multiplication.
  // Hint : Thread::startCalculatePoint & Thread::endCalculatePoint
  int shift = (MASK_SIZE-1)/2;
  for (int i = obj->startCalculatePoint; i < obj->endCalculatePoint; i++)
  {
    for (int j = 0; j < obj->_matrixSize; j++)
    {
      for (int k = -shift; k <= shift; k++)
      {
        for (int l = -shift; l <= shift; l++)
        {
          if( i + k < 0 ||  i + k >= obj->_matrixSize || j + l < 0 ||  j + l >= obj->_matrixSize)
            continue;
          obj->multiResult[i][j] += obj->matrix[i+k][j+l] * obj->mask[k+shift][l+shift];
        }
      }
      /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
      // Observe the thread migration
      int newCore = sched_getcpu();   // Get index of currently used CPU
      if(obj->cur_core != newCore && PART == 1){
        std::cout << "The thread " << obj->_ID << " PID " << obj->PID << " is moved from CPU " << obj->cur_core << " to " << newCore << std::endl;
        obj->cur_core = newCore;
      }
      /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    }
#if (PART == 3)
    /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
    /* Observe the execute thread on core-0 */
    if(obj->core == 0){
      if(current_PID == -1){
        std::cout << "core-0 start from " << obj->PID << std::endl;
        current_PID = obj->PID;
      }
      else if(current_PID != obj->PID){
        std::cout << "Core-0 context switch from " << current_PID << " to " << obj->PID << std::endl;
        current_PID = obj->PID;
      }
    }
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#endif
  }
  /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

  pthread_mutex_lock(&count_Mutex);
    obj->check->checkCorrectness();
	pthread_mutex_unlock(&count_Mutex);

  return 0;
}

void
Thread::setUpCPUAffinityMask (int core_num)
{
  /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
  // Pined the thread to core.
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core_num, &mask);
  if(sched_setaffinity(0, sizeof(mask), &mask) == -1)
    std::cerr << "Warning: could not set CPU affinity mask" << std::endl;
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}

void
Thread::setUpScheduler()
{
	/*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
  // Set up the scheduler for current thread
  if(schedulingPolicy() == SCHED_FIFO){
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    int ret = sched_setscheduler(0, SCHED_FIFO, &sp);
    if(ret == -1)
      std::cerr << "An error occurs when setting scheduler." << std::endl;
  }
  else if(schedulingPolicy() == SCHED_RR){
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_RR);
    int ret = sched_setscheduler(0, SCHED_RR, &sp);
    if(ret == -1)
      std::cerr << "An error occurs when setting scheduler." << std::endl;
  }
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
}

void
Thread::printThreadInfo()
{
    std::cout << "Thread ID : " << _ID ;
    std::cout << "\tPID : " << PID;
    std::cout << "\tCore : " << cur_core;
#if (PART != 1)
    std::cout << "\tUtilization : " << _utilization;
    std::cout << "\tMatrixSize : " << _matrixSize;	
#endif
    std::cout << std::endl;
}

void
Thread::setCheck (Check* _check)
{
  check = _check;
}

void
Thread::setCore (int _core)
{
  core = _core;
}

void 
Thread::setMatrixSize (int matrixSize)
{
  _matrixSize = matrixSize;
}

void 
Thread::setEndCalculatePoint (int endPoint)
{
  endCalculatePoint = endPoint;
}

void 
Thread::setStartCalculatePoint (int startPoint)
{
  startCalculatePoint = startPoint;
}

void 
Thread::setSchedulingPolicy (int schedulingPolicy)
{
  _schedulingPolicy = schedulingPolicy;
}

void
Thread::setThreadID (int id)
{
  _ID = id;
}

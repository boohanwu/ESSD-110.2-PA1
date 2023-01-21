#include "system.h"

System::System (char* input_file)
{
    loadInput(input_file);

    for (int i = 0; i < numThread; i++)
    {
#if (PART == 1)
        /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
        // For part1, we assign the matrix0 into all threads
        threadSet[i].init(multiResult[0], matrix[0], mask[0]);
        // Set up the caculation range of each thread matrix
        threadSet[i].setStartCalculatePoint(threadSet[i].matrixSize() / numThread * i);
        threadSet[i].setEndCalculatePoint(threadSet[i].matrixSize() / numThread * (i+1));
        /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#else
        threadSet[i].init(multiResult[i], matrix[i], mask[i]);
            
#endif

#if (PART == 3)
	    /*~~~~~~~~~~~~Your code(PART3)~~~~~~~~~~~*/
        // Set the scheduling policy for thread.
        if(SCHEDULING == SCHED_FIFO)
            threadSet[i].setSchedulingPolicy(SCHED_FIFO);
        else if(SCHEDULING == SCHED_RR)
            threadSet[i].setSchedulingPolicy(SCHED_RR);
	    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
#endif
    }

    /* CPU list Initialize */
    cpuSet = new CPU[CORE_NUM];
    for (int i = 0; i < CORE_NUM; i++)
        cpuSet[i].init (i, numThread);
    
    /* Set up checker for checking correctness */
    check = new Check;
    check->init(CORE_NUM, MASK_SIZE, numThread, multiResult);
    std::cout << "\n===========Generate Matrix Data===========" << std::endl;
    setStartTime();
    for (int i = 0; i < numThread; i++)
    {
        check->setThreadWithIndex (i, &threadSet[i]._thread);
        check->setMatrixSizeWithIndex( i, threadSet[i].matrixSize() );
        check->dataGenerator(i, matrix, mask);
        threadSet[i].setCheck( check );
    }
    setEndTime();
    std::cout << "Generate Date Spend time : " << _timeUse << std::endl;
    
#if (PART == 3)
    if (SCHEDULING == SCHED_FIFO) {
        check->setCheckState(PARTITION_FIFO);
    } else if (SCHEDULING == SCHED_RR) {
        check->setCheckState(PARTITION_RR);
    } else {
        std::cout << "!! Not supported scheduler !!" << std::endl;
        assert(false);
    }
#endif

}

System::~System()
{
    for (int thread_id = 0; thread_id < numThread; thread_id++)
    {
        for (int i = 0; i < MASK_SIZE; i++)
        {
            delete[] mask[thread_id][i];
        }
        delete[] mask[thread_id];
        for (int i = 0; i < threadSet[thread_id].matrixSize(); i++)
        {
            delete[] matrix[thread_id][i];
            delete[] multiResult[thread_id][i];
        }
        delete[] matrix[thread_id];
        delete[] multiResult[thread_id];
    }
    delete[] mask;
    delete[] matrix;
    delete[] multiResult;

    delete cpuSet;
}

void
System::creatAnswer ()
{
    check->creatAnswer(PART);
}

/**
 * Using the pthread function to perform PA exection.
 * 
 * pthread_create( pthread_t* thread, NULL, void* function (void*), void* args );
 * 
 * pthread_join( pthread_t* thread, NULL );
 * 
 */
void
System::globalMultiCoreConv ()
{
    std::cout << "\n===========Start Global Multi-Thread Convolution===========" << std::endl;
    check->setCheckState(GLOBAL);
    setStartTime();

    /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Create thread and join
    for(int i = 0; i < numThread; i++){
        pthread_create(&threadSet[i]._thread, NULL, threadSet[i].convolution, &threadSet[i]);
    }
    for(int i = 0; i < numThread; i++){
        pthread_join(threadSet[i]._thread, NULL);
    }
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    setEndTime();
    std::cout << "Global Multi Thread Spend time : " << _timeUse << std::endl;
    cleanMultiResult();
}

void
System::partitionMultiCoreConv ()
{
#if (PART == 1)
    std::cout << "\n===========Start Partition Multi-Thread Convolution===========" << std::endl;
    check->setCheckState(PARTITION);
#endif
    setStartTime();

    /*~~~~~~~~~~~~Your code(PART1)~~~~~~~~~~~*/
    // Create thread and join
    for(int i = 0; i < numThread; i++){
        if(PART == 1) threadSet[i].setCore(i);
        pthread_create(&threadSet[i]._thread, NULL, threadSet[i].convolution, &threadSet[i]);
    }
    for(int i = 0; i < numThread; i++){
        pthread_join(threadSet[i]._thread, NULL);
    }
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    
    setEndTime();
    std::cout << "Partition Multi Thread Spend time : " << _timeUse << std::endl;
    cleanMultiResult();
}

void
System::partitionFirstFit ()
{
    std::cout << "\n===========Partition First-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_FF);
#endif

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement parititon first-fit and print result.
    for(int i = 0; i < CORE_NUM; i++)
        cpuSet[i].emptyCPU();
    
    for(int i = 0; i < numThread; i++){
        bool isSchedulable = false;
        for(int j = 0; j < CORE_NUM; j++){
            if(cpuSet[j].utilization() + threadSet[i].utilization() <= 1){
                cpuSet[j].push_thread(threadSet[i].ID(), threadSet[i].utilization());
                threadSet[i].setCore(j);
                isSchedulable = true;
                break;
            }
        }
        if(!isSchedulable)
            std::cout << "Thread-" << i << " is not schedulable." << std::endl;
    }

    pthread_mutex_lock(&count_Mutex);
    for(int i = 0; i < CORE_NUM; i++){
        cpuSet[i].printCPUInformation();
    }
    pthread_mutex_unlock(&count_Mutex);
    /*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    partitionMultiCoreConv();
    cleanMultiResult();
}
    
void
System::partitionBestFit ()
{
    std::cout << "\n===========Partition Best-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_BF);
#endif

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement partition best-fit and print result.
    for(int i = 0; i < CORE_NUM; i++)
        cpuSet[i].emptyCPU();

    for(int i = 0; i < numThread; i++){
        float maxUtili = 0;
        int maxIdx = -1;
        for(int j = 0; j < CORE_NUM; j++){
            float totalUtili = cpuSet[j].utilization() + threadSet[i].utilization();
            if((totalUtili <= 1) && (totalUtili > maxUtili)){
                maxUtili = totalUtili;
                maxIdx = j; 
            }
        }
        if( maxIdx != -1){
            cpuSet[maxIdx].push_thread(threadSet[i].ID(), threadSet[i].utilization());
            threadSet[i].setCore(maxIdx);
        }
        else{
            std::cout << "Thread-" << i << " not schedulable." << std::endl;
        }
    }

    pthread_mutex_lock(&count_Mutex);
    for(int i = 0; i < CORE_NUM; i++){
        cpuSet[i].printCPUInformation();
    }
    pthread_mutex_unlock(&count_Mutex);
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/

    partitionMultiCoreConv();
    cleanMultiResult();
}

void
System::partitionWorstFit ()
{
    std::cout << "\n===========Partition Worst-Fit Multi Thread Matrix Multiplication===========" << std::endl;
#if (PART == 2)
    check->setCheckState(PARTITION_WF);
#endif

    /*~~~~~~~~~~~~Your code(PART2)~~~~~~~~~~~*/
    // Implement partition worst-fit and print result.
    for(int i = 0; i < CORE_NUM; i++)
        cpuSet[i].emptyCPU();

    for(int i = 0; i < numThread; i++){
        float minUtili = 1;
        int minIdx = -1;
        for(int j = 0; j < CORE_NUM; j++){
            float totalUtili = cpuSet[j].utilization() + threadSet[i].utilization();
            if((totalUtili <= 1) && (totalUtili < minUtili)){
                minUtili = totalUtili;
                minIdx = j; 
            }
        }
        if( minIdx != -1){
            cpuSet[minIdx].push_thread(threadSet[i].ID(), threadSet[i].utilization());
            threadSet[i].setCore(minIdx);
        }
        else{
            std::cout << "Thread-" << i << " not schedulable." << std::endl;
        }
    }

    pthread_mutex_lock(&count_Mutex);
    for(int i = 0; i < CORE_NUM; i++){
        cpuSet[i].printCPUInformation();
    }
    pthread_mutex_unlock(&count_Mutex);
	/*~~~~~~~~~~~~~~~~~~END~~~~~~~~~~~~~~~~~~*/
    
    partitionMultiCoreConv();
    cleanMultiResult();
}

void
System::cleanMultiResult ()
{
    for (int thread_id = 0; thread_id < numThread; thread_id++)
    {
        int matrix_size = threadSet[thread_id].matrixSize();
        for (int i = 0; i < matrix_size; i++)
            memset(multiResult[thread_id][i], 0, sizeof(float)*matrix_size);
    }  
}

void 
System::loadInput (char* input_file)
{
    std::ifstream infile(input_file);
    std::string line;

    int read_matrix_size = 0;
    float total_matrix_size = 0;

    if(infile.is_open()) {
        getline(infile, line);
        numThread = atoi(line.c_str());
    
            std::cout << "Input File Name : " << input_file << std::endl;
            std::cout << "numThread : " << numThread << std::endl;

        threadSet = new Thread[numThread];
            multiResult = new float**[numThread];
            matrix = new float**[numThread];
            mask = new float**[numThread];
        
        for (int i = 0; i < numThread; i++)
        {
            getline(infile, line);
            read_matrix_size = atoi(line.c_str());
                std::cout << i << ".Matrix size : " << read_matrix_size << std::endl;
            total_matrix_size += read_matrix_size;
            threadSet[i].setThreadID(i);
            threadSet[i].setMatrixSize(read_matrix_size);
                setUpMatrix(i);

        }
        std::cout << "Workload Utilization : " << total_matrix_size / UTILIZATION_DIVIDER << std::endl;
        
        infile.close();
    } else {
        std::cout << "!! Input file not found !!" << std::endl;
        assert(false);
    }
}

void
System::setEndTime ()
{
	gettimeofday(&end, NULL);
	_timeUse = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000.0;
}

void
System::setStartTime ()
{
	gettimeofday(&start, NULL);
}

void
System::setUpMatrix (int thread_id)
{
    int matrix_size = threadSet[thread_id].matrixSize();

    multiResult[thread_id] = new float*[matrix_size];
    for (int i = 0; i < matrix_size; i++)
        multiResult[thread_id][i] = new float[matrix_size];

    matrix[thread_id] = new float*[matrix_size];
    for (int i = 0; i < matrix_size; i++)
        matrix[thread_id][i] = new float[matrix_size];

    if((MASK_SIZE % 2) != 0)
    {
        mask[thread_id] = new float*[MASK_SIZE];
        for (int i = 0; i < MASK_SIZE; i++)
            mask[thread_id][i] = new float[MASK_SIZE];
    } else {
        std::cout << "!! Mask size not odd number !!" << std::endl;
        assert(false);
    }
    
}



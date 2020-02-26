#include "processManagement_lab.h"

/**
 * The task function to simulate "work" for each worker process
 * TODO#3: Modify the function to be multiprocess-safe 
 * */
void task(long duration)
{
    // simulate computation for x number of seconds
    usleep(duration * TIME_MULTIPLIER);

    // TODO: protect the access of shared variable below
    // update global variables to simulate statistics

    // sem_global_data initially = 1
    // wait, decrease to 0
    // execute code
    // then post, increase to 1
    // printf("CHILD: wait semaphore\n"); // debug
    sem_wait(sem_global_data);
    // printf("CHILD: got semaphore\n"); // debug

    ShmPTR_global_data->sum_work += duration;
    ShmPTR_global_data->total_tasks++;
    if (duration % 2 == 1)
    {
        ShmPTR_global_data->odd++;
    }
    if (duration < ShmPTR_global_data->min)
    {
        ShmPTR_global_data->min = duration;
    }
    if (duration > ShmPTR_global_data->max)
    {
        ShmPTR_global_data->max = duration;
    }

    sem_post(sem_global_data);
    // printf("CHILD: returned semaphore\n"); // debug
}

/**
 * The function that is executed by each worker process to execute any available job given by the main process
 * */
void job_dispatch(int i)
{

    // TODO#3:  a. Always check the corresponding shmPTR_jobs_buffer[i] for new  jobs from the main process
    //          d. Loop back to check for new job
    while (true)
    {
        //          b. Use semaphore so that you don't busy wait
        printf("CHILD: process %i wait semaphore in dispatch\n", i); // debug
        sem_wait(sem_jobs_buffer[i]);
        printf("CHILD: process %i got semaphore in dispatch\n", i); // debug
        //          c. If there's new job, execute the job accordingly: either by calling task(), usleep, exit(3) or kill(getpid(), SIGKILL)
        if (shmPTR_jobs_buffer[i].task_status != -1)
        {
            printf("CHILD: worker %i about to execute %c%i\n", i, shmPTR_jobs_buffer[i].task_type, shmPTR_jobs_buffer[i].task_duration); // debug
            switch (shmPTR_jobs_buffer[i].task_type)
            {
            case 't':
                printf("CHILD: t\n"); // debug
                task(shmPTR_jobs_buffer[i].task_duration);
                shmPTR_jobs_buffer[i].task_status = 0;
                printf("CHILD: worker %i completed %c\n", i, shmPTR_jobs_buffer[i].task_type); // debug
                break;
            case 'w':
                printf("CHILD: w\n"); // debug
                usleep(shmPTR_jobs_buffer[i].task_duration * TIME_MULTIPLIER);
                shmPTR_jobs_buffer[i].task_status = 0;
                printf("CHILD: process %i's task status is %i\n", i, shmPTR_jobs_buffer[i].task_status); // debug
                printf("CHILD: worker %i completed %c\n", i, shmPTR_jobs_buffer[i].task_type);           // debug
                break;
            case 'z':
                printf("CHILD: z\n"); // debug
                exit(3);
                shmPTR_jobs_buffer[i].task_status = 0;
                printf("CHILD: process %i's task status is %i\n", i, shmPTR_jobs_buffer[i].task_status); // debug
                printf("CHILD: worker %i completed %c\n", i, shmPTR_jobs_buffer[i].task_type);           // debug
                break;
            case 'i':
                printf("CHILD: i\n"); // debug
                shmPTR_jobs_buffer[i].task_status = 0;
                printf("CHILD: about to kill id %i\n", getpid());                                        // debug
                printf("CHILD: process %i's task status is %i\n", i, shmPTR_jobs_buffer[i].task_status); // debug
                kill(getpid(), SIGKILL);
                printf("CHILD: worker %i completed %c\n", i, shmPTR_jobs_buffer[i].task_type); // debug
                break;
            }
            printf("CHILD: finish switch case\n"); // debug
        }
    }

    printf("Hello from child %d with pid %d and parent id %d\n", i, getpid(), getppid()); // debug
    // exit(0);
}

/** 
 * Setup function to create shared mems and semaphores
 * **/
void setup()
{

    // TODO#1:  a. Create shared memory for global_data struct (see processManagement_lab.h)
    //          b. When shared memory is successfully created, set the initial values of "max" and "min" of the global_data struct in the shared memory accordingly
    // To bring you up to speed, (a) and (b) are given to you already. Please study how it works.

    //          c. Create semaphore of value 1 which purpose is to protect this global_data struct in shared memory
    //          d. Create shared memory for number_of_processes job struct (see processManagement_lab.h)
    //          e. When shared memory is successfully created, setup the content of the structs (see handout)
    //          f. Create number_of_processes semaphores of value 0 each to protect each job struct in the shared memory. Store the returned pointer by sem_open in sem_jobs_buffer[i]
    //          g. Return to main

    ShmID_global_data = shmget(IPC_PRIVATE, sizeof(global_data), IPC_CREAT | 0666);
    if (ShmID_global_data == -1)
    {
        printf("%i\n", ShmID_global_data);                     // debug // debug
        printf("Global data shared memory creation failed\n"); // debug
        exit(EXIT_FAILURE);
    }
    ShmPTR_global_data = (global_data *)shmat(ShmID_global_data, NULL, 0);
    if ((int)ShmPTR_global_data == -1)
    {
        printf("Attachment of global data shared memory failed \n"); // debug
        exit(EXIT_FAILURE);
    }

    //set global data min and max
    ShmPTR_global_data->max = -1;
    ShmPTR_global_data->min = INT_MAX;

    //          c. Create semaphore of value 1 which purpose is to protect this global_data struct in shared memory

    // create and assign semaphore "semglobaldata" with initial value 1
    sem_global_data = sem_open("semglobaldata", O_CREAT | O_EXCL, 0644, 1);
    // check if failed, if does, recreate: unlink, open
    while (true)
    {
        if (sem_global_data == SEM_FAILED)
        {
            // unlink, might have name clash
            sem_unlink("semglobaldata");

            // create again
            sem_global_data = sem_open("semglobaldata", O_CREAT | O_EXCL, 0644, 1);
        }
        else
            break;
    }

    //          d. Create shared memory for number_of_processes job struct (see processManagement_lab.h)
    //          e. When shared memory is successfully created, setup the content of the structs (see handout)
    ShmID_jobs = shmget(IPC_PRIVATE, sizeof(job) * number_of_processes, IPC_CREAT | 0666);
    if (ShmID_jobs == -1)
    {
        printf("Global data shared memory creation failed\n"); // debug
        exit(EXIT_FAILURE);
    }
    shmPTR_jobs_buffer = (job *)shmat(ShmID_jobs, NULL, 0);
    if ((int)shmPTR_jobs_buffer == -1)
    {
        printf("Attachment of global data shared memory failed \n"); // debug
        exit(EXIT_FAILURE);
    }

    //          f. Create number_of_processes semaphores of value 0 each to protect each job struct in the shared memory. Store the returned pointer by sem_open in sem_jobs_buffer[i]
    for (int i = 0; i < number_of_processes; i++)
    {
        char *semjob_name = malloc(sizeof(char) * 16);
        sprintf(semjob_name, "semjobs%d", i);

        // create and assign semaphore "sem_jobs_buffer[i]" with initial value 0
        sem_jobs_buffer[i] = sem_open(semjob_name, O_CREAT | O_EXCL, 0644, 0);
        // check if failed, if does, recreate: unlink, open
        while (true)
        {
            if (sem_jobs_buffer[i] == SEM_FAILED)
            {
                // unlink, might have name clash
                sem_unlink(semjob_name);

                // create again
                sem_jobs_buffer[i] = sem_open(semjob_name, O_CREAT | O_EXCL, 0644, 0);
            }
            else
                break;
        }
    }

    return;
}

/**
 * Function to spawn all required children processes
 **/

void createchildren()
{
    pid_t pid;
    // TODO#2:  a. Create number_of_processes children processes
    for (int i = 0; i < number_of_processes; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Fork failed");
            return;
        }
        else if (pid == 0)
        {
            //          c. For child process, invoke the method job_dispatch(i)
            job_dispatch(i);
        }
        else
        {
            //          b. Store the pid_t of children i at children_processes[i]
            children_processes[i] = pid;
        }
    }
    {
        /* code */
    }

    //          d. For the parent process, continue creating the next children
    //          e. After number_of_processes children are created, return to main

    return;
}

/**
 * The function where the main process loops and busy wait to dispatch job in available slots
 * */
void main_loop(char *fileName)
{

    // load jobs and add them to the shared memory
    FILE *opened_file = fopen(fileName, "r");
    char action; //stores whether its a 'p' or 'w'
    long num;    //stores the argument of the job
    bool task_assigned;

    while (fscanf(opened_file, "%c %ld\n", &action, &num) == 2)
    { //while the file still has input
        task_assigned = false;

        while (task_assigned == false)
        {
            //TODO#4: create job, busy wait
            for (int i = 0; i < number_of_processes; i++)
            {
                int status;
                int alive = waitpid(children_processes[i], &status, WNOHANG);
                printf("process %i is alive %i\n", i, alive);                                                                                 // debug
                printf("waiting for process %i to complete %c%i\n", i, shmPTR_jobs_buffer[i].task_type, shmPTR_jobs_buffer[i].task_duration); // debug
                printf("process %i's task status is %i\n", i, shmPTR_jobs_buffer[i].task_status);                                             // debug
                printf("waiting for process %i to change task status to %i\n", i, 0);                                                         // debug

                if (WIFEXITED(status))
                {
                    int es = WEXITSTATUS(status);
                    printf("exit status was %i\n", es); // debug
                }

                // while (shmPTR_jobs_buffer[i].task_status != 0)
                // {
                //     alive = waitpid(children_processes[i], NULL, WNOHANG);
                printf("process %i is alive %i\n", i, alive);                         // debug
                printf("waiting for process %i to change task status to %i\n", i, 0); // debug
                // };
                printf("process %i's task status is %i\n", i, shmPTR_jobs_buffer[i].task_status); // debug
                //      a. Busy wait and examine each shmPTR_jobs_buffer[i] for jobs that are done by checking that shmPTR_jobs_buffer[i].task_status == 0. You also need to ensure that the process i IS alive using waitpid(children_processes[i], NULL, WNOHANG). This WNOHANG option will not cause main process to block when the child is still alive. waitpid will return 0 if the child is still alive.
                //      b. If both conditions in (a) is satisfied update the contents of shmPTR_jobs_buffer[i], and increase the semaphore using sem_post(sem_jobs_buffer[i])
                if (shmPTR_jobs_buffer[i].task_status == 0 && alive == 0)
                {
                    shmPTR_jobs_buffer[i].task_type = action;
                    shmPTR_jobs_buffer[i].task_duration = num;
                    shmPTR_jobs_buffer[i].task_status = 1;
                    task_assigned = true;
                    sem_post(sem_jobs_buffer[i]);
                    printf("process %i receive new job %c%i\n", i, shmPTR_jobs_buffer[i].task_type, shmPTR_jobs_buffer[i].task_duration); // debug
                    //      c. Break of busy wait loop, advance to the next task on file
                    break;
                }
                //      d. Otherwise if process i is prematurely terminated, revive it. You are free to design any mechanism you want. The easiest way is to always spawn a new process using fork(), direct the children to job_dispatch(i) function. Then, update the shmPTR_jobs_buffer[i] for this process. Afterwards, don't forget to do sem_post as well
                // else if (shmPTR_jobs_buffer[i].task_status == 0 && alive != 0)
                else if (alive != 0)
                {
                    printf("process %i is dead\n", i); // debug
                    pid_t pid = fork();
                    if (pid < 0)
                    {
                        fprintf(stderr, "Fork failed");
                        return;
                    }
                    else if (pid == 0)
                    {
                        //          c. For child process, invoke the method job_dispatch(i)
                        printf("spawn new child\n"); // debug
                        job_dispatch(i);
                    }
                    else
                    {
                        //          b. Store the pid_t of children i at children_processes[i]
                        children_processes[i] = pid;
                    }
                }
            }
        }

        //      e. The outermost while loop will keep doing this until there's no more content in the input file.
    }
    fclose(opened_file);

    printf("Main process is going to send termination signals\n"); // debug

    // TODO#4: Design a way to send termination jobs to ALL worker that are currently alive
    // sleep(3);
    int dead_count = 0;
    while (true)
    {
        for (int i = 0; i < number_of_processes; i++)
        {

            int alive = waitpid(children_processes[i], NULL, WNOHANG);
            // printf("process %i is alive %i\n", i, alive); // debug
            if (shmPTR_jobs_buffer[i].task_status == 0 && alive == 0)
            {
                shmPTR_jobs_buffer[i].task_type = 'z';
                shmPTR_jobs_buffer[i].task_duration = 0;
                shmPTR_jobs_buffer[i].task_status = 1;
                sem_post(sem_jobs_buffer[i]);
                printf("send z to %i\n", i); // debug
                dead_count++;
            }
            else
            {
                dead_count++;
                printf("%i\n", dead_count);          // debug
                printf("%i\n", number_of_processes); // debug
            }
        }
        if (dead_count == number_of_processes)
        {
            printf("break\n"); // debug
            break;
        }
    }

    //wait for all children processes to properly execute the 'z' termination jobs
    printf("waiting for children to terminate\n"); // debug
    int process_waited_final = 0;
    pid_t wpid;
    while ((wpid = wait(NULL)) > 0)
    {
        process_waited_final++;
        printf("1 child terminated\n"); // debug
    }
    printf("process_waited_final is %i\n", process_waited_final); // debug

    // wait for all N children processes
    int waitpid_result;
    for (int i = 0; i < number_of_processes; i++)
    {
        waitpid_result = waitpid(children_processes[i], NULL, 0); // returns when child exits normally
        printf("waitpid_result is %i\n", waitpid_result);         // debug
        if (waitpid_result != -1)
        {
            printf("Child %d with pid %d has exited successfully\n", i, waitpid_result); // debug
        }
    }

    // print final results
    printf("Final results: sum -- %ld, odd -- %ld, min -- %ld, max -- %ld, total task -- %ld\n", ShmPTR_global_data->sum_work, ShmPTR_global_data->odd, ShmPTR_global_data->min, ShmPTR_global_data->max, ShmPTR_global_data->total_tasks);
}

void cleanup()
{
    //TODO#4:
    // 1. Detach both shared memory (global_data and jobs)
    // 2. Delete both shared memory (global_data and jobs)
    int detach_status = shmdt((void *)ShmPTR_global_data); //detach
    if (detach_status == -1)
        printf("Detach shared memory global_data ERROR\n");
    int remove_status = shmctl(ShmID_global_data, IPC_RMID, NULL); //delete
    if (remove_status == -1)
        printf("Remove shared memory global_data ERROR\n");
    detach_status = shmdt((void *)shmPTR_jobs_buffer); //detach
    if (detach_status == -1)
        printf("Detach shared memory jobs ERROR\n");
    remove_status = shmctl(ShmID_jobs, IPC_RMID, NULL); //delete
    if (remove_status == -1)
        printf("Remove shared memory jobs ERROR\n");

    // 3. Unlink all semaphores in sem_jobs_buffer
    //detach and remove shared memory locations
    //unlink all semaphores before exiting process
    int sem_close_status = sem_unlink("semglobaldata");
    for (int i = 0; i < number_of_processes; i++)
    {
        char *sem_name = malloc(sizeof(char) * 16);
        sprintf(sem_name, "semjobs%d", i);
        sem_close_status = sem_unlink(sem_name);
        free(sem_name);
    }
}

// Real main
int main(int argc, char *argv[])
{

    printf("Lab 1 Starts...\n"); // debug

    struct timeval start, end;
    long secs_used, micros_used;

    //start timer
    gettimeofday(&start, NULL);

    //Check and parse command line options to be in the right format
    if (argc < 2)
    {
        printf("Usage: sum <infile> <numprocs>\n");
        exit(EXIT_FAILURE);
    }

    //Limit number_of_processes into 10.
    //If there's no third argument, set the default number_of_processes into 1.
    if (argc < 3)
    {
        number_of_processes = 1;
    }
    else
    {
        if (atoi(argv[2]) < MAX_PROCESS)
            number_of_processes = atoi(argv[2]);
        else
            number_of_processes = MAX_PROCESS;
    }

    setup();
    createchildren();

    for (int i = 0; i < number_of_processes; i++)
    {
        printf("Child process %d created with pid: %d \n", i, children_processes[i]); // debug
        // wait(NULL);
    }

    main_loop(argv[1]);

    //parent cleanup
    cleanup();

    //stop timer
    gettimeofday(&end, NULL);

    double start_usec = (double)start.tv_sec * 1000000 + (double)start.tv_usec;
    double end_usec = (double)end.tv_sec * 1000000 + (double)end.tv_usec;

    printf("Your computation has used: %lf secs \n", (end_usec - start_usec) / (double)1000000);

    return (EXIT_SUCCESS);
}

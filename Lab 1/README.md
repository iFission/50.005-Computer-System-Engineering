# 50005Lab1

assign new jobs:
initialise new boolean variable called task_assigned.

when fscanf reads new task, set task_assigned to false
while (task_assigned == false):
    loop through all processes
        check that they are alive
        if they are both alive, and have completed their task
            assign new task
            set task_assigned to true
            increase the associated semaphore so that the child process can do the task
            break from the for loop
        if it's not alive
            fork new

            (then should go back to looping through all processes until the task is assigned)


after all tasks have been assigned, wait 3 seconds to ensure all jobs have been completed
send termination jobs to all works that are currently alive by:
loop through all processes
    check that they are alive (this will exclude those processes killed by illegal tasks)
    if they are both alive, and have completed their task
        assign task z
        increase the associated semaphore so that the child process can do the task (terminate itself)

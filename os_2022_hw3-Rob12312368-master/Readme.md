Requirement (1/5)  
1. Tasks & task manager  
○ Use ucontext and the related APIs to create tasks  
○ Each task runs a function defined in ‘function.c’, where all the functions are provided by TA and
should not be modified  
○ Implement a task manager to manage tasks, including their state, data structures and so on  
○ Implement task state-related APIs that can be used by the tasks (described in slide 10-11)  
i. void task_sleep(int msec_10);  
ii. void task_exit();  
  
Requirement (2/5)  
2. Task scheduler  
○ Use ucontext and the related APIs to do context switch  
○ Implement three scheduling algorithms  
■ FCFS  
■ RR with time quantum = 30 (ms)  
■ priority-based preemptive scheduling, smallest integer → highest priority  
○ The algorithm is determined at execution: ./scheduler_simulator {algorithm}  
■ algorithm = FCFS / RR / PP  
○ Once the scheduler dispatches CPU to a task, print a message in the format:  
Task {task_name} is running.  
○ If there are no tasks to be scheduled, but there are still tasks waiting, print a message in the format:  
CPU idle.  
  
Requirement (3/5)  
3. Resource handler  
○ Implement resource-related APIs that can be used by the task (described in slide 12-13)  
i. void get_resource(int count, int *resource_list);  
ii. void release_resource(int count, int *resource_list);  
○ There should be 8 resources with id 0-7 in the simulation.  
○ How to simulate resources is up to your design. For example, you can use a boolean array,
resource_available = { true, false, true, .... }  
  
Requirement (4/5)  
4. Timer & signal handler  
○ Use related system calls to set a timer that should send a signal (SIGVTALRM) every 10 ms  
○ The signal handler should do the followings:  
i. Calculate all task-related time (granularity: 10ms)  
ii. Check if any tasks' state needs to be switched  
iii. Decide whether re-scheduling is needed  
  
Requirement (5/5)  
5. Command line interface  
○ Use HW1’s shell as the simulator’s CLI (HW1’s code is provided by TA, you can also use your own
code)  
○ Should support four more commands (details are described in slide 14-17)  
i. add: Add a new task  
ii. del: Delete a existing task  
iii. ps: Show the information of all tasks, including TID, task name, task state, running time, waiting  
time, turnaround time, resources occupied and priority (if any)  
iv. start: Start or resume simulation  
○ Ctrl + z should pause the simulation and switch to shell mode  
○ Timer should stop in the shell mode and resume when the simulation resumes  
○ When the simulation is over, switch back to shell mode after printing a message in the format:
Simulation over.  

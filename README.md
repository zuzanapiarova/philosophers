# PHILOSOPHERS
The Dining philosophers problem. Introducing threads and sharing resources.
42Prague project
Mandatory part: philo/ - each philosopher is a thread, forks are placed between philosophers represented by mutexes
Bonus part: philo_bonus/ - each philosopher is a process, forks are placed in the middle of the table represented by a semaphore

## PROBLEM DESCRIPTION
- One or more philosophers sit at a round table. There is a large bowl of spaghetti in the middle of the table.
- The philosophers alternatively eat, think, or sleep. While they are eating, they are not thinking nor sleeping; while thinking, they are not eating nor sleeping; and, of course, while sleeping, they are not eating nor thinking.
- There are also forks on the table. There are as many forks as philosophers.
- Because serving and eating spaghetti with only one fork is very inconvenient, a
philosopher takes their right and their left forks to eat, one in each hand.
- When a philosopher has finished eating, they put their forks back on the table and start sleeping. Once awake, they start thinking again. The simulation stops when a philosopher dies of starvation.
- Every philosopher needs to eat and should never starve.
- Philosophers don’t speak with each other.
- Philosophers don’t know if another philosopher is about to die.
- No need to say that philosophers should avoid dying

We have to come up with an algorithm that keeps them  when the time received as arguments allows it, preventing deadlocks and data races. 

### ALLOWED FUNCTIONS
Mandatory: memset, printf, malloc, free, write, usleep, gettimeofday, pthread_create, pthread_detach, pthread_join, pthread_mutex_init, pthread_mutex_destroy, pthread_mutex_lock, pthread_mutex_unlock
Bonus: memset, printf, malloc, free, write, fork, kill, exit, pthread_create, pthread_detach, pthread_join, usleep, gettimeofday, waitpid, sem_open, sem_close, sem_post, sem_wait, sem_unlink

## 1. COMPILATION
Compiling Mandatory Part: make -C philo/
Compiling Bonus Part: make -C philo_bonus/

## 2. EXECUTION
Program expects 4 or 5 arguments. Run ./philo(_bonus)/philo(_bonus) [1] [2] [3] [4] (5): 
    [1] number_of_philosophers
    [2] time_to_die (ms)
    [3] time_to_eat (ms)
    [4] time_to_sleep (ms)
    (5) (optional) number_of_times_each_philosopher_must_eat (ms)

### my approach
- MAIN IDEA was to only pass one philosopher structure and its information to each thread/process, so the others cannot access the data of other philosophers in any way
- working in microseconds for accuracy and logging in miliseconds
- not waiting for all to start at the same time 
- TODO?: including timezone by macro, do it by the function !
- TODO?: mutex that protects philo from not eating and dying at the same time 
- stopping the simulation in threads: stop simulation int pointer shared among all:
a. if philo is full, he increments it and when it reaches number of philosophers, monitor stops the simulation
b. if philo dies, it sets it to number of philosophers straight away, so monitor stops the simulation right away
- stopping the simulation in processes: monitoring named semaphore and stop_simulation named semaphore, both initialized with 0:
a. if philo is full, he posts to monitoring semaphore which waits n times and when it reaches number of philosophers, continues to post to stop_simulation semaphore
b. if philo dies, it posts to stop_simulation semaphore n times altogehter, which allows to continue straight away to post to the stop_simulation semaphore

## TODO - working on
- Mandatory: (almost) done
- TODO: they die eventually when sleep is 0 - probably steal resources as they sleep 0
- sometimes semaphore already exists and it will not create it anew - because program terminated abnormaly and did ot close and unlink the semaphore form memory, thus we cannot open it again. Create one with new name 
- LEFT ON: DID WHAT IS ABOVE, WAITING FOR SEMAPHORE IN MAIN, POSTING TO SEMAPHORE ONCE WHEN PHILO IS FUL OR NUMBER_OF_PHILOS TIMES WHEN ONE DIES SO WE CAN KILL ALL PROCESSES
- possible way not to make them print anything after stop_sim signal: 	set sem_wait(philos[0].shared->msg_sem); so it takes the one resource and pothers cannot take it BUT it doesnt work 
- // TODO: move start time to later in code as now it is too soon
- TODO: not allowed mutexes in bonus part ??? 

### MECHANISMS TO AVOID DEADLOCKS
1. even philos wait for a moment before grabbing forks
2. odd philos grab first left, then right, odd grab first right, then left forks 

### MECHANISMS TO AVOID DYING
1. think time is calculated based on how much time they have left beofre dying, not static 
3. the odd start eating right away, the even a couple miliseconds later, because thinking or sleeping first would unneccessarily reduce their time_to_die limit
4. tried to avoid busy-waiting 

### HOW I CHECK FOR END OF SIMULATION (DEATH OR FINISHED EATING)
- monitoring thread checks every 5-8 ms if the time since last meal did not surpass the time_to_die constant 
- if it surpassed, it will set the stop_simulation to true and exit the monitoring thread
- the philos after each action check if the stop_simulation is true with check_stop_sim function 
- if stop_simulation is found to be true, they will exit the loop and the routine straight away
- if the philo ate for the last time, he will set the finished variable to true and the routine will exit after he ate for the last time 
- monitoring thread will keep checking only the philosophers that have the finished variable set to false
- after all threads finished eating, in the join caller we set stop_simulation to true so monitoring can stop and exit too
- after each action, philosopher threads check if the monitoring thread did not find any death and set stop_simulation to true, if yes exit right away so we do not print any more information to the console

## BONUS
- processes are properly created and freed and they get the philo data

## TEST CASES
### --- incorrect input - check parsing ---
1. ./philo
2. ./philo 100 200 300 - wrong number of arguments
3. ./philo a b c 100 - wrong arguments

### --- correct input - check functionality ---
1. ./philo 4 500 200 100 - best case scenario with even number - should run infinitely
2. ./philo 4 410 200 200 - its close but all should run infinitely
3. ./philo 5 800 200 100 - second best case scenario with odd number - should run infinitely
4. ./philo 5 800 200 100 10 - all should eat 10 times and then quit nicely
5. ./philo 1000 500 100 100 - check with huge number of philosophers
6. ./philo 5 400 300 200 - should end before first thread finishes sleeping
7. ./philo 0 500 100 200 - logical error - zero philosophers
8. ./philo 5 800 200 200 0 - logical error - they do not eat
9. ./philo 5 0 200 100 - must die instantly or not start at all
10. ./philo 1 800 200 100 - only one philo - should die instantly/after time_to_die because does not have enough forks 
11. ./philo 5 800 200 0 - 0 time to sleep - cannot die, as they take resources right away

### --- memory ---
1. valgrind --tool=helgrind ./philo 5 800 200 200 5 - no races when simulation stops when all are full
2. valgrind --tool=helgrind ./philo 5 400 200 200 5 - no races when simulation stops by one of them dying
3. valgrind --tool=helgrind ./philo 5 800 200 200 0 - no races when they are full from beginning
4. valgrind ./philo 5 800 200 200 5 - no leaks when simulation stops when all are full
5. valgrind ./philo 5 400 200 200 5 - no leaks when simulation stops by one of them dying
6. valgrind --tool=helgrind ./philo 1 800 200 100  - no races when only 1 philo
7. valgrind ./philo 1 800 200 100 - no leaks when only 1 philo

### --- check in code ---
1. ... that each thread does not access other philo data - if it gets the data but does not use it, discuss 
2. ... that last_eaten is updated at start of the meal, not after
3. ... that death is logged not more than 10ms after it happened
4. ... that when one thread eats for times_to_eat times, monitoring thread must wait until they all finish, not exit straight away, as some philos may die in the meantime

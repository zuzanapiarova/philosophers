# PHILOSOPHERS
The Dining philosophers problem. Introducing threads and sharing resources.
42Prague project
Mandatory part: philo/ - each philosopher is a thread, forks are placed between philosophers represented by mutexes
Bonus part: philo_bonus/ - each philosopher is a process, forks are placed in the middle of the table represented by a semaphore

# 1. COMPILATION
Compiling Mandatory Part: make -C philo/
Compiling Bonus Part: make -C philo_bonus/

# 2. EXECUTION
Program expects 4 or 5 arguments. Run ./philo(_bonus)/philo(_bonus) [1] [2] [3] [4] (5): 
    [1] number_of_philosophers
    [2] time_to_die (ms)
    [3] time_to_eat (ms)
    [4] time_to_sleep (ms)
    (5) (optional) number_of_times_each_philosopher_must_eat (ms)

### my approach
- working in microseconds for accuracy and logging in miliseconds
- not waiting for all to start at the same time 
- TODO?: including timezone by macro, do it by the function !
- TODO?: mutex that protects philo from not eating and dying at the same time 

# working on
- Mandatory part done.
- bonus :D

# MECHANISMS TO AVOID DEADLOCKS
1. even philos wait for a moment before grabbing forks
2. odd philos grab first left, then right, odd grab first right, then left forks 

# MECHANISMS TO AVOID DYING
1. think time is calculated based on how much time they have left beofre dying, not static 
3. the odd start eating right away, the even a couple miliseconds later, because thinking or sleeping first would unneccessarily reduce their time_to_die limit
4. tried to avoid busy-waiting 

# HOW I CHECK FOR END OF SIMULATION (DEATH OR FINISHED EATING)
- monitoring thread checks every 5-8 ms if the time since last meal did not surpass the time_to_die constant 
- if it surpassed, it will set the stop_simulation to true and exit the monitoring thread
- the philos after each action check if the stop_simulation is true with check_stop_sim function 
- if stop_simulation is found to be true, they will exit the loop and the routine straight away
- if the philo ate for the last time, he will set the finished variable to true and the routine will exit after he ate for the last time 
- monitoring thread will keep checking only the philosophers that have the finished variable set to false
- after all threads finished eating, in the join caller we set stop_simulation to true so monitoring can stop and exit too
- after each action, philosopher threads check if the monitoring thread did not find any death and set stop_simulation to true, if yes exit right away so we do not print any more information to the console

## TEST CASES
### --- incorrect input - check parsing ---
1. ./philo
2. ./philo 100 200 300 - wrong number of arguments
3. ./philo a b c 100 - wrong arguments

### --- correct input - check functionality ---
1. ./philo 4 500 200 100 - best case scenario with even number - should run infinitely
2. ./philo 5 800 200 100 - second best case scenario with odd number - should run infinitely
3. ./philo 5 800 200 100 10 - all should eat 10 times and then quit nicely
4. ./philo 1000 500 100 100 - check with huge number of philosophers
5. ./philo 4 410 200 200 - its close but all should run infinitely
6. ./philo 5 400 300 200 - should end before first thread finishes sleeping
7. ./philo 0 500 100 200 - logical error - zero philosophers
8. ./philo 5 800 200 200 0 - they do not eat
9. ./philo 5 0 200 100 - must die instantly or not start at all
10. ./philo 1 800 200 100 - only one philo - should die instantly/after time_to_die because does not have enough forks 
11. ./philo 5 800 200 0 - 0 time to sleep - cannot die, as they take resources right away


### --- check in code ---
1. ... that each thread does not access other philo data
2. ... that last_eaten is updated at start of the meal, not after
3. ... that death is logged not more than 10ms after it happened
4. ... that when one thread eats for times_to_eat times, monitoring thread must wait until they all finish, not exit straight away, as some philos may die in the meantime

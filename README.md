# PHILOSOPHERS
The Dining philosophers problem. Introducing threads and sharing resources.
42Prague project
Mandatory part: philo/ - each philosopher is a thread, forks are placed between philosophers represented by mutexes
Bonus part: philo_bonus/ - each philosopher is a process, forks are placed in the middle of the table represented by a semaphore

### important
// Q&A: Can we count the time in microseconds and print it in miliseconds ? So the calculation is more precise ? 
// Q&A: What is the starting state for a philosopher?
// Q&A: Does the philospher stop eating-let forks-start sleeping right after each other?
// Q&A: How long does each philosopher have to think? Can they stop sleeping, start thinking and then take forks and start eating right away, having thinked for no time ???
// Q&A: Is the timestamp supposed tobe since start of program or since epoch ?
- working in microseconds for accuracy and logging in miliseconds
- including timezone by macro, do it by the function !
- TODO: they die when sleep is 0 and they have enough time but some take over more resources than others 

# working on
- synchronize all threads to start at the same time ?
- store shared resources to a struct and pass it to init function from start_simulation
- norm
- bonus :D
- test again 

## possible algorithm for scheduling:
1. implement a priority variable that changes based on time_left, eg with values 1, 2, 3, 3 being most critical
- at start of every routine / before taking the forks check this priority variable
- if priority is low, sleep for a while, if more critical, sleep for just a bit, if critical, execute
priority = philo->die - (get_time_in_ms() - philo->last_eaten);
if (priority < 0)
    priority = 0; // Ensure no negative priorities
if (priority > 100)
    usleep(priority * 100); // Sleep proportional to priority
2. another solution would be to change each fork from mutex to struct with variable "available", being changed each time the fork is/isnt available, and based on this, each philo could pick up the forks only if they are both available - check before taking them if available, if not continue to another loop iteration and so on until both of them are available - but has to do this in a mutex because the state of the forks may change

# MECHANISMS TO AVOID DEADLOCKS
1. even philos wait for a moment before grabbing forks
2. odd philos grab first left, then right, odd grab first right, then left forks 

# MECHANISMS TO AVOID DYING
1. think time is calculated based on how much time they have left beofre dying, not static 
2. after each action check if the monitoring thread did not find any death and set stop_simulation to true, if yes exit right away so we do not print any more information to the console
3. start eating right away, the even a couple miliseconds later, because thinking or sleeping first would unneccessarily reduce their time_to_die limit

# HOW I CHECK FOR END OF SIMULATION (DEATH OR FINISHED EATING)
- monitoring thread checks every 5-8 ms if the time since last meal did not surpass the time_to_die constant 
- if it surpassed, it will set the stop_simulation to true and exit the monitoring thread
- the philos after each action check if the stop_simulation is true with check_stop_sim function 
- if stop_simulation is found to be true, they will exit the loop and the routine straight away
- if the philo ate for the last time, he will set the finished variable to true and the routine will exit after he ate for the last time 
- monitoring thread will keep checking only the philosophers that have the finished variable set to false
- after all threads finished eating, in the join caller we set stop_simulation to true so monitoring can stop and exit too

## TEST CASES
### --- incorrect input - check parsing ---
1. ./philo
2. ./philo 100 200 300
3. ./philo a b c 100
4. ./philo 0 500 100 200 - zero philosophers

### --- correct input - check functionality ---
1. ./philo 4 500 200 100 - best case scenario with even number - should run infinitely
2. ./philo 5 800 200 100 - second best case scenario with odd number - should run infinitely
3. ./philo 5 800 200 100 10 - all should eat 10 times and then quit nicely
4. ./philo 1000 500 100 100 - check with huge number of philosophers
4. ./philo 4 410 200 200 - its close but all should run infinitely
5. ./philo 5 400 300 200 - should end before first thread finishes sleeping
6. ./philo 5 800 200 200 0 - they do not eat
7. ./philo 5 0 200 100 - must die instantly or not start at all
8. ./philo 1 800 200 100 - only one philo - should die instantly/after time_to_die because does not have enough forks 
9. ./philo 5 800 200 0 - 0 time to sleep - cannot die, as they take resources right away


### --- check in code ---
1. ... that each thread does not access other philo data
2. ... that last_eaten is updated at start of the meal, not after
3. ... that death is logged not more than 10ms after it happened
4. ... that when one thread eats for times_to_eat times, monitoring thread must wait until they all finish, not exit straight away, as some philos may die in the meantime

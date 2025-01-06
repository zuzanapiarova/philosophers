# PHILOSOPHERS
The Dining philosophers problem. Introducing threads and sharing resources.
42Prague project
Mandatory part: philo/ - each philosopher is a thread, forks are placed between philosophers represented by mutexes
Bonus part: philo_bonus/ - each philosopher is a process, forks are placed in the middle of the table represented by a semaphore

# working on
- threads and philos are set up correctly, finding proper slgorithm to make them not die
- implement some algorithm in p_think function that makes them think only if they have enough time - there is one now, not sure if optimal
- simulation halts when there is only one philo and he dies - he tries to access right fork but it goes to forks[0] which is already locked and it dies while waiting for the resource to be unlocked - what to do now? - this can be solved by other functions of the pthread library unfortunatelly we cannot use these 

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

### important
// TODO: implement stopping as soon as one dies
// TODO: the time is probably not in or timezone but the UTC
// Q&A: What is the starting state for a philosopher?
// Q&A: Does the philospher stop eating-let forks-start sleeping right after each other?
// Q&A: How long does each philosopher have to think? Can they stop sleeping, start thinking and then take forks and start eating right away, having thinked for no time ???

## TEST CASES
### --- wrong input - check parsing ---
1. ./philo
2. ./philo 100 200 300 
3. ./philo a b c 100
4. ./philo 0 500 100 200 

### --- good input - check functionality ---
1. ./philo 4 500 200 100 - best case scenario with even number - should run infinitely
2. ./philo 5 500 200 100 - second best case scenario with odd number - should run infinitely
3. ./philo 5 500 200 100 10 - all should eat 10 times and then quit nicely
4. ./philo 5 400 200 199 - all should run infinitely
5. ./philo 5 400 300 200 - should end before first thread finishes sleeping


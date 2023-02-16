/*****
 * Multiprocessing to generate perfect numbers
 * for CS3510
 * by Abhay Shankar K
 * CS21BTECH11001
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <math.h>

#define PARAMS PROT_READ | PROT_WRITE, MAP_SHARED
#define PARAMS_STD NULL, 1024, PARAMS, fd, 0
// macos doesn't have non-standard itoa. So, this.
#define itos(name, i) sprintf(name, "%d", i)

#define SIZE 50


int * primes;
pid_t * children; 
int n, k; // inputs.

// checks whether a number is prime by a linear search of primes[]. 
//This is better than binary search for this specific application, because the inputs will generally be small.
int isprime(int input)
{
    int i;
    for(i = 1; i <= primes[0]; i++){
        if(primes[i] >= input) break;
    }
    if(input == primes[i]) return 1;
    else return 0;
}

// generates the prime sieve.
void gen_sieve(int upper_bound)
{
    int S[] = {1, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 49, 53, 59};
    int p = (int)ceil(sqrt(upper_bound));
    int * bools = calloc(upper_bound + 2, sizeof(int));
    primes = calloc(upper_bound + 2, sizeof(int));

    primes[0] = 4;
    primes[1] = 2;
    primes[2] = 3;
    primes[3] = 5;

    for(int x = 1; x <= p; x++){
        for(int y = 1; y <= p; y += 2){
            int m = 4*x*x + y*y;
            if(m > upper_bound) break;
            int q = m % 60;
            int s[] = {1, 13, 17, 29, 37, 41, 49, 53};
            for(int j = 0; j < 8; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}   
        }
    }
    for(int x = 1; x <= p; x++){
        for(int y = 2; y <= p; y += 2){
            int m = 3*x*x + y*y;
            if(m > upper_bound) break;
            int q = m % 60;
            int s[] = {7, 19, 31, 43};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }
    for(int x = 2; x <= p; x++){
        for(int y = x - 1; y >= 1; y -= 2){
            int m = 3*x*x - y*y;
            if(m > upper_bound) break;
            int q = m % 60;
            int s[] = {11, 23, 47, 59};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }

    int * holes = calloc(upper_bound + 2, sizeof(int));
    int c = 0;
    for(int i = 0; i <= upper_bound; i += 60){
        for(int j = 0; j < 16; j++){
            holes[c] = i + S[j];
            c++;
        }
    }
    // holes[0] = c;
    for(int i = 1; i < c; i++){
        int sq = holes[i]*holes[i];
        if(sq > upper_bound) break;
        if(bools[i]){
            for(int r = 0; r < c; r++){
                int m = sq * r;
                if(m > upper_bound) break;
                bools[m] = 0;
            }
        }
    }
    for(int i = 7; i <= upper_bound; i++){
        if(bools[i]){
            primes[primes[0]] = i;
            primes[0]++;
        }
    }
    free(bools);
    free(holes);


}

// logging. Consider renaming to lumberjack().
void outcome(int number, int status, char * file_name)
{
    FILE * f = fopen(file_name, "a+");
    if(status) fprintf(f, "%d: is a perfect number\n", number);
    else fprintf(f, "%d: is not a perfect number\n", number);
    fclose(f);
}

// uses the Euclid-Euler theorem (stated in report). This is the fastest, because it is specifically about perfect numbers.
// Assumes that no odd perfect numbers exist: valid, since no odd perfect numbers have been discovered.
int mersenne_check(int input)
{
    double soln = 0.25 + sqrt(1.0 / 16 + ((double)input)/2);
    if(soln != (int)soln) return 0;
    soln = 2*soln;
    double p = log2(soln);
    if(p != (int)p) return 0;
    soln--;
    return isprime((int)p) && isprime((int)soln);
}

// Recursive process-spawner. No forks outside this function.
void many_fork(int pid, int count)
{
    if(pid){
        children[count] = pid; 
        if(count) many_fork(fork(), count - 1);
    } // parent
    else return;
    
}

// Creates a shared memory object with convenient parameters. No shared memory creation outside this function.
char * makeshm(char * name, int create)
{
    int fd;
    if(create) fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    else fd = shm_open(name, O_RDWR, 0666);

    ftruncate(fd, 1024);

    char * out = mmap(PARAMS_STD);

    return out;

}

// Makes one SHM for each child process, writes interval to be checked by a process into its corresponding SHM. 
void init_shm(pid_t ori)
{
    char * name = malloc(7*sizeof(char));
    for(int i = k - 1; i >= 0; i--){
        itos(name, children[i]);
        char * out = makeshm(name, 1);
        int m = ceil((double)n / k);
        int s = m*(i) + 1; // start
        int e = m*(i + 1)>n?n:m*(i + 1); // end
        sprintf(out, "%d %d\n", s, e); // SHM write.
    }
    free(name);
}

// Called in parent. Calls waitpid() repeatedly, once for each child process. 
// Reads from SHMs of terminated children and writes perfect numbers to output file.
void wait_all(){
    // Output file.
    // FILE * f = fopen("OutMain.txt", "w+");
    char * name = malloc(7*sizeof(char)); // 4 or 5 digit pid, one for \0 and one more for padding.
    
    for(int i = 0; i < k; i++){
        waitpid(children[i], NULL, WUNTRACED);
        itos(name, children[i]);
        char * read_from = makeshm(name, 0);

        if(strlen(read_from))
            printf("P%d:%s\n", i + 1, read_from);
        
        shm_unlink(name);
    }
    free(name);
    // fclose(f);
}

// Called in child. Iterates over given interval, checks for perfectness. 
// Also call outcome(), which writes an entry into a logfile for each number checked.
void contents_of_child()
{
    // alloc all
    char * name = calloc(7, sizeof(char)); // 5 digit pid, one for \0 and one more for padding.
    char * perfs = calloc(23, sizeof(char)); // Only 5 perfect numbers less than 10^9. Their length would be 22. "6 28 496 8128 33550336"
    char * temp = calloc(9, sizeof(char)); // contains only one perfect number at a time.
    char * logname = calloc(20, sizeof(int)); // 16 for text, 3 for process number.

    itos(name, getpid());

    char * personal_shm;
    while(!(personal_shm = makeshm(name, 0))); // makes sure that SHM is valid.

    // printf("In child %s.\n", name);
    int s, e;
    int p_copy;
    while(sscanf(personal_shm, "%d %d", &s, &e) == -1); // makes sure that SHM has content.
    int num = (e==n)?k:((s - 1)/(e - s + 1) + 1);

    sprintf(logname, "logs/OutFile%d.log", num);

    for(int i = s; i <= e; i++){
        int stat = mersenne_check(i);  // *
        if(stat){
            sprintf(temp, "%d ", i); // buffer
            // printf("P%d PERF: %d\n", num, i);
            strcat(perfs, temp);
        }
        // outcome(i, stat, logname); // this is the costliest portion: 
        //the program obviously runs several orders of magnitude faster when it doesn't have to log a million entries.
    }
    sprintf(personal_shm, "%s", perfs); // actual shm write. 

    // free all
    free(name);
    free(perfs);
    free(logname);
    free(temp);
        
}

// Handles user input of filename.
// Calls gen_sieve(), many_fork(), init_shm(), wait_all() and contents_of_child(), as well as semaphore system calls.
int main()
{
    // printf("Enter input file.\n");
    // char * fname = malloc(1);
    // size_t buf = 0;
    // getline(&fname, &buf, stdin);
    // fname[strlen(fname) - 1] = 0; // removing terminal \n.

    // FILE * fin = fopen(fname, "r");
    // fscanf(fin, "%d %d", &n, &k);
    // fclose(fin); // we do not need this anymore

    // free(fname);

    scanf("%d %d", &n, &k);

    // Constraints. Due to logging, the program takes too long to compute large numbers.
    // Since only 8 threads can run concurrently on my machine, it is pointless to allow for more processes. 
    // k < 100 just in case this program is tested on a server.
    // Upper bound on n approaches INT_MAX. long could be used, but it is too time-consuming to run.
    if(n < 0 || k < 0){
        printf("Must be positive.\n");
        return 1;
    }
    if(n / 1000000001 || k / 101){
        printf("Too big.");
        return -1;
    }
    
    // system("rm logs/*"); 
    // removes all previous logfiles. Since they are opened in append mode, 
    // this is necessary, otherwise the entries of the previous execution will remain.

    // Semaphores (generalisation of mutex) allow for process synchronisation. 
    // We use one here to ensure that init_shm() executes before the child processes try to read from SHM.
    sem_t * s;

    sem_unlink("/delay"); 
    // in case of abnormal termination, this statement is necessary, 
    // because otherwise the semaphore is locked by sem_wait and cannot be opened.

    if((s = sem_open("/delay", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) 
        printf("Parent: %s\n", strerror(errno)); 
    sem_wait(s);

    // The fact that we need less than sqrt(2x) primes to check for perfectness of x follows from the Euclid-Euler theorem. 
    // For feasible values of n, sqrt(n) primes are sufficient, because of the distribution of perfect numbers.
    gen_sieve((int)ceil(sqrt(2*n))); 

    // for(int i = 1; i < primes[0]; i++) printf("%d\n", primes[i]);

    children = calloc(k+1, sizeof(int)); // +1 because it also include ori_pid at children[k].

    const pid_t ori_pid = getpid(); 

    // fork called in this
    many_fork(ori_pid, k); // first argument can be anything nonzero. ori used for information consistency. If using other things, changes in init_shm().

    // parent in if, children in else.
    if(getpid() == ori_pid){
        // for(int i = 0; i < k; i++) printf("%d\n", children[i]);
        //makeshm (and hence shm_open and mmap) called in this.
        init_shm(ori_pid);
        // printf("Fin init.\n"); // This statement was used to indicate completion of init_shm().
        sem_post(s); // releases the semaphore. All child processes can now run (asynchronously), until the waitpid() call.

        // waits. Compiles information from all shms into OutMain.txt. Unlinks shms.
        wait_all();
        
    }
    else{
        sem_wait(s); // ensures that child process begins only after semaphore is released by parent.
        sem_post(s); // we don't want the semaphore owned by any child process - they must run asynchronously and simultaneously.
        // Hence, we immediately call sem_post().
        
        
        contents_of_child();
        
    }
    
    free(children);
    free(primes);
    sem_unlink("/delay"); // so that we can use the semaphore the next time we run the program.
    // being kernel objects, semaphores are not destroyed when the callig process is terminated.

}


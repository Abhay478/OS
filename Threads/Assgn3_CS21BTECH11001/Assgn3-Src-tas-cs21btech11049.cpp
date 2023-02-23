#include <iostream>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <utility>
#include <atomic> 
#include <sys/time.h>
#include <thread>
#include <chrono>


using namespace std;
typedef long long ll;

vector<ll> indices; //Global vector used to index the threads
ll k_global; //Global variable to store k
ll l1_global, l2_global; //Global variables to store l1 and l2
struct timeval tv1; //Global variable to store the start time
FILE* fp_output; //Global file pointer to store the output
atomic_flag tas_lock = ATOMIC_FLAG_INIT; //Global atomic variable to store the lock

//A function to generate exponential random variables with average lambda
float exp_dist(ll lambda)
{
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<> d(lambda);
    return d(gen);
}


//A function to test the critical section (using test and set)
void* testCS(void* index)
{
    struct timeval tv2;
    float current_time;
    for(ll i = 0; i < k_global; i++)
    {
        ll tid = *((ll*)index); //Get the thread number
        float t1 = exp_dist(l1_global); //Generate t1
        float t2 = exp_dist(l2_global); //Generate t2

        gettimeofday(&tv2, NULL); //Get the current time
        current_time = (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
        fprintf(fp_output, "Thread %lld requests to enter CS at time %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section
        // printf("Thread %lld requests to enter CS at time %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section

        /* ENTRY SECTION */
        while(tas_lock.test_and_set()); //Acquire the lock

        /* CRITICAL SECTION */
        gettimeofday(&tv2, NULL); //Get the current time
        current_time = (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
        fprintf(fp_output, "Thread %lld entered CS at time %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section
        // printf("Thread %lld entered CS at time %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section

        this_thread::sleep_for(chrono::microseconds((int)(t1*1000000))); //Sleep for t1 milliseconds to simulate critical section

        tas_lock.clear(); //Release the lock

        /* EXIT SECTION */

        gettimeofday(&tv2, NULL); //Get the current time
        current_time = (double)(tv2.tv_usec - tv1.tv_usec)/1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
        fprintf(fp_output, "Thread %lld exited CS at time %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section
        // printf("Thread %lld  %f ms\n", tid, current_time*1000); //Print the time at which the thread requests to enter the critical section

        this_thread::sleep_for(chrono::microseconds((int)(t2*1000000))); //Sleep for t2 milliseconds to simulate the remainder section
        

    }

    pthread_exit(0);
}

int main()
{
    gettimeofday(&tv1, NULL); //Function used to compute time
    fp_output = fopen("output-tas.txt", "w"); //Open the output file    

    ifstream input_file;
    input_file.open("inp-params.txt");

    ll n, k, l1, l2; // n = number of threads, k = number of times threads requests critical section entry, l1 = average of exp distrubution for t1, l2 = average of exp distrubution for t2

    if(input_file.is_open())
    {
        input_file >> n >> k >> l1 >> l2; // reading input from file
    }
    k_global = k; //Store k in global variable
    l1_global = l1; //Store l1 in global variable
    l2_global = l2; //Store l2 in global variable

    pthread_t threads[n]; //Create 'nthreads' threads

    for(ll i = 0; i < n; i++) indices.push_back(i); //Set the thread numbers by pushing to global vector (0-indexed) 

    // Create 'n' threads
    for(ll i = 0; i < n; i++) pthread_create(&threads[i], NULL, &testCS, (void *)&indices[i]);
    //Wait for the threads to stop executing
    for(ll i = 0; i < n; i++) pthread_join(threads[i], NULL);


    fclose(fp_output); //Close the output file
}
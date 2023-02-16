#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int * primes;
int prime(int input)
{
    int i;
    for(i = 1; i <= primes[0]; i++){
        if(input <= primes[i]) break;
    }
    // for(int i = 1; i <= primes[0]; i++)
    if(input == primes[i]) return 1;
    else return 0;
}

// int n;

void gen(int n)
{
    int S[] = {1, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 49, 53, 59};
    int p = ceil(sqrt(n));
    int * bools = calloc(n + 1, sizeof(int));
    primes = calloc(p + 1, sizeof(int));

    primes[0] = 4;
    primes[1] = 2;
    primes[2] = 3;
    primes[3] = 5;

    for(int x = 1; x <= p; x++){
        for(int y = 1; y <= p; y += 2){
            int m = 4*x*x + y*y;
            if(m > n) break;
            int q = m % 60;
            int s[] = {1, 13, 17, 29, 37, 41, 49, 53};
            for(int j = 0; j < 8; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}   
        }
    }
    for(int x = 1; x <= p; x++){
        for(int y = 2; y <= p; y += 2){
            int m = 3*x*x + y*y;
            if(m > n) break;
            int q = m % 60;
            int s[] = {7, 19, 31, 43};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }
    for(int x = 2; x <= p; x++){
        for(int y = x - 1; y >= 1; y -= 2){
            int m = 3*x*x - y*y;
            if(m > n) break;
            int q = m % 60;
            int s[] = {11, 23, 47, 59};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }

    int * holes = calloc(ceil((4.0 * n)/15), sizeof(int));
    int c = 0;
    for(int i = 0; i <= n; i += 60){
        for(int j = 0; j < 16; j++){
            holes[c] = i + S[j];
            c++;
        }
    }
    // holes[0] = c;
    for(int i = 1; i < c; i++){
        int sq = holes[i]*holes[i];
        if(sq > n) break;
        if(bools[i]){
            for(int r = 0; r < c; r++){
                int m = sq * r;
                if(m > n) break;
                bools[m] = 0;
            }
        }
    }
    for(int i = 7; i <= n; i++){
        if(bools[i]){
            primes[primes[0]] = i;
            primes[0]++;
        }
    }


}

int mersenne_check(int in)
{
    double soln = 0.25 + sqrt(1.0 / 16 + ((double)in)/2);
    if(soln != (int)soln) return 0;
    // printf("s:%lf\n", soln);
    soln = 2*soln;
    double p = log2(soln);
    if(p != (int)p) return 0;
    // printf("s:%lf p:%lf\n", soln, p);
    soln--;
    return prime((int)p) && prime((int)soln);
}

int main()
{
    int n = 1000000000;
    gen((int)ceil(sqrt(n)));
    for(int i = 1; i < primes[0]; i++) printf("%d\n", primes[i]);

    // for(int i = 2; i < n; i++){
    //     if(mersenne_check(i)) printf("%d\n", i);
    // }

    printf("%d\n", mersenne_check(33550336));


}
// ******************************************************************************************************************
//  Prime Factors - multi-threaded program which finds the prime factors of the given numbers.
//  Copyright(C) 2018  James LoForti
//  Contact Info: jamesloforti@gmail.com
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.If not, see<https://www.gnu.org/licenses/>.
//									     ____.           .____             _____  _______   
//									    |    |           |    |    ____   /  |  | \   _  \  
//									    |    |   ______  |    |   /  _ \ /   |  |_/  /_\  \ 
//									/\__|    |  /_____/  |    |__(  <_> )    ^   /\  \_/   \
//									\________|           |_______ \____/\____   |  \_____  /
//									                             \/          |__|        \/ 
//
// ******************************************************************************************************************
//

#include <stdlib.h> // needed for exit()
#include <stdio.h> // needed for printf()
#include <pthread.h> // needed for pthread_t
#include <math.h> // needed for floor() & sqrt()

#define MAX_NUMS 25
#define MAX_FACTORS 10

//Purpose: Helper function for prime factoring
void* factor(void* container);

//Purpose: To test for primality
int prime_tester(int num);

//Purpose: To flag all non-prime numbers with a 1
void prime_builder(char* flags, int primeThreshold);

//Purpose: To find the first prime factor, then the following component factor
//If that component is prime, finish. If that component is not prime, recursion
void trial_divider(int num, char* flags, int primes[], int count);

//Struct to hold data to/from threads
struct Data
{
	int num;
	int primes[MAX_FACTORS];
};

//Create an array of threads and thread data structs
pthread_t threads[MAX_NUMS];
struct Data threadData[MAX_NUMS];

int main(int argc, char* argv[])
{
    //Print opening seperator, name
    printf("\n*********************************************** ");
    printf("\nName: James LoForti \n\n");

	//Declare & init vars:
	int i = 0;
	int j = 0;
	int result = 0;

    //For every number given - thread creation
    for (i = 0; i < argc - 1; i++)
    {
    	//Save the given number as num in the data struct
    	threadData[i].num = atoi(argv[i + 1]);
    	
    	//Create new thread
		result = pthread_create(&threads[i], NULL, factor, &threadData[i]);

		//If thread creation FAILED
	  	if (result != 0) 
	  	{
	    	perror("An error occurred during thread creation... ");
	    	exit(1);
	  	}
    } // end for
    
    //For every number given - thread joining
    for (i = 0; i < argc - 1; i++)
    {	  	
  	  	//Have parent wait for new thread to finish
		result = pthread_join(threads[i], NULL);
		
		//If thread join FAILED
	  	if (result != 0) 
	  	{
	    	perror("An error occurred during thread joining... ");
	    	exit(1);
	  	}
    } // end for

    //Print header
    printf("Prime Factors:\n");

    //For every number given - prime factor printing
    for (i = 0; i < argc - 1; i++)
    {
    	//Print original number
    	printf("\t%s: ", argv[i + 1]);

    	//For this given number
    	for (j = 0; j < MAX_FACTORS; j++)
    	{
    		//If value is NOT null (0)
    		if (threadData[i].primes[j] != 0)
    		{
	    		//Print prime factor
    			printf("%d ", (threadData[i].primes[j]));
    		}
    	} // end for

    	printf("\n");
    } // end for

    //Print closing seperator
    printf("\n*********************************************** \n");

	return 0;
} // end main()

void* factor(void* container)
{
	//Cast the given struct from void* to Data*
	struct Data* data = (struct Data*)container;
	
	//Save num and init count to track next available element in prime array
	int num = data->num;
	int count = 0;

	//If num is prime (1)
	if (prime_tester(num))
	{
		//Add itself to the array
		data->primes[count] = num;
		return data;
	}
	else // num is NOT prime
	{
		//Use the squareroot as the highest possible prime factor 
		//Any values between the sqrt(n)-num will already have been tested
		int primeThreshold = floor(sqrt(num));

		//Allocate memory to track prime and non-prime numbers via flags
		char* flags = calloc(primeThreshold, 1);

		//Flag all non-prime numbers
		prime_builder(flags, primeThreshold);

		//Find prime factors via trial division
		trial_divider(num, flags, data->primes, count);

		//Deallocate flags
		free(flags);
	} // end else

	return NULL;
} // end function factor()

int prime_tester(int num)
{
	//If num is evenly divisibe by 2 (NOT prime)
	if (num % 2 == 0)
	{
		//Needed to recognize 2 as prime
		if (num == 2)
			return 1;
		else
			return 0;
	} // end if

	//At this point we know that num is not divisible by 2
	//Therefore, it is not divisible by any multiple of 2

	//Use the squareroot as the highest possible prime factor 
	int primeThreshold = floor(sqrt(num));
	
	//For every number between 1 and the sqrt of num
	int i;
	for (i = 1; i < primeThreshold; i++)
	{
		//Save the next odd value
		int temp = (2 * i) + 1;

		//If the current odd number is a factor of num
		if (num % temp == 0)
		{
			//Num is NOT prime
			return 0;
		}
	} // end for

	//Num is prime, return 1
	return 1;
} // end function prime_tester()

void prime_builder(char* flags, int primeThreshold)
{
	//For all possible primes
	int i;
	for (i = 2; i <= primeThreshold; i++)
	{
		//If this values multiples have not been marked as non-prime yet (0)
		if (!flags[i])
		{
			//Iterate through all multiples of 2-sqrt(num)
			int j;
			for (j = 2 * i; j < primeThreshold; j += i)
			{
				//Flag as NOT prime (1)
				flags[j] = 1;
			} // end for
		} // end if
	} // end for
} // end function prime_builder()

void trial_divider(int num, char* flags, int primes[], int count)
{
	//Use the squareroot as the highest possible prime factor 
	int primeThreshold = floor(sqrt(num));

	//For all numbers from 2-sqrt(num)
	int i;
	for (i = 2; i <= primeThreshold; i++)
	{
		//If the current value is not prime (1)
		if (flags[i])
		{
			//Skip it
			continue;
		}
		else // current value is prime
		{
			//Current value is prime, so save it
			int prime = i;

			//If the last prime has been found (prime^2)
			if ((pow(prime, 2)) == num)
			{
				//Add it to the list
				primes[count++] = prime;
				primes[count++] = prime;
				break;
			} // end if

			//If the current prime produces a component prime
			if (num % prime == 0)
			{
				//Add it to the list
				primes[count++] = prime;

				//Find the component factor
				int component = num / prime;

				//If the component is prime
				if (prime_tester(component))
				{
					//Add it to the list
					primes[count++] = component;
				}
				else // component is not prime
				{
					//Factor the component
					trial_divider(component, flags, primes, count);
				}

				break;
			} // end if
		} // end else
	} // end for
} // end function trial_divider()

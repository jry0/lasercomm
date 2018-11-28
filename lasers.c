#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
enum State {START, DONE, BROKE_FIRST, BROKE_BOTH, BROKE_SECOND, ENTERED, EXITED};
//struct to record the data we recieved from the laser input
struct Room{
	int in;
	int out;
	int laser1Counter;
	int laser2Counter;
};
//HARDWARE DEPENDENT CODE BELOW
#ifndef MARMOSET_TESTING`

//This function should initialize the GPIO pins
GPIO_Handle initializeGPIO()
{
		
	//This is the same initialization that was done in Lab 2
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio == NULL)
	{
		perror("Could not initialize GPIO");
	}
	return gpio;

}
#define LASER1_PIN_NUM 17 //used to replace LASER1_PIN_NUM with 17 when compiled
#define LASER2_PIN_NUM 18 // used to replace LASER2_PIN_NUM with 18 when compiled
//This function should accept the diode number (1 or 2) and output
//a 0 if the laser beam is not reaching the diode, a 1 if the laser
//beam is reaching the diode or -1 if an error occurs.
int laserDiodeStatus(GPIO_Handle gpio, int diodeNumber)
{
	if(gpio == NULL){
		return -1;
	}
	if (diodeNumber == 1){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
		if(level_reg & (1 << LASER1_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}else if(diodeNumber == 2){
		uint32_t level_reg = gpiolib_read_reg(gpio, GPLEV(0));
		if(level_reg & (1 << LASER2_PIN_NUM)){
			return 1;
		}
		else{
			return 0;
		}
	}else{
		return -1;
	}
	
}
#endif
//END OF HARDWARE DEPENDENT CODE
//This function will output the number of times each laser was broken
//and it will output how many objects have moved into and out of the room.

//laser1Count will be how many times laser 1 is broken (the left laser).
//laser2Count will be how many times laser 2 is broken (the right laser).
//numberIn will be the number  of objects that moved into the room.
//numberOut will be the number of objects that moved out of the room.
void outputMessage(int laser1Count, int laser2Count, int numberIn, int numberOut)
{
	printf("Laser 1 was broken %d times \n", laser1Count);
	printf("Laser 2 was broken %d times \n", laser2Count);
	printf("%d objects entered the room \n", numberIn);
	printf("%d objects exitted the room \n", numberOut);
}

//This function accepts an errorCode. You can define what the corresponding error code
//will be for each type of error that may occur.
void errorMessage(int errorCode)
{
	fprintf(stderr, "An error occured; the error code was %d \n", errorCode);
}


#ifndef MARMOSET_TESTING
/*
This function takes the gpio and time alloted and processes the current states of both photodiodes. contains a state machiene to process these inputs
and if an object passes through the lasers the state machiene increments whatever variable is associated with the actions
*/
struct Room recordedPeopleEnteringAndLeaving(GPIO_Handle gpio,int timeAlloted){
	enum State s = START;
	struct Room r;
	int laser1;
	int laser2;
	int inFlag = 0;
	int outFlag = 0;
	int flag1  = 0;
	int flag2 = 0;
	int inside = 0;
	int outside = 0;
	r.laser1Counter = 0;
	r.laser2Counter = 0;
	time_t current = time(NULL);
	time_t start = time(NULL);
	int timePassed = 0;
	while(timePassed < timeAlloted){
		laser1 = laserDiodeStatus(gpio, 1);
		laser2 = laserDiodeStatus(gpio, 2);
		switch(s){
			case START:
				if(!laser1){
					s = BROKE_FIRST;
					printf("1");
			
					inFlag = 1;
					break;
				}else if(!laser2){
					s = BROKE_SECOND;
					outFlag = 1;
					break;
				}
				break;
			case BROKE_FIRST:
				if(outFlag){
					if(laser1){
						s = EXITED;
						break;
					}
					else if(!laser2){
						s = BROKE_BOTH;
				
						r.laser2Counter++;
						break;
					}
					break;
				}else if(!outFlag){
					if(!laser2){
						s = BROKE_BOTH;
				
						r.laser2Counter++;
						inFlag = 1;
						break;
					}else if(laser1){
					
						s = START;
						break;
					}
				}
				break;
			case BROKE_SECOND:
				if(inFlag){
					if(laser2){
						s = ENTERED;
						break;
					}else if(!laser1){
					
						s = BROKE_BOTH;
						r.laser1Counter++;
						break;
					}
					break;
				}else{
					if(!laser1){
					
						s = BROKE_BOTH;
						outFlag = 1;
						r.laser1Counter++;
						break;
					}else if(laser2){
						s = START;
						break;
					}
				}
				break;
			case BROKE_BOTH:
				if(!laser1 && laser2){
					s = BROKE_FIRST;
					r.laser1Counter++;
					break;
				}else if(laser1 && !laser2){
					s = BROKE_SECOND;
					r.laser2Counter++;
					break;
				}
				break;
			case ENTERED:
				inside++;
				s = START;
				inFlag = 0;
				break;
			case EXITED:
				outside++;
				s = START;
				outFlag = 0;
				break;
			case DONE:
				break;
	}
		usleep(200);
		current = time(NULL);
		timePassed = difftime(current,start);
	}
	r.in = inside;
	r.out = outside;
	return r;
}
int main(const int argc, const char* const argv[])
{
	//We want to accept a command line argument that will be the number
	//of seconds that the program runs for, ensure that there is time for
	//it to run
	if(argc < 2){
		printf("Error, no time given: exitting \n");
		return -1;
	}

	//Initialize the GPIO pins
	GPIO_Handle gpio = initializeGPIO();
	//create a variable for the start time, time_t is a special variable used by 
	//the time library for calendar times, the time() function returns the current calendar
	//time and you use NULL as the argument for the time()  function
	int timeLimit = atoi(argv[1]);
	struct Room data = recordedPeopleEnteringAndLeaving(gpio,timeLimit);
	outputMessage(data.laser1Counter, data.laser2Counter, data.in,data.out);
	//Free the GPIO now that the program is over.
	gpiolib_free_gpio(gpio);
}

#endif

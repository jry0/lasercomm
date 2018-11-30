#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>
enum State{START, GOT1, GOT2, GOT0, NEXT, DONE};

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

#define LASER1_PIN_NUM 15
#define LASER2_PIN_NUM 18


void writte(FILE* output, int ascii){
	char c = 0;
	if(ascii != 32){
printf("hii");
		c = ascii;
	}else if(ascii == 32){
		c = ' ';
	}
	printf("%c\n",c);
	fprintf(output, "%c", c);
	
}

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

void receive(GPIO_Handle gpio, FILE* output){
	int letter = 0;
	int prev1;
	int prev2;
	int space = 0;
	int laser1;
	int laser2;
	enum State s = START;
	while(s != DONE){
		laser1 = laserDiodeStatus(gpio, 1);
		usleep(20);
                laser2 = laserDiodeStatus(gpio, 2);
		switch(s){
			case START:
				if(laser1 == 1){
					letter++;
printf("hello %d\n",laser1);
					s = GOT1;
					break;
				}else if(laser2 == 1){
					space++;
					s = GOT2;
					break;
				}
				break;
			case GOT1:
				if(!laser1){
					s = GOT0;
				}
				break;
			case GOT2:
				if(!laser2){
					s = GOT0;
				}
				break;
			case GOT0:
				if(laser1 == 1 && laser2 == 1){
					s = NEXT;
				}else if(laser1 == 1){
printf("hi %d\n",letter);
					letter++;
					s = GOT1;
				}else if(laser2){
					space++;
					s = GOT2;
				}
				break;
			case NEXT:
				if(space == 2){
printf("bye");
					s = DONE;
				}else{
					if(space == 1){
printf("%d\n", space);
						writte(output,32);
					}else{
						writte(output,letter + 64);
					s = START;
				}
				break;
			case DONE:
				break;
			}
		}
	}
}

int main(const int argc, const char* const argv[]) {

//printf("start");
  //  GPIO_Handle gpio = initializeGPIO();
//printf("GPO");
    //how long the delay will be in secs. Implemented as int to allow for quick changing
   // int delay = 10;

    //will set to '0' to break loop
    //int i = 1;
    
    // timer. Not using gettime methods because no need for precise timing here
    //int timer = 0;

    //Counter count how many consecutive cycles the laser is aligned; once past a threshold a message will be displayed
    //int consecutiveLaser = 0;
	
    /*while (i)
	{
        int pin_state = laserDiodeStatus(gpio, 1);
		if(!pin_state) //if pin_state is FALSE (not aligned)
		{
			//reset counter. Indicated unstable alignement
            consecutiveLaser = 0;
		}
		if(pin_state) //if pin_state is TRUE (laser is aligned)
		{
			//increment counter
			consecutiveLaser++;
		}
        if(consecutiveLaser > 1000) { //threshold for a stable connection
            //tell user that alignment is correct
            printf("Lasers are aligned!");
            //exit while loop
            timer = 1000001 * delay;
        }
        if (timer >= 1000000 * delay) {
            //break while loop after duration has elapsed
            i = 0;
        }
        //this will act as a (rough) timer. Make sure argument of usleep is equal to how much timer is incremented by
        usleep(1000);
        timer += 1000;
        }*/
    //printf("hi");


    /* This section of the code is the part that receives,
     decodes, and writes the incoming transmission to a 
     text file. The juicy part of the code, if you will */

	FILE* output; //output file pointer
    
        output = fopen("output.txt" , "a"); //set value of pointer to point to output.txt; will be appending to the file
	char write[100] = "Hello Julian";

	int len = strlen(write);

	for (int i = 0; i < len; i++) {
		writte(output, write[i])
	}
	
	//receive(gpio,output); //Receive, decode, and write the message

	//gpiolib_free_gpio(gpio); //Free the GPIO now that the program is over
return 0;
}

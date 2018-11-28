#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>
enum State s{START, GOT1, GOT2, GOT0, NEXT, DONE};
#define LASER1_PIN_NUM 17
#define LASER2_PIN_NUM 18
void writting(FILE* output, int ascii){
	if(ascii != 32){
		char c = ascii + 64;
	}else if(ascii == 32){
		char c = ' ';
	}
	 fprintf(output, "%c", c);
	
}
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

int recieve(GPIO_Handle gpio, FILE* output){
	int letter = 0;
	int space = 0;
	int laser1;
	int laser2;
	int prev1;
	int prev2;
	enum State s = START;
	while(s != DONE){
		laser1 = laserDiodeStatus(gpio, 1);
		usleep(20);
                laser2 = laserDiodeStatus(gpio, 2);
		switch(s){
			case START:
				if(laser1 == 1){
					letter++;
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
					s = next;
				}else if(laser1 == 1){
					letter++;
					s = GOT1;
				}else if(laser2){
					space++;
					s = GOT2;
				}
				break;
			case NEXT:
				if(space == 2){
					s = DONE;
				}else{
					if(space == 1){
						writting(output,32);
					}else{
						writting(output,ascii);
					s = START;
				}
				break;
			case DONE:
				break;
		}
		usleep(190);
	}
	return 0;
}

int main(void){
	FILE* output; //output file pointer
    
        output = fopen("output.txt" , "a"); //set value of pointer to point to output.txt; will be appending to the file
	//initialize gpio
	receive(gpio,output);
}

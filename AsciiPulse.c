// #include "gpiolib_addr.h"
// #include "gpio_reg.h"
// #include "gpiolib_reg.c"

#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>		//for the printf() function
#include <fcntl.h>
//#include <linux/watchdog.h> 	//needed for the watchdog specific constants
#include <unistd.h> 		//needed for sleep
#include <sys/ioctl.h> 		//needed for the ioctl function
#include <stdlib.h> 		//for atoi
#include <time.h> 	

int main () {
	GPIO_Handle gpio;
	gpio = gpiolib_init_gpio();
	if(gpio==NULL)
		perror("Could not init GPIO");

	uint32_t sel_reg18 = gpiolib_read_ref(gpio, GPFSEL(1)) // (A-z) Ascii output laser, set to pin 18
	sel_reg18 |= 1 << 24; 
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg18);

	uint32_t sel_reg17 = gpiolib_read_ref(gpio, GPFSEL(1)) // Space Ascii output laser, set to pin 17
	sel_reg17 |= 1 << 21;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg17);

	// read this individuals character

	
		
	FILE *file; //input file pointer

    file = fopen("file.txt" , "r"); //set value of pointer to point to input file; will be reading from the file
    
    char inString[200]; //create array to store chars from input file
    
    fgets(inString, 999, file); //using fgets to read from file, write contents into array 
    
    int len = strlen(inString); //getting length of resulting string
    
    int input[200] = {0}; //new array to store ASCII values of string 
    
    int pulseLength = 500; // useconds that laser will be turned on for. aka 1 pulse.
    for (int i = 0; i < len; i++) { //loop to typecast char to int and write to array
        input[i] = (int)(inString[i]);
        printf("%c",inString[i]);
        int pulseCounter = 0;

        if (input[i] == 32) { // if character read from file is SPACE (ASCII: 32)
        	usleep(pulseLength);
        	gpiolib_write_reg(gpio,GPSET(1), 1 << 17);
        	usleep(pulseLength);
        	gpiolib_write_reg(gpio, GPCLR(0), 1 << 17); 
        }
        else {
        	 while(pulseCounter <= input[i]) {
        	 	usleep(pulseLength);
        		gpiolib_write_reg(gpio,GPFSET(1), 1 << 18);
        		usleep(pulseLength);
        		gpiolib_write_reg(gpio, GPCLR(0), 1 << 18);
        }
        }
       
    }

    return 0;   

	}
	



// #include "gpiolib_addr.h"
// #include "gpiolib_reg.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>		//for the printf() function
#include <fcntl.h>
//#include <linux/watchdog.h> 	//needed for the watchdog specific constants
#include <unistd.h> 		//needed for sleep
#include <sys/ioctl.h> 		//needed for the ioctl function
#include <stdlib.h> 		//for atoi
#include <time.h> 		//for time_t and the time() function
//#include <sys/time.h>           //for gettimeofday()

int main() {
/* 
    This section of the code writes
    the contents of an int array (containing
    ASCII values) to a file named "output.txt"
    */

    FILE* output; //output file pointer
    
    output = fopen("output.txt" , "a"); //set value of pointer to point to output.txt; will be appending to the file
    
    for (int j = 0; j < len; j++) { //loop to write contents of array to file
        fprintf(output, "%d", inString[j]);
    }
}
    
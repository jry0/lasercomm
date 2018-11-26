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
    This section of the code reads the file
    converts it into an int array containing the ASCII
    values of the contents of the file. 
    */

    FILE *file; //input file pointer

    file = fopen("file.txt" , "r"); //set value of pointer to point to input file; will be reading from the file
    
    char inString[200]; //create array to store chars from input file
    
    fgets(inString, 999, file); //using fgets to read from file, write contents into array 
    
    int len = strlen(inString); //getting length of resulting string
    
    int input[200] = {0}; //new array to store ASCII values of string 
    
    for (int i = 0; i < len; i++) { //loop to typecast char to int and write to array
        input[i] = (int)(inString[i]);
        printf("%c",inString[i]);
    }
    return 0;   
}
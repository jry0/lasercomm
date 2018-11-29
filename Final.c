#include "gpiolib_addr.h"
#include "gpiolib_reg.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h> //for the printf() function
#include <fcntl.h>
#include <linux/watchdog.h> //needed for the watchdog specific constants
#include <unistd.h>         //needed for sleep
#include <sys/ioctl.h>      //needed for the ioctl function
#include <stdlib.h>         //for atoi
#include <time.h>           //for time_t and the time() function
#include <sys/time.h>       //for gettimeofday()

/*Defined macro for logging information. 
+fileName will be file pointer to log file
+time will be current time in string format (format listed in getTime function)
+programName will be name of program
+sev will be the serverity of the message (debug, info, warning, error, critical)
+str +will be message that will be printed*/

#define LOG_MSG(fileName, time, programName, sev, str) \
	do{ \
		fprintf(logFile, "%s : %s : %s : %s", time, programName, sev, str); \
		fflush(logFile); \
	}while(0)



enum State
{
    HUB,
    BLINK1,
    BLINK2,
    BLINK_DONE,
    DONE
};
//Used to get the current time using gettimeofday function from sys/time.h library
void getTime(char* buffer)
{
	//timeval structure: tv
	struct timeval tv;
	// get current time, stores in tv struct
	gettimeofday(&tv, NULL);

	// time_t variable: time, set to number of seconds in tv
	time_t current_time = tv.tv_sec;

	// sets buffer equal to string format of date (in month, day, year, 24-time format)
	strftime(buffer, 30, "%m-%d-% T.", localtime(&current_time));

}

void readConfig(FILE* configFile, int* timeout, char* logFileName, int* CaesarShift)
{
	//Loop counter
	int i = 0;
	
	//A char array to act as a buffer for the file
	char buffer[255];

	//The value of the timeout variable is set to zero at the start
	*timeout = 0;

	//The value of the numBlinks variable is set to zero at the start
	*CaesarShift = 0;

	//This is a variable used to track which input we are currently looking
	//for (timeout, logFileName or numBlinks)
	int input = 0;

	//This will 
	//fgets(buffer, 255, configFile);
	//This will check that the file can still be read from and if it can,
	//then the loop will check to see if the line may have any useful 
	//information.
	while(fgets(buffer, 255, configFile) != NULL)
	{
		i = 0;
		//If the starting character of the string is a '#', 
		//then we can ignore that line
		if(buffer[i] != '#')
		{
			while(buffer[i] != 0)
			{
				//This if will check the value of timeout
				if(buffer[i] == '=' && input == 0)
				{
					//The loop runs while the character is not null
					while(buffer[i] != 0)
					{
						//If the character is a number from 0 to 9
						if(buffer[i] >= '0' && buffer[i] <= '9')
						{
							//Move the previous digits up one position and add the
							//new digit
							*timeout = (*timeout *10) + (buffer[i] - '0');
						}
						i++;
					}
					input++;
				}
				else if(buffer[i] == '=' && input == 1) //This will find the name of the log file
				{
					int j = 0;
					//Loop runs while the character is not a newline or null
					while(buffer[i] != 0  && buffer[i] != '\n')
					{
						//If the characters after the equal sign are not spaces or
						//equal signs, then it will add that character to the string
						if(buffer[i] != ' ' && buffer[i] != '=')
						{
							logFileName[j] = buffer[i];
							j++;
						}
						i++;
					}
					//Add a null terminator at the end
					logFileName[j] = 0;
					input++;
				}
				else if(buffer[i] == '=' && input == 2) //This will find the value of numBlinks
				{
					//The loop runs while the character is not null
					while(buffer[i] != 0)
					{
						//If the character is a number from 0 to 9
						if(buffer[i] >= '0' && buffer[i] <= '9')
						{
							//Move the previous digits up one position and add the
							//new digit
							*CaesarShift = (*CaesarShift *10) + (buffer[i] - '0');
						}
						i++;
					}
					input++;
				}
				else
				{
					i++;
				}
			}
		}
	}
}

//This function will initialize the GPIO pins and handle any error checking
//for the initialization
GPIO_Handle initializeGPIO(FILE* logFile, char programName)
{       
        //This is the same initialization that was done in Lab 2
        GPIO_Handle gpio;
        gpio = gpiolib_init_gpio();

        if(gpio == NULL)
        {       
                perror("Could not initialize GPIO");
        }
        LOG_MSG(logFile, time, programName, "INFO", "Pin 17 has been set to output\n\n");
        return gpio;
}

//This function will change the appropriate pins value in the select register
//so that the pin can function as an output
void setToOutput(GPIO_Handle gpio, int pinNumber, FILE* logFile, char programName)
{       
        //Check that the gpio is functional
        if(gpio == NULL)
        {       
                printf("The GPIO has not been intitialized properly \n");
                return;
        }
        
        //Check that we are trying to set a valid pin number
        if(pinNumber < 2 || pinNumber > 27)
        {       
                printf("Not a valid pinNumer \n");
                return;
        }
        
        //This will create a variable that has the appropriate select
        //register number. For more information about the registers
        //look up BCM 2835.
        int registerNum = pinNumber / 10;
        
        //This will create a variable that is the appropriate amount that
        //the 1 will need to be shifted by to set the pin to be an output
        int bitShift = (pinNumber % 10) * 3;
        
        //This is the same code that was used in Lab 2, except that it uses
        //variables for the register number and the bit shift
        uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(1));
        sel_reg |= 1  << 24;
        gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
	sel_reg |= 1<< 15;
	gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
}

int encode(int input, int CaesarShift)
{
    int in = input + CaesarShift;
    return in;
}

void Send(GPIO_Handle gpio, int ascii, FILE* logFile, char programName, int timeout)
{
   
    int laser2 = 0; //how many times laser2 needs to blink

    int laser1 = 0; //how many times laser1 needs to blink

    if (ascii == 32)
    {
        laser2 = 1; //space?
    }
    else if (65 <= ascii && ascii <= 124)
    {
        laser1 = ascii - 64; //letter
    }
    else
    {
        laser2 = 2; //done (received something invalid, indicating end of transmission)
    }

    // Initialization of the wacthdog timer
    // watchdog file opened 
    int watchdog;
    watchdog = open("/dev/watchdog", O_RDWR | O_NOCTTY);
    
    // timer set to timeout value from config file
    ioctl(watchdog, WDIOC_SETTIMEOUT, &timeout);


    enum State s = HUB;

    while (s != DONE)
    {
        switch (s)
        {
        case HUB:
        	//kicks the watchdog, resetting the timer to 0
        	//state machine returns to HUB after each state
        	ioctl(watchdog, WDIOC_KEEPALIVE, 0);

            if (laser1 == 0 && laser2 == 0)
            {
                s = BLINK_DONE; //blink both lights to indicate end of transmission
                break;
            }
            else if (laser2 != 0 && laser1 == 0)
            {
                s = BLINK2; //blink LED2
                break;
            }
            else
            {
                s = BLINK1; //blink LED1
                break;
            }
        case BLINK1:
		printf("hi");
            gpiolib_write_reg(gpio, GPSET(0), 1 << 15); //turn on laser1
            usleep(2000);
		printf("bye");
            laser1--;                                   //decrement laser1 counter
            gpiolib_write_reg(gpio, GPCLR(0), 1 << 15); 
		usleep(2000);//turn off laser1
		printf("si");
            s = HUB;
            break;
        case BLINK2:
		printf("hello");
            gpiolib_write_reg(gpio, GPSET(0), 1 << 18); //turn on laser2
            usleep(2000);
            laser2--;                                   //decrement laser2 counter
            gpiolib_write_reg(gpio, GPCLR(0), 1 << 18); //turn off laser2
            s = HUB;
		usleep(2000);
            break;
        case BLINK_DONE:
		printf("good");
            gpiolib_write_reg(gpio, GPSET(0), 1 << 18); //turn on laser 1
            gpiolib_write_reg(gpio, GPSET(0), 1 << 15); //turn on laser 2
            usleep(2000);
            gpiolib_write_reg(gpio, GPCLR(0), 1 << 18); //turn off laser 1
            gpiolib_write_reg(gpio, GPCLR(0), 1 << 15); //turn off laser 2
		usleep(2000);
            write(watchdog, "V", 1); //writes "V" to watchdog file to disable watchdog, prevents system resets
            close(watchdog); //closes watchdog file
            s = DONE;
            break;
        case DONE:
            break;
        }
    }
}

int main(const int argc, const char* const argv[])
{
	//Create a string to store program name
	const char* argName = argv[0];

	//Variables used to determine length of program name
	int i = 0;
	int namelength = 0;

	while(argName[i] != 0)
	{
		namelength++;
		i++;
	} 

	char programName[namelength];

	i = 0;

	//Copy the name of the program without the ./ at the start
	//of argv[0]
	while(argName[i + 2] != 0)
	{
		programName[i] = argName[i + 2];
		i++;
	} 	

	// Create file pointer to config file, set to read configureMcConfigureFace
	FILE* configFile;
	configFile = fopen("/home/pi/configureMcConfigureFace.cfg", "r");
   
    /*
    This section of the code gets info from the
    config file to configure the program
    */
   	int timeout = 0;
	char logFileName[50];
	int CaesarShift = 0;
	
	// readConfig function called to read from config file
	readConfig(configFile, &timeout, logFileName, &CaesarShift);

    //Check that the file opens properly.
	if(!configFile)
	{
		perror("The config file could not be opened");
		return -1;
	}
    
	/*Create new file pointer to point to logFile specified by
	config file, appends to fiel when it writes to it.*/
	// if logFile does not exist, creates new logFile
	FILE* logFile;
	logFile = fopen(logFileName, "a");


    /* 
    This section of the code initializes the GPIO
    stuff. Makes the lasers ready to lase and the
    photodiodes ready to photodiode.
    */

    GPIO_Handle gpio;
    gpio =  initializeGPIO(logFile, programName);//turn on gpio
    setToOutput(gpio,17, logFile, programName);
    //setToOutput(gpio,18);
    /*****************************************************/

    /* 
    This section of the code reads the file
    converts it into an int array containing the ASCII
    values of the contents of the file. 
    */

    FILE *sendMsgFile; //message file pointer

    sendMsgFile = fopen("file.txt", "r"); //set value of pointer to point to input file; will be reading from the file

    char inString[200]; //create array to store chars from input file

    fgets(inString, 999, sendMsgFile); //using fgets to read from file, write contents into array

    int len = strlen(inString); //getting length of resulting string

    int input[200] = {0}; //new array to store ASCII values of string

    for (int i = 0; i < len; i++) //loop to typecast char to int and write to array
    {
        input[i] = (int)(inString[i]);
        printf("%c", inString[i]);
    }

    for (int k = 0; k < len; k++)
    {
	printf("1");
        input[k] = encode(input[k], CaesarShift); //encoding via encode function
	
    }

    /***********************************************************************/

    /* 
    This section of the code waits
    for ten seconds to accomodate
    the delay implemented into the
    other system.
    The purpose of this is to allow
    the user to properly align the
    laser and photodiode to ensure 
    a consistent and reliable connection.
*/

    // ****** Need to start laser here to aid alignment *****

    //how long the delay will be in secs. Implemented as int to allow for quick changing.
    //should be set higher than that of the receiving system to accomodate for inaccuracy of usleep. Sending can start after receiving has begun, but the opposite is untrue.
    
    int delay = 11;

    int i = 1; //will set to '0' to break loop

    int timer = 0; // timer. Not using gettime methods because no need for precise timing here

    /*while (i)
    {
        if (timer >= 1000000 * delay)
        {
            i = 0; //break while loop after duration has elapsed by setting i to 0;
        }
        usleep(1000); //this will act as a (rough) timer. Make sure argument of usleep is equal to how much timer is incremented by
        timer += 1000;
    }
    printf("hi");
    */for (int j = 0; j < len; j++) //loop to send laser pulses using "Send" function
    {
	printf("%d\n", input[j]);
        Send(gpio, input[j], configFile, logFile, programName, timeout);
    }

    return 0;
}

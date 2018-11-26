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
    This section of the code waits
    for ten seconds and prints out
    the status of the photodiode.
    The purpose of this is to allow
    the user to properly align the
    laser and photodiode to ensure 
    a consistent and reliable connection.
*/
    //how long the delay will be in secs. Implemented as int to allow for quick changing
    int delay = 10;

    //will set to '0' to break loop
    int i = 1;
    
    // timer. Not using gettime methods because no need for precise timing here
    int timer = 0;

    //Counter count how many consecutive cycles the laser is aligned; once past a threshold a message will be displayed
    int consecutiveLaser = 0;
	
    while (i)
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
    }
    printf("hi");
}
    
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
    for ten seconds to accomodate
    the delay implemented into the
    other system.
    The purpose of this is to allow
    the user to properly align the
    laser and photodiode to ensure 
    a consistent and reliable connection.
*/
    //how long the delay will be in secs. Implemented as int to allow for quick changing.
    //should be set higher than that of the receiving system to accomodate for inaccuracy of usleep. Sending can start after receiving has begun, but the opposite is untrue.
    int delay = 11;

    //will set to '0' to break loop
    int i = 1;
    
    // timer. Not using gettime methods because no need for precise timing here
    int timer = 0;
	
    while (i)
	{
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
    
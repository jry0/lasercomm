#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>
enum State {HUB, BLINK1, BLINK2,  BLINK_DONE, DONE};
void Send(GPIO_Handle gpio, int ascii){
	int laser2 = 0;
	int laser1 = 0;
	if(ascii == 32){
		laser2 = 1;//space
	}else if(65 <= ascii && ascii <= 124){
		laser1 = ascii - 64;//letter
	}else{
		laser2 = 2;//done
	}
	enum State s = HUB;
	while(s != DONE){
		switch(s){
			case HUB:
				if(laser1 == 0 &&  laser2 == 0){
					s = BLINK_DONE;
					break;
				}else if(laser2 != 0 && laser1 == 0){
					s = BLINK2;
					break;
				}else{
					s = BLINK1;
					break;
				}
			case BLINK1:
				//turn on laser1
				usleep(200);
				laser1--;
				//turn off laser1
				s = HUB;
				break;
			case BLINK2:
				//turn on laser2
				usleep(200);
				laser2--;
				//turn off laser2
				s = HUB;
				break;
			case BLINK_DONE:
				//turn on both lasers
				usleep(200);
				//turn off both lasers
				s = DONE;
				break;
			case DONE:
				break;
		}
	}
}
//readletter

/*
 * read_dualshock4.c
 * 
 * Copyright 2018  <Anish Mampetta>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 *  
 * PS4 Pin assignment: 
 * Joystick (Wireless Controller) has 8 axes (X, Y, Z, Rx, Ry, Rz, Hat0X, Hat0Y)
 * and 13 buttons (BtnA, BtnB, BtnX, BtnY, BtnTL, BtnTR, BtnTL2, BtnTR2, BtnSelect, BtnStart, BtnMode, BtnThumbL, BtnThumbR).
 * Note: use this command to get the pig assigment $ jstest /dev/input/js0

	Control / Action 			Axis index		Button index
	Left stick left / right 		0			---	
	Left stick up / down			1			---
	Left stick push in (L3)			---			11
	Right stick left / right		3			---
	Right stick up / down			4			--- 
	Right stick push in (R3)		---			12
	Left Circle up / down			7			---
	Left Circle right / left		6			---
	Square							---			3
	Triangle						---			2
	Circle							---			1
	Cross							---			0
	L1								---			4
	L2								2			6
	R1								---			5
	R2								5			7
	PS (logo)						--- 		10
	OPTIONS							---			9
	SHARE							---			8

* Use this command to figure out the button assignment: $ jstest /dev/input/js0
* 
* REFERENCES:
* This is a good starting point: https://www.piborg.org/blog/build/rpi-ps3-help
* For trouble connecting PS4 to Pi: https://github.com/RetroPie/RetroPie-Setup/wiki/PS4-Controller   
* To read joystick position to terminal: https://github.com/macunixs/dualshock4-pi
 */

#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "joystick.h"

static int joystick_fd = -1;


int open_joystick()
{
        joystick_fd = open(JOYSTICK_DEVNAME, O_RDONLY | O_NONBLOCK);
        return joystick_fd;
}

int read_joystick_event(struct js_event *jse)
{
        int bytes;
        bytes = read(joystick_fd, jse, sizeof(*jse)); 
        if (bytes == -1)
                return 0;
        if (bytes == sizeof(*jse))
                return 1;
        printf("Unexpected bytes from joystick:%d\n", bytes);
        return -1;
}

void close_joystick()
{
        close(joystick_fd);
	joystick_fd = -1;
}



int main(int argc, char **argv)
{
	clock_t time;
	int i;
	int ps4_a[8]; //PS4 has 8 axis
	for (i = 0; i < 8; i++)
		ps4_a[i] = 0;
		
	int ps4_b[13]; //PS4 has 13 Buttoms
	for (i = 0; i < 13; i++)
		ps4_b[i] = 0;
		
	struct js_event jse;
	
	open_joystick();
	printf ("\n Joystick id is %d\n", joystick_fd);
	
	
	while (1)
	{
		time = clock();
		
		// If Joystick cannot be accessed
		if (access(JOYSTICK_DEVNAME, F_OK) == -1)
		{	
			close_joystick();
			for (i = 0; i < 8; i++)
				ps4_a[i] = 0;
			printf("Cannot reach joystick\n");	
		}
		
		////Else If joystick is reachable, open joystick
        else if (access(JOYSTICK_DEVNAME, F_OK) == 0)
		{	
				
			if (joystick_fd == -1)
				open_joystick();
					        	        
			//If joystick state chagnes, capture value		        	        
			if (read_joystick_event(&jse) == 1)
			{
				//Read buttons
				if ((jse.type == 1) && (jse.number >= 0) && (jse.number < 13))
				{
					ps4_b[jse.number] = jse.value;
				}
				
				//Read axes
				if ((jse.type == 2) && (jse.number >= 0) && (jse.number < 8))
				{
					ps4_a[jse.number] = jse.value;
				}
				
				printf("%16ld, %8hd, %8hd, %8hd, %8hd", time, ps4_a[0], ps4_a[1], ps4_a[3], ps4_a[4]);
				printf("%8hd, %8hd, %8hd, %8hd\n", ps4_b[0], ps4_b[1], ps4_b[2], ps4_b[3]);		
				
				// How to pring like jstest /dev/input/js0?
					
			}
		}
	}

	return 0;
}


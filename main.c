/* Copyright (c) 2021
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
#
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
#
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
#
* Authored by: Kris Henriksen <krishenriksen.work@gmail.com>
* Modified by : JohnIrvine1433 for ThemeMaster
#
* ThemeMaster-Joypad
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <libevdev-1.0/libevdev/libevdev.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#include "parser.h"

struct libevdev* dev_joypad;
int fd_ev_joypad;
int rc_joypad;
struct input_event ev_joypad;

static int uinp_fd = -1;
struct uinput_user_dev uidev;

int debug = 0;
char quit_command[100];

// Define variables to be used for key codes
int back_key;
int start_key;
int a_key;
int b_key;
int x_key;
int y_key;
int up_key;
int down_key;
int left_key;
int right_key;
int l1_key;
int l2_key;
int r1_key;
int r2_key;

// Define variables to be used for libevdev key codes
// int lib_back_key;
// int lib_start_key;
// int lib_a_key;
// int lib_b_key;
// int lib_x_key;
// int lib_y_key;
// int lib_up_key;
// int lib_down_key;
// int lib_left_key;
// int lib_right_key;
// int lib_l1_key;
// int lib_l2_key;
// int lib_r1_key;
// int lib_r2_key;

// Set key names to be used for ThemeMaster
short back = KEY_ESC;
short start = KEY_ENTER;
short a = KEY_ENTER;
short b = KEY_SPACE;
short x = KEY_Q;
short y = KEY_F;
short up = KEY_UP;
short down = KEY_DOWN;
short left = KEY_LEFT;
short right = KEY_RIGHT;
short l1 = KEY_SPACE;
short l2 = KEY_SPACE;
short r1 = KEY_ENTER;
short r2 = KEY_ENTER;

int quit = 0;

void emit(int type, int code, int val) {
   struct input_event ev;

   ev.type = type;
   ev.code = code;
   ev.value = val;
   ev.time.tv_sec = 0;
   ev.time.tv_usec = 0;

   write(uinp_fd, &ev, sizeof(ev));
}

// void libkeycodes() {
//   unsigned int event_type = EV_KEY;
//   //based on Linux Gamepad Specification
//
//   lib_back_key = libevdev_event_code_from_name(event_type, "BTN_SELECT");
//   lib_start_key = libevdev_event_code_from_name(event_type, "BTN_START");
//   lib_a_key = libevdev_event_code_from_name(event_type, "BTN_A");
//   //a_key = libevdev_event_code_from_name(event_type, "BTN_EAST");
//   lib_b_key = libevdev_event_code_from_name(event_type, "BTN_B");
//   //b_key = libevdev_event_code_from_name(event_type, "BTN_SOUTH");
//   lib_x_key = libevdev_event_code_from_name(event_type, "BTN_X");
//   //x_key = libevdev_event_code_from_name(event_type, "BTN_NORTH");
//   lib_y_key = libevdev_event_code_from_name(event_type, "BTN_Y");
//   //y_key = libevdev_event_code_from_name(event_type, "BTN_SOUTH");
//   lib_up_key = libevdev_event_code_from_name(event_type, "BTN_DPAD_UP");
//   lib_down_key = libevdev_event_code_from_name(event_type, "BTN_DPAD_DOWN");
//   lib_left_key = libevdev_event_code_from_name(event_type, "BTN_DPAD_LEFT");
//   lib_right_key = libevdev_event_code_from_name(event_type, "BTN_DPAD_RIGHT");
//   lib_l1_key = libevdev_event_code_from_name(event_type, "BTN_TL");
//   lib_l2_key = libevdev_event_code_from_name(event_type, "BTN_TL2");
//   lib_r1_key = libevdev_event_code_from_name(event_type, "BTN_TR");
//   lib_r2_key = libevdev_event_code_from_name(event_type, "BTN_TR2");
// }

//handle event for anbernic devices (except from rg552)
void handle_event_anbernic(int type, int code, int value) {
  if (type == 1) {
		if (code == back_key && value == 1) {
			quit = 0;
			emit(EV_KEY, back, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == back_key && value == 0) {
			emit(EV_KEY, back, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == back_key && value == 2) {
			quit = 1;
		}

		if (code == start_key && value == 1) {
			if (quit == 1) {
				system(quit_command);
				exit(0);
			}
			emit(EV_KEY, start, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == start_key && value == 0) {
			emit(EV_KEY, start, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == a_key && (value == 1 || value == 2)) {
			emit(EV_KEY, a, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == a_key && value == 0) {
			emit(EV_KEY, a, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == b_key && (value == 1 || value == 2)) {
			emit(EV_KEY, b, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == b_key && value == 0) {
			emit(EV_KEY, b, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == x_key && (value == 1 || value == 2)) {
			emit(EV_KEY, x, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == x_key && value == 0) {
			emit(EV_KEY, x, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == y_key && (value == 1 || value == 2)) {
			emit(EV_KEY, y, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == y_key && value == 0) {
			emit(EV_KEY, y, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == l1_key && (value == 1 || value == 2)) {
			emit(EV_KEY, l1, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == l1_key && value == 0) {
			emit(EV_KEY, l1, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == l2_key && (value == 1 || value == 2)) {
			emit(EV_KEY, l2, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == l2_key && value == 0) {
			emit(EV_KEY, l2, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == r1_key && (value == 1 || value == 2)) {
			emit(EV_KEY, r1, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == r1_key && value == 0) {
			emit(EV_KEY, r1, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == r2_key && (value == 1 || value == 2)) {
			emit(EV_KEY, r2, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == r2_key && value == 0) {
			emit(EV_KEY, r2, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}
	}

	// d-pad
	if (type == 3) {
		if (code == up_key && value == -1) {
			emit(EV_KEY, up, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == up_key && value == 0) {
			emit(EV_KEY, up, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == down_key && value == 1) {
			emit(EV_KEY, down, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == down_key && value == 0) {
			emit(EV_KEY, down, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == left_key && value == -1) {
			emit(EV_KEY, left, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == left_key && value == 0) {
			emit(EV_KEY, left, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == right_key && value == 1) {
			emit(EV_KEY, right, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == right_key && value == 0) {
			emit(EV_KEY, right, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}
	}
  if (debug) {
    printf("type:%d code: %d value: %d\n", type, code, value);
    fflush(stdout);
  }
}

//handle event for oga, ogs, rk2020, rg552 and chi devices
void handle_event_ogx(int type, int code, int value) {
  if (type == 1) {
		if (code == back_key && value == 1) {
			quit = 0;
			emit(EV_KEY, back, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == back_key && value == 0) {
			emit(EV_KEY, back, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == back_key && value == 2) {
			quit = 1;
		}

		if (code == start_key && value == 1) {
			if (quit == 1) {
				system(quit_command);
				exit(0);
			}
			emit(EV_KEY, start, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == start_key && value == 0) {
			emit(EV_KEY, start, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == a_key && (value == 1 || value == 2)) {
			emit(EV_KEY, a, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == a_key && value == 0) {
			emit(EV_KEY, a, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == b_key && (value == 1 || value == 2)) {
			emit(EV_KEY, b, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == b_key && value == 0) {
			emit(EV_KEY, b, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == x_key && (value == 1 || value == 2)) {
			emit(EV_KEY, x, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == x_key && value == 0) {
			emit(EV_KEY, x, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == y_key && (value == 1 || value == 2)) {
			emit(EV_KEY, y, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == y_key && value == 0) {
			emit(EV_KEY, y, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == l1_key && (value == 1 || value == 2)) {
			emit(EV_KEY, l1, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == l1_key && value == 0) {
			emit(EV_KEY, l1, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == l2_key && (value == 1 || value == 2)) {
			emit(EV_KEY, l2, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == l2_key && value == 0) {
			emit(EV_KEY, l2, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == r1_key && (value == 1 || value == 2)) {
			emit(EV_KEY, r1, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == r1_key && value == 0) {
			emit(EV_KEY, r1, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == r2_key && (value == 1 || value == 2)) {
			emit(EV_KEY, r2, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == r2_key && value == 0) {
			emit(EV_KEY, r2, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

	// d-pad
		if (code == up_key && value == 1) {
			emit(EV_KEY, up, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == up_key && value == 0) {
			emit(EV_KEY, up, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == down_key && value == 1) {
			emit(EV_KEY, down, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == down_key && value == 0) {
			emit(EV_KEY, down, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == left_key && value == 1) {
			emit(EV_KEY, left, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == left_key && value == 0) {
			emit(EV_KEY, left, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}

		if (code == right_key && value == 1) {
			emit(EV_KEY, right, 1);
			emit(EV_SYN, SYN_REPORT, 0);
		}
		else if (code == right_key && value == 0) {
			emit(EV_KEY, right, 0);
			emit(EV_SYN, SYN_REPORT, 0);
		}
	}
  if (debug) {
    printf("type:%d code: %d value: %d\n", type, code, value);
    fflush(stdout);
  }
}

int main(int argc, char* argv[]) {
  char inputstr[100];

	// command line arguments
  // first argument to be script name in order to manage forced exit
  // second argument to be the name of gamepad
  // argv[0] = controls
  // argv[1] = ThemeMaster
  // argv[2] = (oga, ogs, anbernic, chi, rg552)
  // argv[3] = debug (optional)

  // first 2 arguments required
	if (argc >= 3) {
    strcpy(quit_command, "pgrep -f ");
    strcat(quit_command, argv[1]);
    strcat(quit_command, " | xargs kill -9");

    if (argc == 4) {
      if (strcmp(argv[3], "debug") == 0) {
  	     debug = 1;
         freopen("./debug/tm_joypad.log","a+",stdout);
         printf("**** INIT DEBUG MODE ****\n");
  	  }
    }

    if (strcmp(argv[2], "anbernic") == 0) {
      //devices confirmed : rg351 p/m/v
      back_key = 311;
      start_key = 310;
      a_key = 304;
      b_key = 305;
      x_key = 306;
      y_key = 307;
      up_key = 17;
      down_key = 17;
      left_key = 16;
      right_key = 16;
      l1_key = 308;
      l2_key = 314;
      r1_key = 309;
      r2_key = 315;
      strcpy(inputstr, "/dev/input/by-path/platform-ff300000.usb-usb-0:1.2:1.0-event-joystick");
    }
    else if (strcmp(argv[2], "rg552") == 0) {
      back_key = 314;
      start_key = 315;
      a_key = 305;
      b_key = 304;
      x_key = 307;
      y_key = 308;
      up_key = 544;
      down_key = 545;
      left_key = 546;
      right_key = 547;
      l1_key = 310;
      l2_key = 312;
      r1_key = 311;
      r2_key = 313;
      strcpy(inputstr, "/dev/input/by-path/platform-singleadc-joypad-event-joystick");
    }
	  else if (strcmp(argv[2], "oga") == 0) {
      //devices confirmed : rk2020, rgb10
      back_key = 704;
      start_key = 709;
      a_key = 305;
      b_key = 304;
      x_key = 307;
      y_key = 308;
      up_key = 544;
      down_key = 545;
      left_key = 546;
      right_key = 547;
      l1_key = 310;
      l2_key = 706;
      r1_key = 311;
      r2_key = 707;
      strcpy(inputstr, "/dev/input/by-path/platform-odroidgo2-joypad-event-joystick");
	  }
    else if (strcmp(argv[2], "ogs") == 0) {
      back_key = 704;
      start_key = 705;
      a_key = 305;
      b_key = 304;
      x_key = 307;
      y_key = 308;
      up_key = 544;
      down_key = 545;
      left_key = 546;
      right_key = 547;
      l1_key = 310;
      l2_key = 311;
      r1_key = 312;
      r2_key = 313;
      strcpy(inputstr, "/dev/input/by-path/platform-odroidgo3-joypad-event-joystick");
    }
    else if (strcmp(argv[2], "chi") == 0) {
      back_key = 314;
      start_key = 315;
      a_key = 305;
      b_key = 304;
      x_key = 307;
      y_key = 308;
      up_key = 544;
      down_key = 545;
      left_key = 546;
      right_key = 547;
      l1_key = 310;
      l2_key = 312;
      r1_key = 311;
      r2_key = 313;
      strcpy(inputstr, "/dev/input/by-path/platform-gameforce-gamepad-event-joystick");
    }
    else if (strcmp(argv[2], "x55") == 0) {
      back_key = 704;
      start_key = 709;
      a_key = 304;
      b_key = 305;
      x_key = 307;
      y_key = 308;
      up_key = 544;
      down_key = 545;
      left_key = 546;
      right_key = 547;
      l1_key = 310;
      l2_key = 312;
      r1_key = 311;
      r2_key = 313;
      strcpy(inputstr, "/dev/input/by-path/platform-zed_joystick-event-joystick");
    }
    else {
      printf("Error lauching, unrecognised parameters\n");
  		exit(0);
		}
  }
  else {
    printf("Error lauching, missing required parameters\n");
		exit(0);
	}

	uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (uinp_fd < 0) {
		printf("Unable to open /dev/uinput\n");
		return -1;
	}

	// Intialize the uInput device to NULL
	memset(&uidev, 0, sizeof(uidev));

	strncpy(uidev.name, "ThemeMaster Joypad", UINPUT_MAX_NAME_SIZE);
	uidev.id.version = 1;
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 0x1234; /* sample vendor */
	uidev.id.product = 0x5678; /* sample product */

	for (int i = 0; i < 256; i++) {
		ioctl(uinp_fd, UI_SET_KEYBIT, i);
	}

	// Keys or Buttons
	ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);

	// Create input device into input sub-system
	write(uinp_fd, &uidev, sizeof(uidev));

	if (ioctl(uinp_fd, UI_DEV_CREATE)) {
		printf("Unable to create UINPUT device\n");
		return -1;
	}

	fd_ev_joypad = open(inputstr, O_RDONLY|O_NONBLOCK);
	rc_joypad = libevdev_new_from_fd(fd_ev_joypad, &dev_joypad);

	do {
		rc_joypad = libevdev_next_event(dev_joypad, LIBEVDEV_READ_FLAG_NORMAL, &ev_joypad);

		if (rc_joypad == LIBEVDEV_READ_STATUS_SUCCESS) {
      if (strcmp(argv[2], "anbernic") == 0) {
        handle_event_anbernic(ev_joypad.type, ev_joypad.code, ev_joypad.value);
      }
      else {
        // argv[2] = "rg552", "oga", "ogs", "chi", "x55"
        handle_event_ogx(ev_joypad.type, ev_joypad.code, ev_joypad.value);
      }
		}

		sleep(0.0001);
	} while (rc_joypad == LIBEVDEV_READ_STATUS_SYNC || rc_joypad == LIBEVDEV_READ_STATUS_SUCCESS || rc_joypad == -EAGAIN);

	libevdev_free(dev_joypad);
	close(fd_ev_joypad);

	/*
	* Give userspace some time to read the events before we destroy the
	* device with UI_DEV_DESTROY.
	*/
	sleep(1);

	/* Clean up */
	ioctl(uinp_fd, UI_DEV_DESTROY);
	close(uinp_fd);
  fclose(stdout);
	return 0;
}

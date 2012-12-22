/*****************************************************
 Copyright (c) 2012, Vitaly Lipatov <lav@etersoft.ru>
 Public domain
******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/input.h>
#include <fcntl.h>
#include <signal.h>

#define EVENTS 10
#define HZ_LIST 64

typedef struct event_s {
	int fd;
	int hz[HZ_LIST];
	int count;
	double prvtime;
} event_t;

int quit = 0;

void sigint() {
	signal(SIGINT, sigint);
	quit = 1;
}

void calc_event(event_t *evi, int evnum)
{
	int bytes;
	struct input_event event;
	double time;
	int hz;
	int j;
	int avghz;

	bytes = read(evi->fd, &event, sizeof(event));

	if(bytes != sizeof(event))
		return;

	if(event.type != EV_REL)
		return;

	time = event.time.tv_sec * 1000 + event.time.tv_usec / 1000;
	hz = 1000 / (time - evi->prvtime);
	evi->prvtime = time;

	if(hz <= 0)
		return;

	evi->count++;
	evi->hz[evi->count & (HZ_LIST - 1)] = hz;

	avghz = 0;
	for(j = 0; j < HZ_LIST; j++) {
		avghz += evi->hz[j];
	}

	avghz /= (evi->count > HZ_LIST) ? HZ_LIST : evi->count;

	printf("event%i: latest hz = %i (average hz = %i)\n", evnum, hz, avghz);
}


int main(int argc, char **argv) {
	int i;
	int has_input = 0;
	event_t events[EVENTS];

	memset(events, 0, sizeof(events));

	for(i = 0; i < EVENTS; i++) {
		char device[18];
		char name[128];

		sprintf(device, "/dev/input/event%i", i);
		events[i].fd = open(device, O_RDONLY);
		
		if(events[i].fd == -1)
			continue;

		ioctl(events[i].fd, EVIOCGNAME(sizeof(name)), name);
		printf("event%i: %s\n", i, name);
		has_input = 1;
	}

	if (!has_input) {
		printf("There are no accessible /dev/input/event* devices\n");
		exit(1);
	}

	signal(SIGINT, sigint);
	printf("Press CTRL-C to exit.\n\n");

	while(!quit) {
		fd_set set;
		FD_ZERO(&set);

		for(i = 0; i < EVENTS; i++) {
			if(events[i].fd != -1) {
				FD_SET(events[i].fd, &set);
			}
		}

		if(select(FD_SETSIZE, &set, NULL, NULL, NULL) <= 0)
			continue;

		for(i = 0; i < EVENTS; i++) {
			if(events[i].fd == -1) {
				continue;
			}

			if(!FD_ISSET(events[i].fd, &set)) {
				continue;
			}

			calc_event(&events[i], i);
		}
	}

	for(i = 0; i < EVENTS; i++) {
		if(events[i].fd != -1) {
			close(events[i].fd);
		}
	}
}

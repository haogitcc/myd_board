#ifndef _GPIO_INIT_H_
#define _GPIO_INIT_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>

#define MYD // use myd y6ul-y2
#ifdef MYD
#define GPO_04 4
#define GPO_03 3
#define GPI_02 2
#define GPI_01 1

#define GPO04_DEVICE "/sys/class/gpio/gpio4/value"
#define GPO03_DEVICE "/sys/class/gpio/gpio3/value"
#define GPI02_DEVICE "/sys/class/gpio/gpio2/value"
#define GPO01_DEVICE "/sys/class/gpio/gpio1/value"

#else
#define GPO_04 60 //28
#define GPO_03 61 //29
#define GPI_02 62 //30
#define GPI_01 63 //31

#define GPO04_DEVICE "/sys/class/gpio/gpio60/value" //28
#define GPO03_DEVICE "/sys/class/gpio/gpio61/value" //29
#define GPO02_DEVICE "/sys/class/gpio/gpio62/value" //30
#define GPO01_DEVICE "/sys/class/gpio/gpio63/value" //31

#endif

void gpio_init();
int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int direction);
int gpio_write(int pin, int value);
int gpio_read(int pin);
int gpio_edge(int pin, int edge);
int gpio_poll();
void* gpo_write(void* para);
int gpo_open(char *device);
int gpo_close();
int gpio_write_with_timeout(int pin, int timeout);


#endif

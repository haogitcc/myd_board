#ifndef _GPIO_INIT_H_
#define _GPIO_INIT_H_

#define MYD
#ifdef MYD
#define GPO_28 4
#define GPO_29 3
#define GPI_30 2
#define GPI_31 1
//#define GPO1_DEVICE "/sys/class/gpio/gpio108/value"
#define GPO2_DEVICE "/sys/class/gpio/gpio4/value"
#define GPO3_DEVICE "/sys/class/gpio/gpio3/value"
#define GPO4_DEVICE "/sys/class/gpio/gpio2/value"
#define GPO5_DEVICE "/sys/class/gpio/gpio1/value"

#else
#define GPO_28 60
#define GPO_29 61
#define GPI_30 62
#define GPI_31 63

//#define GPO1_DEVICE "/sys/class/gpio/gpio108/value"
#define GPO2_DEVICE "/sys/class/gpio/gpio60/value"
#define GPO3_DEVICE "/sys/class/gpio/gpio61/value"
#define GPO4_DEVICE "/sys/class/gpio/gpio62/value"
#define GPO5_DEVICE "/sys/class/gpio/gpio63/value"

#endif

void gpio_init();
int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int direction);
int gpio_write(int pin, int value);
int gpio_read(int pin);
int gpio_edge(int pin, int edge);
int gpio_poll();
int gpio_write_with_timeout(int pin, int timeout);


#endif

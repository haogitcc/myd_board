#ifndef _READER_H_
#define _READER_H_

#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

//0 disable use wifi
#define USEWIFI 0

#define MYD // use myd y6ul-y2
#ifdef MYD
//MYD demo board
#define DEVICE "/dev/ttymxc1"
#define DEVICE_NAME "tmr:///dev/ttymxc1"
#else
//M28x demo board
#define DEVICE "/dev/ttySP0"
#define DEVICE_NAME "tmr:///dev/ttySP0"
#endif

#endif
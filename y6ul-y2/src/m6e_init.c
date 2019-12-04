#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "mid_task.h"
#include "tm_reader.h"
#include "tmr_utils.h"

TMR_Status ret;
TMR_Reader r,*rp;
TMR_ReadPlan plan;
int antennaCount = 0;
uint8_t *antennaList = NULL;

static pthread_cond_t cond;
static pthread_mutex_t mutex;
void errx(int exitval, const char *fmt, ...);
void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg);



int m6e_init(char* strUri)
{
	plog("m6e_init %s\n", strUri);

	rp = &r;
	ret = TMR_create(rp, strUri);
	checkerr(rp, ret, 1, "creating reader");
	if(ret != TMR_SUCCESS)
	{
		plog("Create device failed!\n");
		return -1;
	}
	plog("create success\n");

	ret = TMR_connect(rp);
	checkerr(rp, ret, 1, "connecting reader");
	if(ret != TMR_SUCCESS)
	{
		plog("Open device failed!\n");
		return -1;
	}
	plog("connect success\n");
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond, NULL);
	return 0;
}

int set_bundrate(int rate)
{
	ret = TMR_paramSet(rp,TMR_PARAM_BAUDRATE,&rate);
	checkerr(rp, ret, 1, "set baudrate");
	return ret;
}

int m6e_destory()
{
   ret = TMR_destroy(rp);
   checkerr(rp, ret, 1, "destroy reader");
   return ret;
}

int m6e_configuration_init()
{
}


int m6e_set_configuration(int key, int value)
{
	ret = TMR_paramSet(rp, key, &value);
	checkerr(rp, ret, 1, "set config");
}

void errx(int exitval, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);

  exit(exitval);
}

void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg)
{
  if (TMR_SUCCESS != ret)
  {
    errx(exitval, "Error %s: %s\n", msg, TMR_strerr(rp, ret));
  }
}


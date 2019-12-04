#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include "gpio_init.h"
#include "app_sys_setting.h"

static struct timeval tv;

void gpio_init()
{
    plog("[%s, %s, %d]", __FILE__, __FUNCTION__, __LINE__);
	gpio_export(GPO_28);
	gpio_direction(GPO_28, 1);
	gpio_write(GPO_28, 0);

	gpio_export(GPO_29);
	gpio_direction(GPO_29, 1);
	gpio_write(GPO_29, 1);

	gpio_export(GPI_30);
	gpio_direction(GPI_30, 0);
	//gpio_edge(GPI_30, 1);
	
	gpio_export(GPI_31);
	gpio_direction(GPI_31, 0);
	//gpio_edge(GPI_31, 1);

	plog("[%s, %d] gpio_init success\n", __FILE__, __LINE__);
}

int gpio_export(int pin)
{
    plog("[%s, %s, %d] pin=%d", __FILE__, __FUNCTION__, __LINE__, pin);
	char buffer[64];
	int len;
	int fd;

	fd = open("/sys/class/gpio/export" ,O_WRONLY);
	if(fd < 0) {
		plog("Failed to open export for writing!\n");
		return -1;
	}

	len = snprintf(buffer, sizeof(buffer), "%d", pin);
	if(write(fd, buffer, len) < 0) {
		plog("Failed to export gpio!\n");
	}

	close(fd);
	return 0;
}

int gpio_unexport(int pin)
{
    plog("[%s, %s, %d] pin=%d", __FILE__, __FUNCTION__, __LINE__, pin);
	char buffer[64];
	int len;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(fd < 0) {
		plog("Failed to open unexport for writing!\n");
		return -1;
	}

	len = snprintf(buffer, sizeof(buffer), "%d", pin);
	if(write(fd, buffer, len) < 0) {
		plog("Failed to unexport gpio!\n");
	}

	close(fd);
	return 0;
}

int gpio_direction(int pin, int direction)
{
	plog("[%s, %s, %d] pin=%d, direction=%s", __FILE__, __FUNCTION__, __LINE__, pin, (direction==1?"out":"in"));
	const char dir_str[] = "in\0out";
	char path[64];
	int fd;

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction",pin);
	fd = open(path, O_WRONLY);
	if(fd < 0 ){
		plog("Failed to open gpio direction for writing!\n");
		return -1;
	}

	if(write(fd, &dir_str[direction == 0? 0 : 3], direction == 0? 2 :3) < 0) {
		plog("Failed to set direction!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int gpio_write(int pin, int value)
{
    plog("[%s, %s, %d] pin=%d, value=%d", __FILE__, __FUNCTION__, __LINE__, pin, value);
	const char dir_str[] = "01";
	char path[64];
	int fd;

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value",pin);
	fd = open(path, O_WRONLY);
	if(fd < 0 ){
		plog("Failed to open gpio value for writing!\n");
		return -1;
	}

	if(write(fd, &dir_str[value == 0? 0 : 1], 1) < 0) {
		plog("Failed to write value!\n");
		return -1;
	}

	close(fd);
	return 0;
}

int gpio_write_with_timeout(int pin, int timeout)
{
	plog("[%s, %s, %d] pin=%d, timeout=%d", __FILE__, __FUNCTION__, __LINE__, pin, timeout);
    gpio_write(pin, 1);
	setTimer(0, timeout);
	gpio_write(pin, 0);
	return 0;
}


int gpio_read(int pin)
{
    plog("[%s, %s, %d] pin=%d", __FILE__, __FUNCTION__, __LINE__, pin);
	char value_str[3];
	char path[64];
	int fd;

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value",pin);
	fd = open(path, O_RDONLY);
	if(fd < 0 ){
		plog("Failed to open gpio value for writing!\n");
		return -1;
	}

	if(read(fd, value_str, 3) < 0) {
		plog("Failed to read value!\n");
		return -1;
	}

	close(fd);
	return atoi(value_str);
}

int gpio_edge(int pin, int edge)
{
	plog("[%s, %s, %d] pin=%d, edge=%d", __FILE__, __FUNCTION__, __LINE__, pin, edge);
	const char dir_str[] = "none\0rising\0falling\0both";

	int ptr;
	char path[64];
	int fd;

	switch(edge) {
		case 0:
			ptr = 0;
			break;
		case 1:
			ptr = 5;
			break;
		case 2:
			ptr = 12;
			break;
		case 3:
			ptr = 20;
			break;
		default:
			ptr = 0;
	}

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);
	fd = open(path, O_WRONLY);
	if(fd < 0) {
		plog("Failed to open gpio edge for writing!\n");
		return -1;
	}

	if(write(fd, &dir_str[ptr], strlen(&dir_str[ptr])) < 0) {
		plog("Failed to set edge!\n");
		return -1;
	}

	close(fd);
	return 0;
}

void sysUsecTime()
{
	plog("[%s, %s, %d]", __FILE__, __FUNCTION__, __LINE__);
	struct timeval tv;
	gettimeofday(&tv, NULL);
}

int compare_time()
{
	plog("[%s, %s, %d]", __FILE__, __FUNCTION__, __LINE__);
	struct timeval tv2;
	gettimeofday(&tv2, NULL);
	plog("paper 1 : %ld , %ld\n", tv.tv_sec, tv.tv_usec/1000);
	plog("paper 2 : %ld , %ld\n", tv2.tv_sec, tv2.tv_usec/1000);
	int time;  
	if(tv2.tv_usec > tv.tv_usec) {
		time =  (tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec)/1000; 
	} else {
		time = (tv2.tv_sec - tv.tv_sec) * 1000 - (tv.tv_usec - tv2.tv_usec)/1000; 
	}
	return time;
}

/*int gpio_poll()
{
	plog("[%s, %s, %d]", __FILE__, __FUNCTION__, __LINE__);
	int gpio_fd, ret,count;
	struct pollfd fds[1];
	char buff[10];
	gpio_fd = open("/sys/class/gpio/gpio62/value", O_RDONLY);
	if(gpio_fd< 0) {
		plog("Failed to open value!\n");
		return -1;
	}

	gettimeofday(&tv, NULL);

	fds[0].fd = gpio_fd;
	fds[0].events = POLLPRI;
	ret = read(gpio_fd, buff, 10);
	if(ret == -1)
		plog("read!\n");

	while(1) {
		ret = poll(fds,1,0);
		if(ret == -1)
			plog("poll!\n");
		if(fds[0].revents & POLLPRI) {
			int time = compare_time();
			if(time < 400)
				continue;
			gettimeofday(&tv, NULL);
			
			ret = lseek(gpio_fd, 0, SEEK_SET);
			if(ret == -1)
				plog("lseek!\n");
			ret = read(gpio_fd, buff, 10);
			if(ret == -1)
				plog("read!\n");
			//plog("buff = %d\n", atoi(buff));
			sysSettingGetInt("count", &count, 1);
			count++;
			sysSettingSetInt("count", count);
			sys_config_timer();
		}

		//usleep(50000);
	}
}*/

void setTimer(int seconds, int mseconds)
{
	plog("[%s, %s, %d] %ds %dms", __FILE__, __FUNCTION__, __LINE__, seconds, mseconds);
	struct timeval temp;
	temp.tv_sec = seconds;
	temp.tv_usec = mseconds;
	select(0,NULL,NULL,NULL,&temp);
	return;
}

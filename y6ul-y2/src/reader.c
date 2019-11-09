#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>


#include "app_sys_setting.h"
#include "mid_timer.h"
#include "mid_task.h"
#include "mid_telnet.h"
#include "mid_net.h"
#include "serial_m6e.h"
#include "server_m6e.h"
#include "m6e_init.h"
#include "gpio_init.h"

//#define DEVICE "/dev/ttySP0"
#define DEVICE "/dev/ttymxc1"		//ttymxc1 对应与M6E的串口

#define DEVICE_NAME "tmr:///dev/ttySP0"

void handle_pipe(int sig)
{

}

void *interrupt()
{
  	struct sigaction action;
  	action.sa_handler = handle_pipe;
  	sigemptyset(&action.sa_mask);
  	action.sa_flags = 0;
  	sigaction(SIGPIPE, &action, NULL);
	return NULL;
}

int main(int argc, char **argv)
{
	int ret = -1;
	interrupt();
  	sys_config_init();  
  	sys_config_load(0);
	mid_task_init();	
	mid_timer_init();

	mid_connect();	//ip设置 在y2无效

	telnetd_init(1);
	shell_play_init();

	//gpio_init();
	
	/*
	//if(0 == m6e_init("tmr:///dev/ttySP0")) //ttymxc0
	if(0 == m6e_init("tmr:///dev/ttymxc0"))
	{
		m6e_configuration_init();
		m6e_destory();
	}
	*/

	/*
		FF 04 06 00 07 08 00 0E A6 
		收到数据
	*/
	
	
	//m6e_version(); 先打开串口115200 获取版本号 之后发送460800
	printf("serial_open()\n");
	if(serial_open(DEVICE) < 0)
	{
		printf("error serial_open\n");
		return -1;
	}
	else 
	{
		serial_setBaudRate(115200);		//核心板默认9600波特率 需要先设置115200与M6E上电匹配 之后设置460800
		
		printf("m6e_baudrate()\n");
		m6e_baudrate(460800);	//460800
		//m6e_version();
		serial_flush();
	}
	
	pthread_tag_init();
	m6e_start();
	return 0;
}



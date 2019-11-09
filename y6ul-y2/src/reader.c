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
#define DEVICE "/dev/ttymxc1"		//ttymxc1 ��Ӧ��M6E�Ĵ���

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

	mid_connect();	//ip���� ��y2��Ч

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
		�յ�����
	*/
	
	
	//m6e_version(); �ȴ򿪴���115200 ��ȡ�汾�� ֮����460800
	printf("serial_open()\n");
	if(serial_open(DEVICE) < 0)
	{
		printf("error serial_open\n");
		return -1;
	}
	else 
	{
		serial_setBaudRate(115200);		//���İ�Ĭ��9600������ ��Ҫ������115200��M6E�ϵ�ƥ�� ֮������460800
		
		printf("m6e_baudrate()\n");
		m6e_baudrate(460800);	//460800
		//m6e_version();
		serial_flush();
	}
	
	pthread_tag_init();
	m6e_start();
	return 0;
}



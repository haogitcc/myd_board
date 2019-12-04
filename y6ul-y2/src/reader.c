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
#include "reader.h"

void handle_pipe(int sig)
{
  //do nothing
  plog("### handle_pipe %d\n", sig);
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
	plog("[%s: %s %d]### ver MYD FFFF ###\n", __FILE__, __FUNCTION__, __LINE__);

	int ret = -1;
	interrupt();
  	sys_config_init();  
  	sys_config_load(0);
	mid_task_init();	
	mid_timer_init();

	mid_connect();	//ip���� ��y2��Ч

	telnetd_init(1);
	shell_play_init();

	gpio_init();

	ret = m6e_init(DEVICE_NAME);
	if(ret != 0)
	{
		int times = 0;
		while(times <= 3 && ret != 0)
		{
			plog("[%s: %s %d]re init times= %d, %d\n", __FILE__, __FUNCTION__, __LINE__, ++times, ret);
			ret = m6e_init(DEVICE_NAME);
			plog("re init %d\n", ret);
		}
		if(ret != 0 && times == 4)
		{
			plog("m6e_init and restart failed\n");
			return -1;
		}
	}
	else {
		plog("m6e_init success\n");
		m6e_configuration_init();
		m6e_destory();
	}


	/*
		FF 04 06 00 07 08 00 0E A6 
		�յ�����
	*/
	
	
	//m6e_version(); �ȴ򿪴���115200 ��ȡ�汾�� ֮����460800
	if(serial_open(DEVICE) < 0)
	{
		plog("[%s, %d] error serial_open\n", __FILE__, __LINE__);
		return -1;
	}
	else 
	{
		serial_setBaudRate(115200);		//���İ�Ĭ��9600������ ��Ҫ������115200��M6E�ϵ�ƥ�� ֮������460800
		
		m6e_baudrate(460800);	//460800
		//m6e_version();
		serial_flush();
	}
	
	pthread_tag_init();
	m6e_start();
	return 0;
}



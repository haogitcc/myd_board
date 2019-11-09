
#ifndef __TELNETD_PORT_H__
#define __TELNETD_PORT_H__

#include "mid_task.h"

void telnetd_link_state_display_task_create(mid_func_t entry);
void telnetd_port_task_create(mid_func_t entry, void *arg);

/*
	�˶��˺�

	����ֵ��
		0 �ް�
		1 ��Ч
 */
int telnetd_port_user_ok(char *user);

/*
	�˶�����

	����ֵ��
		0 �ް�
		1 ��Ч
 */
int telnetd_port_pass_ok(char *pass);

enum {
	eTelnet_state_idle = 0,	//����
	eTelnet_state_connect,	//����
	eTelnet_state_session,	//��½�ɹ�����ʽ�Ự
} eTelnet_state;
/*
	telnet״̬�ı�ʱ����ø�ֵ
	state ȡֵ��ΧΪeTelnet_state
 */
void telnetd_port_post_state(int state);

#endif//__TELNETD_PORT_H__



/*******************************************************************************
	��˾��
			Yuxing software
	��¼��
			2008-12-8 11:26:52 create by Liu Jianhua
	ģ�飺
			netwk
	������
			telnetd
 *******************************************************************************/

#ifndef __TELNETD_H__
#define __TELNETD_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TD_ARGC_MAX		5

/*
	argc��
		����������Ҳ���� �������� ��Ԫ�ظ���
	argv��
		�������飬argv[0] Ϊ��1��������argv[1]Ϊ�ڶ�������
 */
typedef int (*td_callfunc)(int argc, char **argv);

//void mid_netwk_buildtime(void);

//#define telnetd_init telnetd_init_v1
int telnetd_init(int active);

void telnetd_active(void);
void telnetd_suspend(void);

/*
	callname��
		�������ƣ�����Ҫ��С��16 ���ɳ��ȶ���һ��ΪСд�ַ���������telnet�����롣
	func��
		�����Ӧ�ĺ���
	argstype:
		 ��'d'��'s'��'e'��ɵĲ��������ַ����� 'd' ���֣�'s' �ַ�����'e' ��β�ַ�������'s'������ͬ����'e'ֻ��Ϊ���һ�����������԰���һЩ�����ַ���
 */
int telnetd_regist(const char *callname, td_callfunc func, const char *argstype);

int telnetd_output(const char *buf, unsigned int len);


/*
����1��iptv��Ƶ���������������ֻ��һ���ַ�������������Ƶ������URL

static int shell_iptv(int argc, char **argv)
{
	mid_stream_open(0, argv[0], APP_TYPE_IPTV, -1);
	return 0;
}
......
telnetd_regist("iptv",			shell_iptv,				"s");
......

����2��rect��������Ƶ�������������ֻ���ĸ����ֲ����������ꡢ�����ꡢ��͸ߡ�
static int shell_rect(int argc, char **argv)
{
	int x, y, w, h;

	x = atoi(argv[0]);
	y = atoi(argv[1]);
	w = atoi(argv[2]);
	h = atoi(argv[3]);

	mid_stream_rect(0, x, y, w, h);

	return 0;
}
......
telnetd_regist("rect",			shell_rect,				"dddd");
......
 */

#ifdef __cplusplus
}
#endif

#endif /* __TELNETD_H__ */

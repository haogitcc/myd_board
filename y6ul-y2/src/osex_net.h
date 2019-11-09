

#ifndef __OSEX_NET_H__
#define __OSEX_NET_H__

int yx_middle_drv_net_get_link_status(char* devname, int *status);


int osex_igmp_version_get(void);               // add by Li Sensen.
int osex_igmp_version_set(const char*, const char*);        // add by Li Sensen.
int osex_ipaddr_get( const char* ifname, char* buf );
int osex_ipaddr_set( const char* ifname, char* ip );
int osex_ipdns_get(char *dns0, char *dns1);
int osex_ipdns_set(char *dns0, char *dns1);
int osex_ipmask_get(char *ifname, char *buf);
int osex_ipmask_set(const char* ifname, char* ip);
int osex_gateway_get(const char *ifname, char *buf);
int osex_netmac_get(const char* ifname, char* mac);
int osex_iproute_add( const char* dst, const char* mask, const char* gw, int metric);

int osex_pppoe_nakmsg(char *msg, int size);
/*
	����padi���ͺ�ȴ�pado�ĳ�ʱʱ��
	��padr���ͺ�ȴ�padsҲ������
*/
int osex_pppoe_set_paditimeout(int time);

/*
	����padi���ʹ�������padrҲ������
*/
int osex_pppoe_set_padinum(int num);

/*
	����PPPOE�������ڣ�Ҳ���ǡ�Echo request������
	����Ϊ0��client������LCP Echo Request��Ҳ���ǵ����ѯ
 */
int osex_pppoe_interval(int interval);

int mid_net4_staticip_connect(void);

#endif//__OSEX_NET_H__

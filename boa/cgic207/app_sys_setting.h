#ifndef _APP_SYS_SETTING_H
#define _APP_SYS_SETTING_H

#define NET_LEN 16
#define WIFI_LEN 64


typedef struct {
    //int	nettype; // The network physical link type. 0 is cable, 1 is wifi.
    char ip[NET_LEN]; // static IP address
    char netmask[NET_LEN]; // static network submask
    char gateway[NET_LEN]; // static network gateway
    char wifi[WIFI_LEN];   //
    char wifiwd[WIFI_LEN]; // 
    char ap[WIFI_LEN];   //
    char apwd[WIFI_LEN]; // 
    int  port;
    int  wifiMode;		//   
}s_IP_SETTINGS;

int sys_config_init(void);
void sys_config_load(int reset);
void sys_config_load_2(int reset,s_IP_SETTINGS* ipInfo);

int sysSettingGetString(const char* name, char* value, int valueLen, int searchFlag);
int sysSettingGetInt(const char* name, int* value, int searchFlag);
int sysSettingSetString(const char* name, const char* value);
int sysSettingSetInt(const char* name, const int value);
int sys_config_save(void);/* sysConfigSave */
int sys_config_timer();

#endif



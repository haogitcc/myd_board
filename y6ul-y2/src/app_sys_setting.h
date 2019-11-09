#ifndef _APP_SYS_SETTING_H
#define _APP_SYS_SETTING_H

int sys_config_init(void);
void sys_config_load(int reset);
int sysSettingGetString(const char* name, char* value, int valueLen, int searchFlag);
int sysSettingGetInt(const char* name, int* value, int searchFlag);
int sysSettingSetString(const char* name, const char* value);
int sysSettingSetInt(const char* name, const int value);
int sys_config_save(void);/* sysConfigSave */
int sys_config_timer();

#endif



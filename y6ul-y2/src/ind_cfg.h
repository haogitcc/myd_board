
#ifndef __IND_CFG_H__
#define __IND_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CfgTree* CfgTree_t;

enum {
	CFG_TYPE_OBJECT = 1,
	CFG_TYPE_INT,
	CFG_TYPE_UINT,
	CFG_TYPE_INT64,
	CFG_TYPE_STRING
};

/*
	����������
 */
CfgTree_t ind_cfg_create(void);

int ind_cfg_inset_object(CfgTree_t tree, char *paramname);
//����
int ind_cfg_inset_int(CfgTree_t tree, char *paramname, int *addr);
//�ַ��� len �ַ�������
int ind_cfg_inset_string(CfgTree_t tree, char *paramname, char *addr, int len);

int ind_cfg_input(CfgTree_t tree, char *rootname, char *buf, int len);

int ind_cfg_output(CfgTree_t tree, char *rootname, char *buf, int len);

/*
	visible
	param����ʱvisibleΪ1��param�������Ը���
	Ϊ 0 ʱ����д�������ļ���
 */
int ind_cfg_set_visible(CfgTree_t tree, char *paramname, int visible);


typedef struct CfgContext* CfgContext_t;

CfgContext_t ind_cfg_context_create(int objsize);

int ind_cfg_obj_offset(CfgContext_t context, char *paramname, int offset, int type, int prmsize);

int ind_cfg_obj_input(CfgContext_t context, void *obj, char *buf, int len);

int ind_cfg_obj_output(CfgContext_t context, void *obj, char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif//__TREE_CFG_H__

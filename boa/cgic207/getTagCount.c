/* Change this if the SERVER_NAME environment variable does not report
	the true name of your web server. */

#if 1
#define SERVER_NAME cgiServerName
#endif
#if 0
#define SERVER_NAME "www.boutell.com"
#endif

/* You may need to change this, particularly under Windows;
	it is a reasonable guess as to an acceptable place to
	store a saved environment in order to test that feature. 
	If that feature is not important to you, you needn't
	concern yourself with this. */

#ifdef WIN32
#define SAVED_ENVIRONMENT "c:\\cgicsave.env"
#else
#define SAVED_ENVIRONMENT "/tmp/cgicsave.env"
#endif /* WIN32 */

#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>    
#include <sys/ipc.h>    
#include <sys/sem.h>   
#include <stdbool.h>


void CookieSet();
void Cookies();

//1024*(tagEPC size) + 1024 �Զ�������  
#define SHARE_MEMORY_SIZE (1024*265)

typedef struct tagEPC {
	char epc[256];
	int length;
	struct tagEPC *next;
}tagEPC;

typedef struct{
	char m_clearFlag;				//��������ڴ����ݱ�־
	unsigned int m_uniqueTagTotal;	//��ǩ����
//	uint32_t m_tagTotal;			//�����Ҫ�ϴ�������
}S_HEADER_INFO;

typedef struct {	
	S_HEADER_INFO m_header_info;
	tagEPC m_tagEPC[1024];
}S_SHARE_MEMORY;  

/*
//�����̵� response 2 head
typedef sturct{
	char soh;
	char length;
	char opcode;
	short status;
	char option2;
	short searchFlag;
	short metadataFlag;
	char tagCountt;
	char readCount;
	char rssi;
	char antennaID;
	char frequency[3];
	char timestamp[4];
	short phase;
	char protocol;
	short EmbDataLen;
}S_TAG_HEADER;
*/

//S_TAG_HEADER *tagHeader = NULL;

/*==========================================
// HEXת���ַ��� 0-9 A-F 
// parameter(s): [OUT] pbDest - ���Ŀ���ַ���
//	[IN] pbSrc - ����16����������ʼ��ַ
//	[IN] nLen - 16���������ֽ��� 8
// return value:
// remarks : ��16������ת��Ϊ�ַ���

hex�޹ش�Сд
�޸� ԭ���� /16 û��ʹ����λ���� ����ʱ���������
ddh = 48 + pbSrc[i] / 16;
ddl = 48 + pbSrc[i] % 16;
===========================================*/
void HexTOStrAF(char *pbDest, char *pbSrc, int nLen)
{
	char ddl, ddh;
	int i;

	for (i = 0; i<nLen; i++)
	{
		ddh = 48 + ((pbSrc[i]>>4)&0x0F); //ȡ��4bit
		ddl = 48 + (pbSrc[i] &0x0F);     //ȡ��4bit

		if (ddh > 57) 
			ddh = ddh + 7;
		if (ddl > 57) 
			ddl = ddl + 7;

		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}

	pbDest[nLen * 2] = '\0'; // ԭ��
}


char rtnData[1024*255] = "";	//ȫ�����ص��ַ���

//��Ҫ�ϴ��������ֶ������ﶨ���Ա
typedef struct{
	char  m_epcId[100];
	short m_epcLen;		//bit or byte
	char m_tagCount;
	char m_readCount;
	char m_rssi;		//���� ��ʾʧ��
}S_TAG_INFO;
S_TAG_INFO tagDB[1024];	//tag���ݿ�    ͨ��malloc���� �˳�ʱ���ͷ�

int g_tagDbIndex = 0;	//tag������¼

#if 1
int cgiMain() {

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "</BODY></HTML>\n");
	return 0;
}
#else
/*============================*/
int cgiMain() {

	char EPCIDStr[255] = "";	//��� ASCII EPCID
	
	cgiHeaderContentType("text/html");

	//��ȡ�����ڴ���   
    int shmid;       
    key_t shmkey;    

    shmkey=ftok("/opt/app",0);    
       
    shmid=shmget(shmkey,SHARE_MEMORY_SIZE,0666|IPC_CREAT);    
    if(shmid==-1)    
   	 	printf("creat shm is fail\n");      	
        
    /*�������ڴ�ӳ�䵽��ǰ���̵ĵ�ַ�У�֮��ֱ�ӶԽ����еĵ�ַaddr�������ǶԹ����ڴ����*/    
    S_SHARE_MEMORY *addr;
    addr=(S_SHARE_MEMORY*)shmat(shmid,0,0);    
    if(addr==(S_SHARE_MEMORY*)-1)    
    	printf("shm shmat is fail\n");      

	int i,j;
	short embLen = 0;
	short epcLen = 0;
	short offset = 0;

	int uniqueTagTotal = 0;		//��¼��ǩ����

	char *p = NULL;

	bool tagExist = false;

	//debug
	char debugTagCount = 0;
	char debugTagReadCount = 0;

	int totalTagCount = addr->m_header_info.m_uniqueTagTotal;
	fprintf(cgiOut,"<tr><th>NO</th><th>EPC ID</th><th>RSSI</th><th>DebugInfo totalTag[%d]</th></tr>",totalTagCount);

	// ��ѯȫ������tag����
	//for(i=0;i<1024;i++)
	for(i=0;i<totalTagCount;i++)
	{
		//�ӹ����ڴ�ȡ��һ��tag����
		p= addr->m_tagEPC[i].epc;

		debugTagCount = p[10];
		debugTagReadCount = p[11];	

		embLen = p[24]*256 + p[25]; 
		embLen = embLen/8;
		
		offset = 25+embLen+1+1; 	
		epcLen = p[offset]*256 + p[offset+1]; 
		epcLen = epcLen/8;			//��������bit����� תΪbyte���ݳ���
		
		offset = offset + 4;		//EPCID ��ʼ��λ��
		epcLen = epcLen - 4;		//ȥ��2�ֽ�PC 2�ֽ�EPC CRC ʣ�µľ���EPCID���ݳ���


		if(0 > epcLen)	//��Ϊ ��ǩ������Ч
		{
			continue;
		}
		if(i==0)	//��0��ֱ��д�����ݿ�
		{
			memcpy(tagDB[0].m_epcId,p+offset,epcLen);
			tagDB[0].m_epcLen = epcLen;
			tagDB[0].m_tagCount = p[10];
			tagDB[0].m_readCount = p[11];
			tagDB[0].m_rssi = p[12];
			g_tagDbIndex = 1;
			
			HexTOStrAF(EPCIDStr, p+offset, epcLen); 
			fprintf(cgiOut," <tr><td>%d</td> ",g_tagDbIndex);
			//fprintf(cgiOut," <td>%d</td> ",epcLen);
			fprintf(cgiOut," <td>%s</td> ",EPCIDStr);
			//fprintf(cgiOut," <td>%d|%d</td> ", debugTagCount,debugTagReadCount);
			if(p[12]&0x80)
			{
				//���λ��1 ����			
				fprintf(cgiOut," <td>-%d</td> ", tagDB[0].m_rssi&0X7F);
			}
			else
			{
				fprintf(cgiOut," <td>%d</td> ", tagDB[0].m_rssi);
			}	
			fprintf(cgiOut," <td>%d</td> </tr>",g_tagDbIndex);		
			
		}
		else
		{
			for(j=0;j<g_tagDbIndex;j++) //��ѯ��ǩ���ݿ� �Ƿ���ڸñ�ǩ��¼
			{
				if( (p[0] != 0xFF) && (p != 0x22) && (p[5] != 0x10) )
				{
					continue;
				}
				if(!memcmp(tagDB[j].m_epcId,p+offset,epcLen))
				{
					//��tag�Ѿ��洢 
					tagExist = true;	
					memcpy(tagDB[j].m_epcId,p+offset,epcLen); //ͬһ�ű�ǩ ���¼�¼
					tagDB[j].m_epcLen = epcLen;
					tagDB[j].m_tagCount = p[10];
					tagDB[j].m_readCount = p[11];
					tagDB[j].m_rssi = p[12];

					//�������Ҫ���� 
					/*
					HexTOStrAF(EPCIDStr, p+offset, epcLen); 
					fprintf(cgiOut," <tr><td>[%d][%d]</td> ",i,g_tagDbIndex);
					fprintf(cgiOut," <td>%d</td> ",epcLen);
					fprintf(cgiOut," <td>%s</td> ",EPCIDStr);
					fprintf(cgiOut," <td>%d|%d</td> ", debugTagCount,debugTagReadCount);
					fprintf(cgiOut," <td>%d</td> </tr>",g_tagDbIndex);	
					*/
					break;
				}
				else
				{
					tagExist = false;
				}
				
			}
			if(tagExist == false)
			{
				//׷��д�� 
				memcpy(tagDB[g_tagDbIndex].m_epcId,p+offset,epcLen);
				tagDB[g_tagDbIndex].m_epcLen = epcLen;
				tagDB[g_tagDbIndex].m_tagCount = p[10];
				tagDB[g_tagDbIndex].m_readCount = p[11];
				tagDB[g_tagDbIndex].m_rssi = p[12];
				g_tagDbIndex++;		//��ǩ���� +1

				
				HexTOStrAF(EPCIDStr, p+offset, epcLen); 
				fprintf(cgiOut," <tr><td>%d</td> ",g_tagDbIndex);
				//fprintf(cgiOut," <td>%d</td> ",epcLen);
				fprintf(cgiOut," <td>%s</td> ",EPCIDStr);
				//fprintf(cgiOut," <td>%d|%d</td> ", debugTagCount,debugTagReadCount);
				char testRSSI = 0XAE;
				if(p[12]&0x80)
				{
					//���λ��1 ����			
					fprintf(cgiOut," <td>-%d</td> ", tagDB[g_tagDbIndex-1].m_rssi&0X7F);
				}
				else
				{
					fprintf(cgiOut," <td>%d</td> ", tagDB[g_tagDbIndex-1].m_rssi);
				}				
				fprintf(cgiOut," <td>%d</td> </tr>",g_tagDbIndex);	
				
			}
		}
	}

	//�����ڴ��ͷ��Ϣ��ռ��� client �жϸñ���Ϊ0 �ʹ�ͷ��ʼ������
	addr->m_header_info.m_clearFlag = 1;	

/*
	//ȫ�����ݲ���ӡ ����ٴ�ӡ����
	//���ݱ�ǩ������ӡ��Ϣ ������Ϣ
	for(i=0;i<g_tagDbIndex;i++)
	{	
		HexTOStrAF(EPCIDStr, tagDB[i].m_epcId, epcLen); 
		fprintf(cgiOut," <tr><td>[%d][%d]</td> ",i,g_tagDbIndex);
		fprintf(cgiOut," <td>%d</td> ",tagDB[i].m_epcLen);
		fprintf(cgiOut," <td>%s</td> ",EPCIDStr);
		fprintf(cgiOut," <td>%d|%d</td> ", tagDB[i].m_tagCount,tagDB[i].m_readCount);
		fprintf(cgiOut," <td>%d</td> </tr>",g_tagDbIndex);	
	}
*/

    /*�������ڴ��뵱ǰ���̶Ͽ�*/    
    if(shmdt(addr)==-1)    
    {
    	printf("shmdt is fail\n");    
	}
	
	/* Finish up the page */
	//fprintf(cgiOut, "</BODY></HTML>\n");

	return 0;
}
#endif

void Cookies()
{
	char **array, **arrayStep;
	char cname[1024], cvalue[1024];
	fprintf(cgiOut, "Cookies Submitted On This Call, With Values (Many Browsers NEVER Submit Cookies):<p>\n");
	if (cgiCookies(&array) != cgiFormSuccess) {
		return;
	}
	arrayStep = array;
	fprintf(cgiOut, "<table border=1>\n");
	fprintf(cgiOut, "<tr><th>Cookie<th>Value</tr>\n");
	while (*arrayStep) {
		char value[1024];
		fprintf(cgiOut, "<tr>");
		fprintf(cgiOut, "<td>");
		cgiHtmlEscape(*arrayStep);
		fprintf(cgiOut, "<td>");
		cgiCookieString(*arrayStep, value, sizeof(value));
		cgiHtmlEscape(value);
		fprintf(cgiOut, "\n");
		arrayStep++;
	}
	fprintf(cgiOut, "</table>\n");
	cgiFormString("cname", cname, sizeof(cname));	
	cgiFormString("cvalue", cvalue, sizeof(cvalue));	
	if (strlen(cname)) {
		fprintf(cgiOut, "New Cookie Set On This Call:<p>\n");
		fprintf(cgiOut, "Name: ");	
		cgiHtmlEscape(cname);
		fprintf(cgiOut, "Value: ");	
		cgiHtmlEscape(cvalue);
		fprintf(cgiOut, "<p>\n");
		fprintf(cgiOut, "If your browser accepts cookies (many do not), this new cookie should appear in the above list the next time the form is submitted.<p>\n"); 
	}
	cgiStringArrayFree(array);
}


void CookieSet()
{
	char cname[1024];
	char cvalue[1024];
	/* Must set cookies BEFORE calling cgiHeaderContentType */
	cgiFormString("cname", cname, sizeof(cname));	
	cgiFormString("cvalue", cvalue, sizeof(cvalue));	
	if (strlen(cname)) {
		/* Cookie lives for one day (or until browser chooses
			to get rid of it, which may be immediately),
			and applies only to this script on this site. */	
		cgiHeaderCookieSetString(cname, cvalue,
			86400, cgiScriptName, SERVER_NAME);
	}
}


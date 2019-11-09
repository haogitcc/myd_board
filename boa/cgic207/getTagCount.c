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

//1024*(tagEPC size) + 1024 自定义数据  
#define SHARE_MEMORY_SIZE (1024*265)

typedef struct tagEPC {
	char epc[256];
	int length;
	struct tagEPC *next;
}tagEPC;

typedef struct{
	char m_clearFlag;				//清除共享内存数据标志
	unsigned int m_uniqueTagTotal;	//标签总数
//	uint32_t m_tagTotal;			//添加需要上传的数据
}S_HEADER_INFO;

typedef struct {	
	S_HEADER_INFO m_header_info;
	tagEPC m_tagEPC[1024];
}S_SHARE_MEMORY;  

/*
//连续盘点 response 2 head
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
// HEX转成字符串 0-9 A-F 
// parameter(s): [OUT] pbDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数 8
// return value:
// remarks : 将16进制数转化为字符串

hex无关大小写
修改 原版是 /16 没有使用移位操作 负数时候会有问题
ddh = 48 + pbSrc[i] / 16;
ddl = 48 + pbSrc[i] % 16;
===========================================*/
void HexTOStrAF(char *pbDest, char *pbSrc, int nLen)
{
	char ddl, ddh;
	int i;

	for (i = 0; i<nLen; i++)
	{
		ddh = 48 + ((pbSrc[i]>>4)&0x0F); //取高4bit
		ddl = 48 + (pbSrc[i] &0x0F);     //取低4bit

		if (ddh > 57) 
			ddh = ddh + 7;
		if (ddl > 57) 
			ddl = ddl + 7;

		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}

	pbDest[nLen * 2] = '\0'; // 原版
}


char rtnData[1024*255] = "";	//全部返回的字符串

//需要上传的数据字段在这里定义成员
typedef struct{
	char  m_epcId[100];
	short m_epcLen;		//bit or byte
	char m_tagCount;
	char m_readCount;
	char m_rssi;		//负数 表示失败
}S_TAG_INFO;
S_TAG_INFO tagDB[1024];	//tag数据库    通过malloc分配 退出时候释放

int g_tagDbIndex = 0;	//tag总数记录

#if 1
int cgiMain() {

	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "</BODY></HTML>\n");
	return 0;
}
#else
/*============================*/
int cgiMain() {

	char EPCIDStr[255] = "";	//存放 ASCII EPCID
	
	cgiHeaderContentType("text/html");

	//读取共享内存数   
    int shmid;       
    key_t shmkey;    

    shmkey=ftok("/opt/app",0);    
       
    shmid=shmget(shmkey,SHARE_MEMORY_SIZE,0666|IPC_CREAT);    
    if(shmid==-1)    
   	 	printf("creat shm is fail\n");      	
        
    /*将共享内存映射到当前进程的地址中，之后直接对进程中的地址addr操作就是对共享内存操作*/    
    S_SHARE_MEMORY *addr;
    addr=(S_SHARE_MEMORY*)shmat(shmid,0,0);    
    if(addr==(S_SHARE_MEMORY*)-1)    
    	printf("shm shmat is fail\n");      

	int i,j;
	short embLen = 0;
	short epcLen = 0;
	short offset = 0;

	int uniqueTagTotal = 0;		//记录标签总数

	char *p = NULL;

	bool tagExist = false;

	//debug
	char debugTagCount = 0;
	char debugTagReadCount = 0;

	int totalTagCount = addr->m_header_info.m_uniqueTagTotal;
	fprintf(cgiOut,"<tr><th>NO</th><th>EPC ID</th><th>RSSI</th><th>DebugInfo totalTag[%d]</th></tr>",totalTagCount);

	// 查询全部数据tag数据
	//for(i=0;i<1024;i++)
	for(i=0;i<totalTagCount;i++)
	{
		//从共享内存取出一个tag数据
		p= addr->m_tagEPC[i].epc;

		debugTagCount = p[10];
		debugTagReadCount = p[11];	

		embLen = p[24]*256 + p[25]; 
		embLen = embLen/8;
		
		offset = 25+embLen+1+1; 	
		epcLen = p[offset]*256 + p[offset+1]; 
		epcLen = epcLen/8;			//长度是以bit计算的 转为byte数据长度
		
		offset = offset + 4;		//EPCID 开始的位置
		epcLen = epcLen - 4;		//去掉2字节PC 2字节EPC CRC 剩下的就是EPCID数据长度


		if(0 > epcLen)	//认为 标签数据无效
		{
			continue;
		}
		if(i==0)	//第0个直接写入数据库
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
				//最高位是1 负数			
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
			for(j=0;j<g_tagDbIndex;j++) //轮询标签数据库 是否存在该标签记录
			{
				if( (p[0] != 0xFF) && (p != 0x22) && (p[5] != 0x10) )
				{
					continue;
				}
				if(!memcmp(tagDB[j].m_epcId,p+offset,epcLen))
				{
					//该tag已经存储 
					tagExist = true;	
					memcpy(tagDB[j].m_epcId,p+offset,epcLen); //同一张标签 更新记录
					tagDB[j].m_epcLen = epcLen;
					tagDB[j].m_tagCount = p[10];
					tagDB[j].m_readCount = p[11];
					tagDB[j].m_rssi = p[12];

					//这个不需要开启 
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
				//追加写入 
				memcpy(tagDB[g_tagDbIndex].m_epcId,p+offset,epcLen);
				tagDB[g_tagDbIndex].m_epcLen = epcLen;
				tagDB[g_tagDbIndex].m_tagCount = p[10];
				tagDB[g_tagDbIndex].m_readCount = p[11];
				tagDB[g_tagDbIndex].m_rssi = p[12];
				g_tagDbIndex++;		//标签总数 +1

				
				HexTOStrAF(EPCIDStr, p+offset, epcLen); 
				fprintf(cgiOut," <tr><td>%d</td> ",g_tagDbIndex);
				//fprintf(cgiOut," <td>%d</td> ",epcLen);
				fprintf(cgiOut," <td>%s</td> ",EPCIDStr);
				//fprintf(cgiOut," <td>%d|%d</td> ", debugTagCount,debugTagReadCount);
				char testRSSI = 0XAE;
				if(p[12]&0x80)
				{
					//最高位是1 负数			
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

	//共享内存的头信息清空即可 client 判断该变量为0 就从头开始放数据
	addr->m_header_info.m_clearFlag = 1;	

/*
	//全部数据不打印 最后再打印数据
	//根据标签总数打印信息 最新信息
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

    /*将共享内存与当前进程断开*/    
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


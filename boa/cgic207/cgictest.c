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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
	
#include "app_sys_setting.h"


void HandleSubmit();
void ShowForm();
void CookieSet();
void Name();
void Address();
void Hungry();
void Temperature();
void Frogs();
void Color();
void Flavors();
void NonExButtons();
void RadioButtons();
void File();
void Entries();
void Cookies();
void LoadEnvironment();
void SaveEnvironment();

int GetWifiInfo(char *argv);

char wipaddr[16] = {0};
char wnetmask[16] = {0};
char wbroadaddr[16] = {0};
char wgateway[16] = {0};


/** 
 * [GetWifiInfo description]
 * @Author   YY
 * @DateTime 2019年5月7日T19:14:46+0800
 * @param    argv                     [网络接口名称 eth0 eth0:0 lo wlan0]
 * @return                            [description]
 */
int GetWifiInfo(char *argv)
{
	struct sockaddr_in *addr;
    struct ifreq ifr;
    char *name, *address;
    int sockfd = -1;

//    if (argc != 2) {
//        printf("usage : %s interface \n", argv[0]);
//        exit(-1);
//    } else
//        name = argv[1];

	name = argv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl error\n");
       	return -1;
    }
    addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    address = inet_ntoa(addr->sin_addr);
    //printf("inet addr: %s\n", address);
    memcpy(wipaddr,address,16);
    
    if (ioctl(sockfd, SIOCGIFBRDADDR, &ifr) == -1) {
        perror("ioctl error\n");
        return -1;
    }
    addr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
    address = inet_ntoa(addr->sin_addr);
    //printf("broad addr: %s\n", address);
    memcpy(wgateway,address,16);
    
    if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) == -1) {
        perror("ioctl error\n");
         return -1;
    }
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    address = inet_ntoa(addr->sin_addr);
    //printf("inet mask: %s\n", address);
    memcpy(wnetmask,address,16);

    close(sockfd);
	
	return 0;
}


/*============================*/
//#define NET_LEN 16
//#define CONFIG_FILE_DIR "/opt/file"
//#define NET_ETH 0
#define STRING_LEN_64 64


int cgiMain() {
#ifdef DEBUG
	LoadEnvironment();
#endif /* DEBUG */

	char *str = NULL;	
	char ipaddr[16] = {0};
	char netmask[16] = {0};
	char gateway[16] = {0};
    char dns0[16] = {0};
    char dns1[16] = {0};
  
	char wifiname[STRING_LEN_64] = {0};
	char wifipassword[STRING_LEN_64] = {0};

	char apname[STRING_LEN_64] = {0};
	char appassword[STRING_LEN_64] = {0};
	int wifiMode = 0;
	
	int serverport = 8086;

	cgiHeaderContentType("text/html");
	
	sys_config_init();	
	sys_config_load(0);

	str = (char *)malloc(1024);
	
    sysSettingGetString("ip", ipaddr, 16, 0);
    sysSettingGetString("netmask", netmask, 16, 0);
    sysSettingGetString("gateway", gateway, 16, 0);
    sysSettingGetInt("serverport", &serverport, 0);
    
    sysSettingGetString("wifi", wifiname, 64, 0);
    sysSettingGetString("wifiwd", wifipassword, 64, 0);

    sysSettingGetInt("wifiMode", &wifiMode, 0);
    
		//debug 不是调试时候关闭这些打印信息否则出错
	//	  printf("sysConfigs.ip=[%s]",ipaddr);
	//	  printf("sysConfigs.netmask=[%s]",netmask);
	//	  printf("sysConfigs.gateway=[%s]",gateway);
	//	  printf("sysConfigs.serverport=[%d]",serverport);

	/*
		wifi和有线网络可以同时存在
		如果wlan没有信息 不需要设置 
		web端会显示 undefined
	*/

	//返回 json数据格式信息
	if(0 == GetWifiInfo("wlan0"))	//正确获取WIFI信息  //"eth0:0"
	{
		sprintf(str,"{\"ip\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\"sPort\":\"%d\",\"wifi\":\"%s\",\"wifiwd\":\"%s\",\"wifiMode\":\"%d\",\"wifiIp\":\"%s\",\"wifiMask\":\"%s\",\"wifiGw\":\"%s\"}"
		,ipaddr,netmask,gateway,serverport,wifiname,wifipassword,wifiMode,wipaddr,wnetmask,wgateway);
	}
	else	//只有有线网络 
	{
		sprintf(str,"{\"ip\":\"%s\",\"netmask\":\"%s\",\"gateway\":\"%s\",\"sPort\":\"%d\",\"wifi\":\"%s\",\"wifiwd\":\"%s\",\"wifiMode\":\"%d\"}",ipaddr,netmask,gateway,serverport,wifiname,wifipassword,wifiMode);
	}

	//fprintf(cgiOut,"{ \"firstName\":\"John\" , \"lastName\":\"Doe\" }");
	fprintf(cgiOut,str);

	/* Finish up the page */
	//fprintf(cgiOut, "</BODY></HTML>\n");
	free(str);
	return 0;
}


void HandleSubmit()
{
	/*
	Name();
	Address();
	Hungry();
	Temperature();
	Frogs();
	Color();
	Flavors();
	NonExButtons();
	RadioButtons();
	File();
	Entries();
	Cookies();
	*/
	/* The saveenvironment button, in addition to submitting the form,
		also saves the resulting CGI scenario to disk for later
		replay with the 'load saved environment' button. */
	if (cgiFormSubmitClicked("saveenvironment") == cgiFormSuccess) {
		SaveEnvironment();
	}
}

void Name() {
	char name[81];
	cgiFormStringNoNewlines("name", name, 81);
	fprintf(cgiOut, "Name: ");
	cgiHtmlEscape(name);
	fprintf(cgiOut, "<BR>\n");
}
	
void Address() {
	char address[241];
	cgiFormString("address", address, 241);
	fprintf(cgiOut, "Address: <PRE>\n");
	cgiHtmlEscape(address);
	fprintf(cgiOut, "</PRE>\n");
}

void Hungry() {
	if (cgiFormCheckboxSingle("hungry") == cgiFormSuccess) {
		fprintf(cgiOut, "I'm Hungry!<BR>\n");
	} else {
		fprintf(cgiOut, "I'm Not Hungry!<BR>\n");
	}
}
	
void Temperature() {
	double temperature;
	cgiFormDoubleBounded("temperature", &temperature, 80.0, 120.0, 98.6);
	fprintf(cgiOut, "My temperature is %f.<BR>\n", temperature);
}
	
void Frogs() {
	int frogsEaten;
	cgiFormInteger("frogs", &frogsEaten, 0);
	fprintf(cgiOut, "I have eaten %d frogs.<BR>\n", frogsEaten);
}

char *colors[] = {
	"Red",
	"Green",
	"Blue"
};

void Color() {
	int colorChoice;
	cgiFormSelectSingle("colors", colors, 3, &colorChoice, 0);
	fprintf(cgiOut, "I am: %s<BR>\n", colors[colorChoice]);
}	 

char *flavors[] = {
	"pistachio",
	"walnut",
	"creme"
};

void Flavors() {
	int flavorChoices[3];
	int i;
	int result;	
	int invalid;
	result = cgiFormSelectMultiple("flavors", flavors, 3, 
		flavorChoices, &invalid);
	if (result == cgiFormNotFound) {
		fprintf(cgiOut, "I hate ice cream.<p>\n");
	} else {	
		fprintf(cgiOut, "My favorite ice cream flavors are:\n");
		fprintf(cgiOut, "<ul>\n");
		for (i=0; (i < 3); i++) {
			if (flavorChoices[i]) {
				fprintf(cgiOut, "<li>%s\n", flavors[i]);
			}
		}
		fprintf(cgiOut, "</ul>\n");
	}
}

char *ages[] = {
	"1",
	"2",
	"3",
	"4"
};

void RadioButtons() {
	int ageChoice;
	char ageText[10];
	/* Approach #1: check for one of several valid responses. 
		Good if there are a short list of possible button values and
		you wish to enumerate them. */
	cgiFormRadio("age", ages, 4, &ageChoice, 0);

	fprintf(cgiOut, "Age of Truck: %s (method #1)<BR>\n", 
		ages[ageChoice]);

	/* Approach #2: just get the string. Good
		if the information is not critical or if you wish
		to verify it in some other way. Note that if
		the information is numeric, cgiFormInteger,
		cgiFormDouble, and related functions may be
		used instead of cgiFormString. */	
	cgiFormString("age", ageText, 10);

	fprintf(cgiOut, "Age of Truck: %s (method #2)<BR>\n", ageText);
}

char *votes[] = {
	"A",
	"B",
	"C",
	"D"
};

void NonExButtons() {
	int voteChoices[4];
	int i;
	int result;	
	int invalid;

	char **responses;

	/* Method #1: check for valid votes. This is a good idea,
		since votes for nonexistent candidates should probably
		be discounted... */
	fprintf(cgiOut, "Votes (method 1):<BR>\n");
	result = cgiFormCheckboxMultiple("vote", votes, 4, 
		voteChoices, &invalid);
	if (result == cgiFormNotFound) {
		fprintf(cgiOut, "I hate them all!<p>\n");
	} else {	
		fprintf(cgiOut, "My preferred candidates are:\n");
		fprintf(cgiOut, "<ul>\n");
		for (i=0; (i < 4); i++) {
			if (voteChoices[i]) {
				fprintf(cgiOut, "<li>%s\n", votes[i]);
			}
		}
		fprintf(cgiOut, "</ul>\n");
	}

	/* Method #2: get all the names voted for and trust them.
		This is good if the form will change more often
		than the code and invented responses are not a danger
		or can be checked in some other way. */
	fprintf(cgiOut, "Votes (method 2):<BR>\n");
	result = cgiFormStringMultiple("vote", &responses);
	if (result == cgiFormNotFound) {	
		fprintf(cgiOut, "I hate them all!<p>\n");
	} else {
		int i = 0;
		fprintf(cgiOut, "My preferred candidates are:\n");
		fprintf(cgiOut, "<ul>\n");
		while (responses[i]) {
			fprintf(cgiOut, "<li>%s\n", responses[i]);
			i++;
		}
		fprintf(cgiOut, "</ul>\n");
	}
	/* We must be sure to free the string array or a memory
		leak will occur. Simply calling free() would free
		the array but not the individual strings. The
		function cgiStringArrayFree() does the job completely. */	
	cgiStringArrayFree(responses);
}

void Entries()
{
	char **array, **arrayStep;
	fprintf(cgiOut, "List of All Submitted Form Field Names:<p>\n");
	if (cgiFormEntries(&array) != cgiFormSuccess) {
		return;
	}
	arrayStep = array;
	fprintf(cgiOut, "<ul>\n");
	while (*arrayStep) {
		fprintf(cgiOut, "<li>");
		cgiHtmlEscape(*arrayStep);
		fprintf(cgiOut, "\n");
		arrayStep++;
	}
	fprintf(cgiOut, "</ul>\n");
	cgiStringArrayFree(array);
}

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
	
void File()
{
	cgiFilePtr file;
	char name[1024];
	char contentType[1024];
	char buffer[1024];
	int size;
	int got;
	if (cgiFormFileName("file", name, sizeof(name)) != cgiFormSuccess) {
		printf("<p>No file was uploaded.<p>\n");
		return;
	} 
	fprintf(cgiOut, "The filename submitted was: ");
	cgiHtmlEscape(name);
	fprintf(cgiOut, "<p>\n");
	cgiFormFileSize("file", &size);
	fprintf(cgiOut, "The file size was: %d bytes<p>\n", size);
	cgiFormFileContentType("file", contentType, sizeof(contentType));
	fprintf(cgiOut, "The alleged content type of the file was: ");
	cgiHtmlEscape(contentType);
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Of course, this is only the claim the browser made when uploading the file. Much like the filename, it cannot be trusted.<p>\n");
	fprintf(cgiOut, "The file's contents are shown here:<p>\n");
	if (cgiFormFileOpen("file", &file) != cgiFormSuccess) {
		fprintf(cgiOut, "Could not open the file.<p>\n");
		return;
	}
	fprintf(cgiOut, "<pre>\n");
	while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) ==
		cgiFormSuccess)
	{
		cgiHtmlEscapeData(buffer, got);
	}
	fprintf(cgiOut, "</pre>\n");
	cgiFormFileClose(file);
}

void ShowForm()
{
	fprintf(cgiOut, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" />");
	//css
	fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"/css/style.css\" media=\"screen\" type=\"text/css\" />");
	//end css
	
	fprintf(cgiOut, "</head>");
	fprintf(cgiOut, "<body>");
	fprintf(cgiOut, "<form>");
	//fprintf(cgiOut, "<link rel=\"stylesheet\" href=\"./css/style.css\" media=\"screen\" type=\"text/css\" />");
	fprintf(cgiOut, "<div id=\"masterdiv\">");
	fprintf(cgiOut, "<div class=\"menutitle\" onclick=\"SwitchMenu('sub1')\">Site Menu</div>");
	fprintf(cgiOut, "<span class=\"submenu\" id=\"sub1\">");
	fprintf(cgiOut, "<fieldset>");
	fprintf(cgiOut, "<legend>Read/Write Options </legend>");
	fprintf(cgiOut, "RF On (ms)<input type=\"text\" />");
	fprintf(cgiOut, "RF Off (ms)<input type=\"text\" />");
	fprintf(cgiOut, "</fieldset>");
	fprintf(cgiOut, "<fieldset>");
    fprintf(cgiOut,"<legend>Protocols </legend>");
    fprintf(cgiOut, "</fieldset>");
	fprintf(cgiOut, "<fieldset>");
    fprintf(cgiOut,"<legend>Antennas </legend>");
    fprintf(cgiOut,"ONE锛?input type=\"text\" />");
    fprintf(cgiOut,"TWO锛?input type=\"text\" />");
  	fprintf(cgiOut,"</fieldset>");
	fprintf(cgiOut, "</span>");
	fprintf(cgiOut, "</div>");
	fprintf(cgiOut, "</form>");

	/*==========================*/
	fprintf(cgiOut, "<script src=\"/js/index.js\"></script>");
}

/*
void ShowForm()
{
	fprintf(cgiOut, "<!-- 2.0: multipart/form-data is required for file uploads. -->");
	fprintf(cgiOut, "<form method=\"POST\" enctype=\"multipart/form-data\" ");
	fprintf(cgiOut, "	action=\"");
	cgiValueEscape(cgiScriptName);
	fprintf(cgiOut, "\">\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Text Field containing Plaintext\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<input type=\"text\" name=\"name\">Your Name\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Multiple-Line Text Field\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<textarea NAME=\"address\" ROWS=4 COLS=40>\n");
	fprintf(cgiOut, "Default contents go here. \n");
	fprintf(cgiOut, "</textarea>\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Checkbox\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"hungry\" checked>Hungry\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Text Field containing a Numeric Value\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<input type=\"text\" name=\"temperature\" value=\"98.6\">\n");
	fprintf(cgiOut, "Blood Temperature (80.0-120.0)\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Text Field containing an Integer Value\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<input type=\"text\" name=\"frogs\" value=\"1\">\n");
	fprintf(cgiOut, "Frogs Eaten\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "Single-SELECT\n");
	fprintf(cgiOut, "<br>\n");
	fprintf(cgiOut, "<select name=\"colors\">\n");
	fprintf(cgiOut, "<option value=\"Red\">Red\n");
	fprintf(cgiOut, "<option value=\"Green\">Green\n");
	fprintf(cgiOut, "<option value=\"Blue\">Blue\n");
	fprintf(cgiOut, "</select>\n");
	fprintf(cgiOut, "<br>\n");
	fprintf(cgiOut, "Multiple-SELECT\n");
	fprintf(cgiOut, "<br>\n");
	fprintf(cgiOut, "<select name=\"flavors\" multiple>\n");
	fprintf(cgiOut, "<option value=\"pistachio\">Pistachio\n");
	fprintf(cgiOut, "<option value=\"walnut\">Walnut\n");
	fprintf(cgiOut, "<option value=\"creme\">Creme\n");
	fprintf(cgiOut, "</select>\n");
	fprintf(cgiOut, "<p>Exclusive Radio Button Group: Age of Truck in Years\n");
	fprintf(cgiOut, "<input type=\"radio\" name=\"age\" value=\"1\">1\n");
	fprintf(cgiOut, "<input type=\"radio\" name=\"age\" value=\"2\">2\n");
	fprintf(cgiOut, "<input type=\"radio\" name=\"age\" value=\"3\" checked>3\n");
	fprintf(cgiOut, "<input type=\"radio\" name=\"age\" value=\"4\">4\n");
	fprintf(cgiOut, "<p>Nonexclusive Checkbox Group: Voting for Zero through Four Candidates\n");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"vote\" value=\"A\">A\n");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"vote\" value=\"B\">B\n");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"vote\" value=\"C\">C\n");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"vote\" value=\"D\">D\n");
	fprintf(cgiOut, "<p>File Upload:\n");
	fprintf(cgiOut, "<input type=\"file\" name=\"file\" value=\"\"> (Select A Local File)\n");
	fprintf(cgiOut, "<p>\n");
	fprintf(cgiOut, "<p>Set a Cookie<p>\n");
	fprintf(cgiOut, "<input name=\"cname\" value=\"\"> Cookie Name\n");
	fprintf(cgiOut, "<input name=\"cvalue\" value=\"\"> Cookie Value<p>\n");
	fprintf(cgiOut, "<input type=\"submit\" name=\"testcgic\" value=\"Submit Request\">\n");
	fprintf(cgiOut, "<input type=\"reset\" value=\"Reset Request\">\n");
	fprintf(cgiOut, "<p>Save the CGI Environment<p>\n");
	fprintf(cgiOut, "Pressing this button will submit the form, then save the CGI environment so that it can be replayed later by calling cgiReadEnvironment (in a debugger, for instance).<p>\n");
	fprintf(cgiOut, "<input type=\"submit\" name=\"saveenvironment\" value=\"Save Environment\">\n");
	fprintf(cgiOut, "</form>\n");
}
*/

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

void LoadEnvironment()
{
	if (cgiReadEnvironment(SAVED_ENVIRONMENT) != 
		cgiEnvironmentSuccess) 
	{
		cgiHeaderContentType("text/html");
		fprintf(cgiOut, "<head>Error</head>\n");
		fprintf(cgiOut, "<body><h1>Error</h1>\n");
		fprintf(cgiOut, "cgiReadEnvironment failed. Most "
			"likely you have not saved an environment "
			"yet.\n");
		exit(0);
	}
	/* OK, return now and show the results of the saved environment */
}

void SaveEnvironment()
{
	if (cgiWriteEnvironment(SAVED_ENVIRONMENT) != 
		cgiEnvironmentSuccess) 
	{
		fprintf(cgiOut, "<p>cgiWriteEnvironment failed. Most "
			"likely %s is not a valid path or is not "
			"writable by the user that the CGI program "
			"is running as.<p>\n", SAVED_ENVIRONMENT);
	} else {
		fprintf(cgiOut, "<p>Environment saved. Click this button "
			"to restore it, playing back exactly the same "
			"scenario: "
			"<form method=POST action=\"");
		cgiValueEscape(cgiScriptName);
		fprintf(cgiOut, "\">" 
			"<input type=\"submit\" "
			"value=\"Load Environment\" "
			"name=\"loadenvironment\"></form><p>\n");
	}
}


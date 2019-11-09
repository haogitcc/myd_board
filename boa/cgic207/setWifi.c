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

#include "app_sys_setting.h"
#include <netdb.h>


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

/*============================*/
#define cgiStrEq(a, b) (!strcmp((a), (b)))

s_IP_SETTINGS ip_settings;

char cmdbuf[100] = {0};

char *WifiMode[] = {
	"0",
	"1",
	"2"
};

/*
	连接WIFI
	同时写入配置文件
*/
int cgiMain() {
#ifdef DEBUG
	LoadEnvironment();
#endif /* DEBUG */

	char *str = NULL;

	//printf("int setip cgi\n");	
	str = (char *)malloc(1024);
		
	cgiHeaderContentType("text/html");

//	cgiFormStringNoNewlines("ip", &ip_settings.ip, 16);

//	cgiFormStringNoNewlines("netmask", &ip_settings.netmask, 16);

//	cgiFormStringNoNewlines("gateway", &ip_settings.gateway, 16);

//	//端口 html name字段
//	cgiFormInteger("sPort", &ip_settings.port, 8086);

	cgiFormStringNoNewlines("wifiSsid", &ip_settings.wifi, 64);
	
	cgiFormStringNoNewlines("wifiwd", &ip_settings.wifiwd, 64);

	int modeChoice;
	cgiFormSelectSingle("wifiMode", WifiMode, 3, &modeChoice, 0);
	if(cgiStrEq(WifiMode[modeChoice],"0"))
	{
		ip_settings.wifiMode = 0;
	}
	else if(cgiStrEq(WifiMode[modeChoice],"1"))
	{
		ip_settings.wifiMode = 1;
	}	
	else if(cgiStrEq(WifiMode[modeChoice],"2"))
	{
		ip_settings.wifiMode = 2;
	}	

	sys_config_init();	
	//写入配置文件
	sys_config_load_2(2,&ip_settings);
	/* Finish up the page */
	//fprintf(cgiOut, "</BODY></HTML>\n");  // no need
	free(str);

	/*
		cgi 使用system 需要绝对路径
		/sbin 目录下的需要绝对路径 否则不执行
		或者修改 boa.conf CGIPath 添加该路径
		CGIPath /bin:/usr/bin:/usr/local/bin:/sbin:/usr/sbin ###在后面要加上命令的搜索路径，以冒号隔开
	*/
	//系统调用     连接wifi  
	//system("echo hello >>txt.txt"); //可以调用
	//system("/sbin/ifconfig>ip.txt"); //使用绝对路径就可以 
	//system("reboot");	 //不能执行
	//根据wifiMode 执行不同脚本
	//system("sh /opt/boa/cgi-bin/sh.sh");
	//system("ps>ps.txt");

	char *ssid = ip_settings.wifi;
	char *passwd = ip_settings.wifiwd;

	//char ssid[64]= "ssid";
	//char passwd[64]= "12345678";	
	
	if(0 == ip_settings.wifiMode)
	{
		sprintf(cmdbuf, "sta.sh %s %s",ssid,passwd);
		printf("%s\n", cmdbuf);
		system(cmdbuf);
		//system("ps>sta.txt");
	}
	else if(1 == ip_settings.wifiMode)
	{
		system("2g_ap_mode.sh");
	}
	else if(2 == ip_settings.wifiMode)
	{
		system("5g_ap_mode.sh");
	}

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
	cgiFormInteger("frogs", &frogsEaten,0);
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
	
	/*============缃戦〉鏄剧ず鍐呭==============*/
	//涓枃瀛楃 闇�瑕佸皢璇ユ枃浠舵敼涓篻b2312缂栫爜 浣跨敤UTF-8浼氫贡鐮?	fprintf(cgiOut, "<title>Test</title>");
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



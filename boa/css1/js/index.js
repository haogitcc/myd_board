if (document.getElementById){ //DynamicDrive.com change
	document.write('<style type="text/css">\n')
	document.write('.submenu{display: none;}\n')
	document.write('</style>\n')
}

//fold
function SwitchMenu(obj){
	if(document.getElementById){
		var el = document.getElementById(obj);
		var ar = document.getElementById("masterdiv").getElementsByTagName("span"); //DynamicDrive.com change
		if(el.style.display != "block"){ //DynamicDrive.com change
			for (var i=0; i<ar.length; i++){
				if (ar[i].className=="submenu") //DynamicDrive.com change
				ar[i].style.display = "none";
			}
			el.style.display = "block";
		}else{
			el.style.display = "none";
		}
	}
}

function loadXMLDoc()
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
		}
	}
	xmlhttp.open("GET","/cgi-bin/cgictest.cgi?t=" + Math.random(),true);
	xmlhttp.send();
}

function loadXMLDocTagShow()
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			document.getElementById("myDiv2").innerHTML=xmlhttp.responseText;
		}
	}
	xmlhttp.open("GET","/cgi-bin/getTagCount.cgi?t=" + Math.random(),true);
	xmlhttp.send();
}

//var timerForTag;
//function TagCount()
//{ 
	//timerForTag = window.setInterval("loadXMLDocTagShow()",1000); 
	//window.clearInterval(t1); 
//}

function setIpPostJs(){ 
	obj = document.getElementById("txtIp").value;  
	//ip地址  
	var exp=/^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;  
	var reg = obj.match(exp); 
	if(reg==null)  
	{  
		alert("IP地址不合法！");  
		onloadFun();
		return;
	}  

	obj = document.getElementById("txtSub").value;  
	reg = obj.match(exp); 
	if(reg==null)  
	{  
		alert("IP地址不合法！"); 
		onloadFun(); 
		return;
	}  

	obj = document.getElementById("txtGw").value;   
	reg = obj.match(exp); 
	if(reg==null)  
	{  
		alert("IP地址不合法！"); 
		onloadFun(); 
		return;
	}  
	 
	alert("写入配置文件");
    $(function(){
        $.ajax({
            url:'/cgi-bin/setIp.cgi',
            type:'post',
            //dataType:'json',
            data:$("#ipSetForm").serialize(),
            success:function(result){
				alert(result);
            },
            error:function(result){
		        alert(arguments[1])
		    } 
        });
    });
}

//连接wifi
function setWifiPostJs(){ 
	var x = document.getElementById("wifiSsid").value;
	if(x.length == 0 || x.length >63)
	{
		alert("ssid不能为空");
		onloadFun();
		return;
	}
	var x = document.getElementById("wifiwd").value;
	if(x.length < 8 || x.length >63)
	{
		alert("密码最小长度8位");
		onloadFun();
		return;
	}
	
    $(function(){
        $.ajax({
            url:'/cgi-bin/setWifi.cgi',
            type:'post',
            //dataType:'json',
            data:$("#ipSetForm").serialize(),
            success:function(result){
            	//if(result == 'success'){
            		 alert("waiting connect " + result);
            	//}      
            },
            error:function(result){
		        alert(arguments[1])
		    }  
        });
    });
}

var staSsid;
var staPwd;

function onloadFun()
{
	var xmlhttp;
	
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			//debug
			//document.getElementById("debug").innerHTML=xmlhttp.responseText;
			
			var rtnStr = xmlhttp.responseText;			
			JSONObject = eval ("(" + rtnStr + ")");
			document.getElementById("txtIp").value=JSONObject.ip;
			document.getElementById("txtSub").value=JSONObject.netmask;
			document.getElementById("txtGw").value=JSONObject.gateway;		
			document.getElementById("txtPort").value=JSONObject.sPort;	
			document.getElementById("wifiSsid").value=JSONObject.wifi;
			document.getElementById("wifiwd").value=JSONObject.wifiwd;	
			staSsid = JSONObject.wifi;
			staPwd = JSONObject.wifiwd;		
			document.getElementById("wifiIp").value=JSONObject.wifiIp;
			document.getElementById("wifiSub").value=JSONObject.wifiMask;
			document.getElementById("wifiGw").value=JSONObject.wifiGw;	
			var wmode =  JSONObject.wifiMode;
			var  ss =document.getElementById('wifiMode');
			if(wmode == 0)
			{
				ss[0].selected = true;
			}
			else if(wmode == 1)
			{
				ss[1].selected = true;
				document.getElementById("wifiSsid").value="FUWIT2G";
				document.getElementById("wifiwd").value="88888888";	
				document.getElementById("wifiSsid").disabled = true;
				document.getElementById("wifiwd").disabled = true;
			}
			else if(wmode == 2)
			{
				ss[2].selected = true;
				document.getElementById("wifiSsid").value="FUWIT5G";
				document.getElementById("wifiwd").value="88888888";	
				document.getElementById("wifiSsid").disabled = true;
				document.getElementById("wifiwd").disabled = true;
			}			
			/*
			var JSONObject= {"ip":"192.168.129.130","netmask":"255.255.0.0","gateway":"192.168.1.1"};
			*/	
		}
	}
	xmlhttp.open("GET","/cgi-bin/cgictest.cgi?t=" + Math.random(),true);
	xmlhttp.send();
}

function selectChange()
{
	var  wmode =document.getElementById('wifiMode').value;
	if(wmode == 0)
	{
		document.getElementById("wifiSsid").value=staSsid;
		document.getElementById("wifiwd").value=staPwd;	
		document.getElementById("wifiSsid").disabled = false;
		document.getElementById("wifiwd").disabled = false;
	}
	else if(wmode == 1)
	{
		document.getElementById("wifiSsid").value="FUWIT2G";
		document.getElementById("wifiwd").value="88888888";	
		document.getElementById("wifiSsid").disabled = true;
		document.getElementById("wifiwd").disabled = true;
	}
	else if(wmode == 2)
	{
		document.getElementById("wifiSsid").value="FUWIT5G";
		document.getElementById("wifiwd").value="88888888";	
		document.getElementById("wifiSsid").disabled = true;
		document.getElementById("wifiwd").disabled = true;
	}	
}
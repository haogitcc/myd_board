

这个是整个web的工作目录

将css js 和 index拷贝到虚拟机


index.html是主页 
js/index.js 是主js文件
cgictest.html  是cgi官网的源文件命名 内容已经被修改 不再使用

访问虚拟机IE输入 192.169.129.130

本地调试需要更改 index.html 的相关路径设置

<link rel="stylesheet" href="/css/style.css" media="screen" type="text/css" />

改为 添加相对路径.
href="./css/style.css"

<script src="/js/index.js"></script>
<script type="text/javascript" src="/js/jquery-3.0.0.js"></script>
<script type="text/javascript" src="/js/jquery-3.0.0.min.js"></script>
改为
src="./js/index.js"

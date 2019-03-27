本目录下的文件主要是用于测试本次实习课题的功能实现

主要测试文件如下：
Client：客户端的可执行程序
Server：服务器的可执行程序
joey.xml xml数据文件 
joey.json json数据文件
joey.bmp bmp图片
joey.txt txt文件

1、将Server文件用tftp工具下载到机顶盒中，给权限 chmod 777 Server

2、将其他文件挂载或复制到Linux操作系统的工作目录下

3、在控制盒子的SecureCRt终端下，先运行服务器 ./Server 

4、在Linux操作系统下，运行客户端，
	记得在后面加服务器的端口号(6000)和IP地址(服务器ip)
	./Client 6000 服务器ip
	
5、再按照提示进行命令输入：
	客户端先输入0 表示请求发送数据包  =>  服务器输入0拒绝 1不加密 2加密
	客户端输入1 表示端口连接
	客户端输入2 表示发送xml文件 再输入xml文件路径 ./joey.xml
	客户端输入3 表示发送json文件 再输入json文件路径 ./joey.json
	客户端输入4 表示发送bmp图片 再输入bmp图片路径 ./joey.bmp
	客户端输入5 表示发送txt文件 再输入txt文件路径 ./joey.txt
	客户端输入6 表示发送URL链接 再输入链接地址 如 http://10.10.63.10:8003/video/4k/food.mp4
	
6、测试是否成功：
	在盒子的SecureCRT终端，使用 'cd' 进入生成的两个不同目录下
	再用 'ls -la' 命令查看每个目录下 所有文件的大小，检查接收到
	的文件大小与原文件大小是否一致，
	还可以用 'cat' 或 'more'命令来查看文本内容是否一致
	或者直接用tftp命令将所接收到的文件上传到windows端，再进行比对数据传输是否正确
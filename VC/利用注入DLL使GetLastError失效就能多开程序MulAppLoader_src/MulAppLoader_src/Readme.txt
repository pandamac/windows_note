1、MulAppLoader工程中包含一个Dll和一个Exe，Dll中实现了Hook，Exe中实现了启动进行并向进程注入Dll。
2、SingleApp工程是一个单实例运行的例子程序，使用了Mutex和Event的方式防止多开。
3、测试时可以打开MulAppExe.exe，然后将SingleApp.exe拖到对话框，SingleApp就可以多开了，直接运行的话会打印提示的。


有建议或意见可以用以下方式进行交流：
QQ群:275372932
主页:www.peheaven.cn
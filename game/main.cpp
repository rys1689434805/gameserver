#include <iostream>
#include <zinx.h>
#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "AoiWord.h"
#include "RandomName.h"
using namespace std;
extern RandomName random_name;  //跨文件声明

void deamlize() {
	int pid = fork();  //fork()
	if (pid < 0) {
		exit(-1);
	}
	if (pid > 0) {
		exit(0);  //父进程退出
	}
	setsid();  //子进程设置会话id
	int nullfd = open("/dev/null", O_RDWR);  
	if (nullfd >= 0) {
		dup2(nullfd, 0);  //把这三个文件描述符重定向到null
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
}

//class MyPalyer :public Palyer {
//public:
//	int x = 0, y = 0;
//	string str;
//	MyPalyer(int _x, int _y, string _str) :x(_x), y(_y), str(_str)
//	{};
//	// 通过 Palyer 继承
//	virtual int getX() override
//	{
//		return x;
//	}
//	virtual int getY() override
//	{
//		return y;
//	}
//};


int main() {
	/*pb::SyncPid* pMsg = new pb::SyncPid();
	pMsg->set_pid(1);
	pMsg->set_username("test");
	GameMsg* msg = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME,pMsg);
	auto str = msg->sarilize();
	for (auto single : str) {
		printf("%02x", single);
	}
	puts("");*/
	/*AoiWord w(20,200,6,50,230,6);
	MyPalyer m1(60, 107, "1");
	MyPalyer m2(91, 118, "2");
	MyPalyer m3(147, 113, "3");
	w.addPalyer(&m1);
	w.addPalyer(&m2);
	w.addPalyer(&m3);
	auto s = w.getSrdPalyers(&m1);
	for (auto single : s) {
		cout << dynamic_cast<MyPalyer*>(single)->str << endl;
	}*/
	deamlize(); //守护进程
	random_name.loadFile(); //随机姓名池初始化
	ZinxKernel::ZinxKernelInit();

	ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GameConFact())));
	ZinxKernel::Zinx_Run();
	ZinxKernel::ZinxKernelFini();
	return 0;
}
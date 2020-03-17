#include "GameProtocol.h"
#include <iostream>
#include "GameMsg.h"
#include "GameChannel.h"
#include "GameRole.h"
using namespace std;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
	if (m_role != NULL) {
		ZinxKernel::Zinx_Del_Role(*m_role);
		delete m_role;
	}
}

UserData* GameProtocol::raw2request(std::string _szInput)
{
	MultiMsg* pret = new MultiMsg(); //这里一定要创建对象后面才能pushu
	mLast.append(_szInput);  //这里不要放到循环中去了
	while (1) {
		if (mLast.size() < 8) {
			break;
		}
		int length = 0, id = 0;
		length |= mLast[0] << 0;
		length |= mLast[1] << 8;
		length |= mLast[2] << 16;
		length |= mLast[3] << 24;
		id |= mLast[4] << 0;
		id |= mLast[5] << 8;
		id |= mLast[6] << 16;
		id |= mLast[7] << 24;

		if (mLast.size() - 8 < length) {	//后续报文是否合法
			break;
		}
		GameMsg* msg = new GameMsg((GameMsg::MSG_TYPE)id, mLast.substr(8, length));
		pret->m_Msg.push_back(msg);
		mLast.erase(0,8+length);//弹出处理完成的报文
	}
	//测试输出
	//for (auto single : pret->m_Msg) {
	//	cout << single->pMsg->Utf8DebugString() << endl; //utf8是protobuf种自带的
	//	
	//}
	return pret;
} 

std::string* GameProtocol::response2raw(UserData& _oUserData)
{
	int id = 0, msgLength = 0;
	string msgContent;
	auto pret = new string();
	GET_REF2DATA(GameMsg, output, _oUserData);
	id = output.enMsgType;
	msgContent = output.sarilize();	//序列化
	msgLength = msgContent.size();
	//pret[0] = (msgLength >> 0) & 0xff;	//这里没有数据不能用[]访问
	pret->push_back((msgLength >> 0) & 0xff);
	pret->push_back((msgLength >> 8) & 0xff);
	pret->push_back((msgLength >> 16) & 0xff);
	pret->push_back((msgLength >> 24) & 0xff);
	pret->push_back((id >> 0) & 0xff);
	pret->push_back((id >> 8) & 0xff);
	pret->push_back((id >> 16) & 0xff);
	pret->push_back((id >> 24) & 0xff);
	pret->append(msgContent);
	return pret;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
	return m_role;
}

Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{
	return m_channel;
}

#pragma once
#include <zinx.h>
#include <google/protobuf/message.h>
#include <list>
class GameMsg :
	public UserData
{
public:
	google::protobuf::Message *pMsg = NULL;
	enum MSG_TYPE
	{
		MSG_TYPE_LOGIN_ID_NAME = 1,
		MSG_TYPE_CHAT_CONTENT=2,
		MSG_TYPE_NEW_POS=3,
		MSG_TYPE_BRODCAST=200,
		MSG_TYPE_LOGOFF_ID_NAME=201,
		MSG_TYPE_SRD_POS=202
	} enMsgType;
	//�������е�ָ���ö�ٴ�����
	GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg); 
	GameMsg(MSG_TYPE _type, std::string _str);	//�����л�
	std::string sarilize();	//���л�
	virtual ~GameMsg();
};

class MultiMsg:public UserData {
public:
	std::list<GameMsg*> m_Msg;
	virtual ~MultiMsg();
};

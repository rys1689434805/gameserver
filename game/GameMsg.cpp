#include "GameMsg.h"
#include <iostream>
#include "msg.pb.h"

GameMsg::GameMsg(MSG_TYPE _type, google::protobuf::Message* _pMsg):enMsgType(_type),pMsg(_pMsg)
{
}

GameMsg::GameMsg(MSG_TYPE _type, std::string _str):enMsgType(_type)
{
	switch (_type)
	{
	case GameMsg::MSG_TYPE_LOGIN_ID_NAME:
		pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_CHAT_CONTENT:
		pMsg = new pb::Talk();
		break;
	case GameMsg::MSG_TYPE_NEW_POS:
		pMsg = new pb::Position();
		break;
	case GameMsg::MSG_TYPE_BRODCAST:
		pMsg = new pb::BroadCast();
		break;
	case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
		pMsg = new pb::SyncPid();
		break;
	case GameMsg::MSG_TYPE_SRD_POS:
		pMsg = new pb::SyncPlayers();
		break;
	default:
		break;
	}
	pMsg->ParseFromString(_str); //½âÎö
}

std::string GameMsg::sarilize()
{
	std::string ret;
	pMsg->SerializeToString(&ret); 
	return ret;
}

GameMsg::~GameMsg()
{
	if (this->pMsg != NULL) {
		delete pMsg;
	}
}

MultiMsg::~MultiMsg()
{
	for (auto single : this->m_Msg) {
		delete single;
	}
}

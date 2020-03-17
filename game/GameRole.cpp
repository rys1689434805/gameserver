#include "GameRole.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <hiredis/hiredis.h>
#include <random>
#include "msg.pb.h"
#include "GameProtocol.h"
#include "GameChannel.h"
#include "RandomName.h"

using namespace std;

static AoiWord word(0, 400, 20, 0, 400, 20); //全局地图
static default_random_engine random_engine(time(NULL)); //c++随机数种子,这里注意不要定义成为random
RandomName random_name;
GameRole::GameRole()
{
	this->szName = random_name.getName();
	x = 100 + random_engine() % 50;  //100-150之间
	z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
	random_name.releaseName(this->szName); //把姓名还回去
}

bool GameRole::Init()
{	
	bool ret = word.addPalyer(this);	//将自己添加到游戏世界
	if (ret == true) {
		this->pid = this->m_proto->m_channel->GetFd();
		auto msg = this->GreatLogin_id_name();
		ZinxKernel::Zinx_SendOut(*msg, *m_proto);	//1给自己发送服务器中的id和name
		msg = this->GreatSrdPlayer_pos();	
		ZinxKernel::Zinx_SendOut(*msg, *m_proto);//2向自己发送周围玩家的位置id和name
		auto pPalyer = word.getSrdPalyers(this); //获取自己周围玩家
		for (auto single : pPalyer) {
			auto pRole = dynamic_cast<GameRole*>(single);
			msg = this->GreatSelfPos();
			ZinxKernel::Zinx_SendOut(*msg, *(pRole->m_proto)); //发送给周围玩家
		}
	}
	auto context = redisConnect("127.0.0.1", 6379); //把玩家姓名存到redis,下线就删除
	if (context != NULL) {
		auto reply = redisCommand(context, "lpush gameName %s", this->szName.data());
		freeReplyObject(reply);
		redisFree(context);
	}
	return ret;
}

void GameRole::ProcTalkMsg(std::string _content)
{
	//发给所有人
	auto role_list = ZinxKernel::Zinx_GetAllRole();
	for (auto pRole : role_list)
	{
		auto pGameRole = dynamic_cast<GameRole*>(pRole);
		auto pmsg = this->GreatTalkBroadcast(_content);
		ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_proto));
	}
}

void GameRole::ProcMoveMsg(float _x, float _y, float _z, float _v)
{
	/*1.跨网格处理*/
	/*获取原来的邻居s1*/
	auto s1 = word.getSrdPalyers(this);
	/*摘出旧格子,更新坐标,添加新格子，获取新邻居s2*/
	word.removePalyer(this);
	x = _x;
	y = _y;
	z = _z;
	v = _v;
	word.addPalyer(this);
	auto s2 = word.getSrdPalyers(this);
	/*遍历s2，若元素不属于s1, 视野出现*/
	for (auto single_player : s2)
	{
		if (s1.end() == find(s1.begin(), s1.end(), single_player))
		{
			//视野出现
			ViewAppear(dynamic_cast<GameRole*>(single_player));
		}
	}
	/*遍历s1，若元素不属于s2，视野消失*/
	for (auto single_player : s1)
	{
		if (s2.end() == find(s2.begin(), s2.end(), single_player))
		{
			//视野消失
			ViewLost(dynamic_cast<GameRole*>(single_player));
		}
	}

	/*2.广播新位置给周围玩家*/  
	auto srd_list = word.getSrdPalyers(this);
 	for (auto single : srd_list)  //遍历周围玩家发送新的位置
	{
		//组成待发送的报文
		pb::BroadCast* pMsg = new pb::BroadCast();
		auto pPos = pMsg->mutable_p();
		pPos->set_x(_x);
		pPos->set_y(_y);
		pPos->set_z(_z);
		pPos->set_v(_v);
		pMsg->set_pid(this->pid);
		pMsg->set_tp(4);
		pMsg->set_username(szName);
		auto pRole = dynamic_cast<GameRole*>(single);
		ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BRODCAST, pMsg)), *(pRole->m_proto));
	}
}


UserData* GameRole::ProcMsg(UserData& _poUserData)
{
	GET_REF2DATA(MultiMsg, muti_msg, _poUserData);
	for (auto single : muti_msg.m_Msg) {
		cout << " 消息格式： " << single->enMsgType << endl;
		cout << single->pMsg->Utf8DebugString() << endl;
		
		auto NewPos = dynamic_cast<pb::Position*>(single->pMsg);
		switch (single->enMsgType)
		{
		case GameMsg::MSG_TYPE_CHAT_CONTENT:  //如果是聊天类容
			ProcTalkMsg(dynamic_cast<pb::Talk*>(single->pMsg)->content());
			break;
		case GameMsg::MSG_TYPE_NEW_POS:   //如果是新位置
			ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
			break;
		default:
			break;
		}
	}
	return nullptr;
}

void GameRole::Fini()
{
	auto list_palyer = word.getSrdPalyers(this);
	for (auto single : list_palyer) {
		auto srd_palyer = dynamic_cast<GameRole*>(single);
		auto msg = GreatLogoff_id_name();
		ZinxKernel::Zinx_SendOut(*msg, *(srd_palyer->m_proto)); //给周围玩家发送下线
	}
	word.removePalyer(this);
	auto red = redisConnect("127.0.0.1", 6379);
	if (red != NULL) {
		auto ret = redisCommand(red, "lrem gameName 1 %s", this->szName.c_str());
		freeReplyObject(ret);
		redisFree(red);
	}
}

int GameRole::getX()
{
	return (int)x;
}

int GameRole::getY()
{
	return (int)y;
}

GameMsg* GameRole::GreatLogin_id_name()
{
	pb::SyncPid* _msg = new pb::SyncPid();
	_msg->set_pid(this->pid);
	_msg->set_username(this->szName);
	GameMsg* ret = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, _msg); 
	return ret;
}

GameMsg* GameRole::GreatSrdPlayer_pos()
{
	pb::SyncPlayers* srd_player = new pb::SyncPlayers();
	auto lis = word.getSrdPalyers(this);
	for (auto single : lis) {
		auto pPlayer = srd_player->add_ps();	//protobuf数组
		auto pRole = dynamic_cast<GameRole*>(single);
		pPlayer->set_pid(pRole->pid);	//设置周围玩家的id
		pPlayer->set_username(pRole->szName);	//周围玩家的name
		auto pPos = pPlayer->mutable_p();	//protobuf内嵌类
		pPos->set_x(pRole->x);
		pPos->set_y(pRole->y);
		pPos->set_z(pRole->z);
		pPos->set_v(pRole->v);
	}
	GameMsg* ret = new GameMsg(GameMsg::MSG_TYPE_SRD_POS, srd_player);
	//m_ptr = make_shared<GameMsg>(GameMsg::MSG_TYPE_SRD_POS, srd_player);
	return ret;
}

GameMsg* GameRole::GreatSelfPos()
{
	pb::BroadCast* msg = new pb::BroadCast();
	msg->set_pid(this->pid);
	msg->set_username(this->szName);
	msg->set_tp(2);
	auto pPos = msg->mutable_p();
	pPos->set_x(this->x);
	pPos->set_y(this->y);
	pPos->set_z(this->z);
	pPos->set_v(this->v);
	GameMsg *ret = new GameMsg(GameMsg::MSG_TYPE_BRODCAST, msg);
	return ret;
}

GameMsg* GameRole::GreatLogoff_id_name()
{
	pb::SyncPid* msg = new pb::SyncPid();
	msg->set_pid(this->pid);
	msg->set_username(this->szName);
	GameMsg* ret = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, msg);
	return ret;
}

GameMsg* GameRole::GreatTalkBroadcast(std::string _content)
{
	auto msg = new pb::BroadCast();
	msg->set_content(_content);
	msg->set_pid(this->pid);
	msg->set_tp(1); //设置1表示聊天消息
	msg->set_username(this->szName);
	GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BRODCAST, msg);
	return pret;
}

//视野出现
void GameRole::ViewAppear(GameRole* _role)
{
	//给自己发送参数的200号消息
	auto msg = _role->GreatSelfPos();
	ZinxKernel::Zinx_SendOut(*msg, *m_proto);
	//给参数玩家发送自己的200号消息
	msg = this->GreatSelfPos();
	ZinxKernel::Zinx_SendOut(*msg, *(_role->m_proto));
}
//视野消失
void GameRole::ViewLost(GameRole* _role)
{
	//给自己发送参数的201号消息
	auto msg = _role->GreatLogoff_id_name();
	ZinxKernel::Zinx_SendOut(*msg, *m_proto);
	//给参数玩家发送自己的201号消息
	msg = this->GreatLogoff_id_name();
	ZinxKernel::Zinx_SendOut(*msg, *(_role->m_proto));
}

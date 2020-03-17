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

static AoiWord word(0, 400, 20, 0, 400, 20); //ȫ�ֵ�ͼ
static default_random_engine random_engine(time(NULL)); //c++���������,����ע�ⲻҪ�����Ϊrandom
RandomName random_name;
GameRole::GameRole()
{
	this->szName = random_name.getName();
	x = 100 + random_engine() % 50;  //100-150֮��
	z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
	random_name.releaseName(this->szName); //����������ȥ
}

bool GameRole::Init()
{	
	bool ret = word.addPalyer(this);	//���Լ���ӵ���Ϸ����
	if (ret == true) {
		this->pid = this->m_proto->m_channel->GetFd();
		auto msg = this->GreatLogin_id_name();
		ZinxKernel::Zinx_SendOut(*msg, *m_proto);	//1���Լ����ͷ������е�id��name
		msg = this->GreatSrdPlayer_pos();	
		ZinxKernel::Zinx_SendOut(*msg, *m_proto);//2���Լ�������Χ��ҵ�λ��id��name
		auto pPalyer = word.getSrdPalyers(this); //��ȡ�Լ���Χ���
		for (auto single : pPalyer) {
			auto pRole = dynamic_cast<GameRole*>(single);
			msg = this->GreatSelfPos();
			ZinxKernel::Zinx_SendOut(*msg, *(pRole->m_proto)); //���͸���Χ���
		}
	}
	auto context = redisConnect("127.0.0.1", 6379); //����������浽redis,���߾�ɾ��
	if (context != NULL) {
		auto reply = redisCommand(context, "lpush gameName %s", this->szName.data());
		freeReplyObject(reply);
		redisFree(context);
	}
	return ret;
}

void GameRole::ProcTalkMsg(std::string _content)
{
	//����������
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
	/*1.��������*/
	/*��ȡԭ�����ھ�s1*/
	auto s1 = word.getSrdPalyers(this);
	/*ժ���ɸ���,��������,����¸��ӣ���ȡ���ھ�s2*/
	word.removePalyer(this);
	x = _x;
	y = _y;
	z = _z;
	v = _v;
	word.addPalyer(this);
	auto s2 = word.getSrdPalyers(this);
	/*����s2����Ԫ�ز�����s1, ��Ұ����*/
	for (auto single_player : s2)
	{
		if (s1.end() == find(s1.begin(), s1.end(), single_player))
		{
			//��Ұ����
			ViewAppear(dynamic_cast<GameRole*>(single_player));
		}
	}
	/*����s1����Ԫ�ز�����s2����Ұ��ʧ*/
	for (auto single_player : s1)
	{
		if (s2.end() == find(s2.begin(), s2.end(), single_player))
		{
			//��Ұ��ʧ
			ViewLost(dynamic_cast<GameRole*>(single_player));
		}
	}

	/*2.�㲥��λ�ø���Χ���*/  
	auto srd_list = word.getSrdPalyers(this);
 	for (auto single : srd_list)  //������Χ��ҷ����µ�λ��
	{
		//��ɴ����͵ı���
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
		cout << " ��Ϣ��ʽ�� " << single->enMsgType << endl;
		cout << single->pMsg->Utf8DebugString() << endl;
		
		auto NewPos = dynamic_cast<pb::Position*>(single->pMsg);
		switch (single->enMsgType)
		{
		case GameMsg::MSG_TYPE_CHAT_CONTENT:  //�������������
			ProcTalkMsg(dynamic_cast<pb::Talk*>(single->pMsg)->content());
			break;
		case GameMsg::MSG_TYPE_NEW_POS:   //�������λ��
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
		ZinxKernel::Zinx_SendOut(*msg, *(srd_palyer->m_proto)); //����Χ��ҷ�������
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
		auto pPlayer = srd_player->add_ps();	//protobuf����
		auto pRole = dynamic_cast<GameRole*>(single);
		pPlayer->set_pid(pRole->pid);	//������Χ��ҵ�id
		pPlayer->set_username(pRole->szName);	//��Χ��ҵ�name
		auto pPos = pPlayer->mutable_p();	//protobuf��Ƕ��
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
	msg->set_tp(1); //����1��ʾ������Ϣ
	msg->set_username(this->szName);
	GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BRODCAST, msg);
	return pret;
}

//��Ұ����
void GameRole::ViewAppear(GameRole* _role)
{
	//���Լ����Ͳ�����200����Ϣ
	auto msg = _role->GreatSelfPos();
	ZinxKernel::Zinx_SendOut(*msg, *m_proto);
	//��������ҷ����Լ���200����Ϣ
	msg = this->GreatSelfPos();
	ZinxKernel::Zinx_SendOut(*msg, *(_role->m_proto));
}
//��Ұ��ʧ
void GameRole::ViewLost(GameRole* _role)
{
	//���Լ����Ͳ�����201����Ϣ
	auto msg = _role->GreatLogoff_id_name();
	ZinxKernel::Zinx_SendOut(*msg, *m_proto);
	//��������ҷ����Լ���201����Ϣ
	msg = this->GreatLogoff_id_name();
	ZinxKernel::Zinx_SendOut(*msg, *(_role->m_proto));
}

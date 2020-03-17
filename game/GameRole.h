#pragma once
#include <zinx.h>
#include "AoiWord.h"
#include "GameMsg.h"
#include <memory>
#include "msg.pb.h"
class GameProtocol;
class GameRole :
	public Irole ,public Palyer
{
	float x = 0, y = 0, z = 0, v = 0;
	int pid = 0;
	std::string szName;
	//std::shared_ptr<GameMsg> m_ptr;

	GameMsg* GreatLogin_id_name();
	GameMsg* GreatSrdPlayer_pos();
	GameMsg* GreatSelfPos();
	GameMsg* GreatLogoff_id_name();
	GameMsg* GreatTalkBroadcast(std::string _content);
	void ViewAppear(GameRole *_role);
	void ViewLost(GameRole* _role);
	void ProcTalkMsg(std::string _content);
	void ProcMoveMsg(float _x, float _y, float _z, float _v);
public:
	GameProtocol* m_proto = NULL;
	GameRole();
	virtual ~GameRole();
	// 通过 Irole 继承
	virtual bool Init() override;
	virtual UserData* ProcMsg(UserData& _poUserData) override;
	virtual void Fini() override;

	// 通过 Palyer 继承
	virtual int getX() override;
	virtual int getY() override;
	
};


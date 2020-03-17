#pragma once
#include <zinx.h>
class GameChannel; //声明通道类
class GameRole;
class GameProtocol :
	public Iprotocol
{
	std::string mLast;
public:
	GameChannel* m_channel = NULL;
	GameRole* m_role = NULL;
	GameProtocol();
	virtual ~GameProtocol();
	// 通过 Iprotocol 继承
	virtual UserData* raw2request(std::string _szInput) override;
	virtual std::string* response2raw(UserData& _oUserData) override;
	virtual Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
	virtual Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};


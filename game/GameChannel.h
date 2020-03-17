#pragma once
#include <ZinxTCP.h>
#include "GameProtocol.h"
#include "GameRole.h"
class GameChannel :
	public ZinxTcpData
{
public:
	GameChannel(int fd);
	virtual ~GameChannel();
	GameProtocol* mProtocol = NULL;
	// 返回协议对象
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GameConFact : public IZinxTcpConnFact {
	// 通过 IZinxTcpConnFact 继承
	virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};


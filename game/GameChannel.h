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
	// ����Э�����
	virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};

class GameConFact : public IZinxTcpConnFact {
	// ͨ�� IZinxTcpConnFact �̳�
	virtual ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};


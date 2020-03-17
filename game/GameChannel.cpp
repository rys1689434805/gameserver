#include "GameChannel.h"

GameChannel::GameChannel(int fd) :ZinxTcpData(fd)
{
}

GameChannel::~GameChannel()
{
	if (mProtocol != NULL) {
		ZinxKernel::Zinx_Del_Proto(*mProtocol); //��ժ��
		delete mProtocol;
	}
}

AZinxHandler* GameChannel::GetInputNextStage(BytesMsg& _oInput)
{
	return mProtocol;
}

ZinxTcpData* GameConFact::CreateTcpDataChannel(int _fd)
{
	auto pChanal = new GameChannel(_fd);
	auto pProtocol = new GameProtocol();
	auto pRole = new GameRole();
	pChanal->mProtocol = pProtocol;
	pProtocol->m_channel = pChanal;
	pProtocol->m_role = pRole;
	pRole->m_proto = pProtocol;
	//���Э�����
	ZinxKernel::Zinx_Add_Proto(*pProtocol);
	ZinxKernel::Zinx_Add_Role(*pRole);
	return pChanal;
}

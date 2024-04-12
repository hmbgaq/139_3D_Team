#pragma once
#include "Player_State_Normal.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CPlayer_ZipLine_Start : public CPlayer_State_Normal
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	static const _uint g_iAnimIndex = CPlayer::Player_State::Player_ZipLine_Start;

private:
	_float3	  m_vZipLineDir = {};
	CNavigation* m_pActorNavigation = { nullptr };
	_float4	  m_vArrivalPosition = {};
	
};

END
#pragma once
#include "State.h"
#include "Player.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CPlayer_State abstract : public CState<CPlayer>
{
public:
	virtual void Initialize(CPlayer* pActor) override;
	virtual CState<CPlayer>* Update(CPlayer* pActor, _float fTimeDelta) override;
	virtual void Release(CPlayer* pActor) override;

public:
	virtual CState<CPlayer>* Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:
	CState<CPlayer>* Normal_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Attack_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Run_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Dodge_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Roll_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);


public:
	CState<CPlayer>* Normal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Run(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Dodge(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* Attack(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CPlayer>* MeleeCombo(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);



public:
	_int iKeyUp		= DIK_UP;
	_int iKeyDown	= DIK_DOWN;
	_int iKeyLeft	= DIK_LEFT;
	_int iKeyRight	= DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
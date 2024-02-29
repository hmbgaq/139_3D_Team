#pragma once
#include "State.h"
#include "Infected.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CInfected_State abstract : public CState<CInfected>
{
public:
	virtual void Initialize(CInfected* pActor) override;
	virtual CState<CInfected>* Update(CInfected* pActor, _float fTimeDelta) override;
	virtual void Release(CInfected* pActor) override;

public:
	virtual CState<CInfected>* Update_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:		//상태 그룹 별 업데이트 
	CState<CInfected>* Normal_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Walk_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Run_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Attack_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Hit_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Knock_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Dodge_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Spawn_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Death_State(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);


public:		//개별 동작 상태 집합
	CState<CInfected>* Normal(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Walk(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Run(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Attack(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Hit(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Knock(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Dodge(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Spawn(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CInfected>* Death(CInfected* pActor, _float fTimeDelta, _uint _iAnimIndex);




public:
	_int iKeyUp = DIK_W;	//DIK_UP;
	_int iKeyDown = DIK_S;	//DIK_DOWN;
	_int iKeyLeft = DIK_A;	//DIK_LEFT;
	_int iKeyRight = DIK_D;	//DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
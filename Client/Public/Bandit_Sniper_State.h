#pragma once
#include "State.h"
#include "Bandit_Sniper.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CBandit_Sniper_State abstract : public CState<CBandit_Sniper>
{
public:
	virtual void Initialize(CBandit_Sniper* pActor) override;
	virtual CState<CBandit_Sniper>* Update(CBandit_Sniper* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Sniper* pActor) override;

public:
	virtual CState<CBandit_Sniper>* Update_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:		//상태 그룹 별 업데이트 
	CState<CBandit_Sniper>* Normal_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Walk_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Run_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Attack_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Hit_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Knock_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Dodge_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Spawn_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Death_State(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);


public:		//개별 동작 상태 집합
	CState<CBandit_Sniper>* Normal(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Walk(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Run(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Attack(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Sniper>* Dodge(CBandit_Sniper* pActor, _float fTimeDelta, _uint _iAnimIndex);



public:
	_int iKeyUp = DIK_W;	//DIK_UP;
	_int iKeyDown = DIK_S;	//DIK_DOWN;
	_int iKeyLeft = DIK_A;	//DIK_LEFT;
	_int iKeyRight = DIK_D;	//DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
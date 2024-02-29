#pragma once
#include "State.h"
#include "Bandit_Heavy.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CBandit_Heavy_State abstract : public CState<CBandit_Heavy>
{
public:
	virtual void Initialize(CBandit_Heavy* pActor) override;
	virtual CState<CBandit_Heavy>* Update(CBandit_Heavy* pActor, _float fTimeDelta) override;
	virtual void Release(CBandit_Heavy* pActor) override;

public:
	virtual CState<CBandit_Heavy>* Update_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:		//상태 그룹 별 업데이트 
	CState<CBandit_Heavy>* Normal_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Walk_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Run_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Attack_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Hit_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Knock_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Death_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);


	CState<CBandit_Heavy>* Dodge_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Spawn_State(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);




public:		//개별 동작 상태 집합
	CState<CBandit_Heavy>* Normal(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CBandit_Heavy>* Idle(CBandit_Heavy* pActor, _float fTimeDelta, _uint _iAnimIndex);



public:
	_int iKeyUp = DIK_W;	//DIK_UP;
	_int iKeyDown = DIK_S;	//DIK_DOWN;
	_int iKeyLeft = DIK_A;	//DIK_LEFT;
	_int iKeyRight = DIK_D;	//DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
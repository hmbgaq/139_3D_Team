#pragma once
#include "State.h"
#include "VampireCommander.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CVampireCommander_State abstract : public CState<CVampireCommander>
{
public:
	virtual void Initialize(CVampireCommander* pActor) override;
	virtual CState<CVampireCommander>* Update(CVampireCommander* pActor, _float fTimeDelta) override;
	virtual void Release(CVampireCommander* pActor) override;

public:
	virtual CState<CVampireCommander>* Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:		//상태 그룹 별 업데이트 
	CState<CVampireCommander>* Normal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Walk_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Attack_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* HitNormal_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Stun_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Taunt_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Spawn_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Dead_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);







public:		//개별 동작 상태 집합
	CState<CVampireCommander>* Normal(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CVampireCommander>* Idle(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex);



public:
	_int iKeyUp = DIK_W;	//DIK_UP;
	_int iKeyDown = DIK_S;	//DIK_DOWN;
	_int iKeyLeft = DIK_A;	//DIK_LEFT;
	_int iKeyRight = DIK_D;	//DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
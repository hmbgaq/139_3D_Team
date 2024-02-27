#pragma once
#include "State.h"
#include "Assassin.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CAssassin_State abstract : public CState<CAssassin>
{
public:
	virtual void Initialize(CAssassin* pActor) override;
	virtual CState<CAssassin>* Update(CAssassin* pActor, _float fTimeDelta) override;
	virtual void Release(CAssassin* pActor) override;

public:
	virtual CState<CAssassin>* Update_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;



public:		//상태 그룹 별 업데이트 
	CState<CAssassin>* Normal_State(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex);




public:		//개별 동작 상태 집합
	CState<CAssassin>* Normal(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CAssassin>* Idle(CAssassin* pActor, _float fTimeDelta, _uint _iAnimIndex);



public:
	_int iKeyUp = DIK_W;	//DIK_UP;
	_int iKeyDown = DIK_S;	//DIK_DOWN;
	_int iKeyLeft = DIK_A;	//DIK_LEFT;
	_int iKeyRight = DIK_D;	//DIK_RIGHT;


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
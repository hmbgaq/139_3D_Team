#pragma once
#include "State.h"
#include "Hawk.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CHawk_State abstract : public CState<CHawk>
{
public:
	virtual void Initialize(CHawk* pActor) override;
	virtual CState<CHawk>* Update(CHawk* pActor, _float fTimeDelta) override;
	virtual void Release(CHawk* pActor) override;

public:
	virtual CState<CHawk>* Update_State(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;


//public:		//상태 그룹 별 업데이트 
//	CState<CHawk>* Normal_State(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex);
//
//
//public:		//개별 동작 상태 집합
//	CState<CHawk>* Normal(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex);
//	CState<CHawk>* Walk(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex);
//	CState<CHawk>* Run(CHawk* pActor, _float fTimeDelta, _uint _iAnimIndex);


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
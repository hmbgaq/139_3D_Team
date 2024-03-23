#pragma once
#include "State.h"
#include "Mother.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMother_State abstract : public CState<CMother>
{
public:
	virtual void Initialize(CMother* pActor) override;
	virtual CState<CMother>* Update(CMother* pActor, _float fTimeDelta) override;
	virtual void Release(CMother* pActor) override;

public:
	virtual CState<CMother>* Update_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;

public:		//상태 그룹 별 업데이트 
	CState<CMother>* Normal_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CMother>* Attack_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CMother>* Stun_State(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex);


public:		//개별 동작 상태 집합
	CState<CMother>* Normal(CMother* pActor, _float fTimeDelta, _uint _iAnimIndex);

protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
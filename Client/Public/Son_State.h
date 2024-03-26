#pragma once
#include "State.h"
#include "Son.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CSon_State abstract : public CState<CSon>
{
public:
	virtual void Initialize(CSon* pActor) override;
	virtual CState<CSon>* Update(CSon* pActor, _float fTimeDelta) override;
	virtual void Release(CSon* pActor) override;

public:
	virtual CState<CSon>* Update_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;

public:		//상태 그룹 별 업데이트 
	CState<CSon>* Normal_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CSon>* Attack_State(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex);

public:		//개별 동작 상태 집합
	CState<CSon>* Normal(CSon* pActor, _float fTimeDelta, _uint _iAnimIndex);

protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
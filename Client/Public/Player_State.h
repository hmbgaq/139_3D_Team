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
	CState<CPlayer>* MeleeCombo(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);

	//CState<CPlayer>* Ground_Normal(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);

	//CState<CPlayer>* Run(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);
	//CState<CPlayer>* Roll(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);

	//CState<CPlayer>* Hitted(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex);


public:
	virtual CState<CPlayer>* Update_State(CPlayer* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
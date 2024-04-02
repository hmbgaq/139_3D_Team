#pragma once
#include "State.h"
#include "Edgar.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CEdgar_State abstract : public CState<CEdgar>
{
public:
	virtual void Initialize(CEdgar* pActor) override;
	virtual CState<CEdgar>* Update(CEdgar* pActor, _float fTimeDelta) override;
	virtual void Release(CEdgar* pActor) override;

public:
	virtual CState<CEdgar>* Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex) PURE;


public:		//상태 그룹 별 업데이트 
	CState<CEdgar>* Normal_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Walk_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Walk_Aim_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Run_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_High_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_High_Left_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_High_Right_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);

	CState<CEdgar>* Cover_Low_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_Low_Over_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_Low_Left_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Cover_Low_Right_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);

	CState<CEdgar>* Crouch_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);

	CState<CEdgar>* Hit_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* CutScene_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);

	


public:		//개별 동작 상태 집합
	CState<CEdgar>* Normal(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Walk(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);
	CState<CEdgar>* Run(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex);


protected:
	CGameInstance* m_pGameInstance = { nullptr };
};

END
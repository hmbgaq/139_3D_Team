#include "..\Public\Edgar_State.h"
#include "GameInstance.h"


void CEdgar_State::Initialize(CEdgar* pActor)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

}

CState<CEdgar>* CEdgar_State::Update(CEdgar* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CEdgar_State::Release(CEdgar* pActor)
{
	Safe_Release(m_pGameInstance);
}

CState<CEdgar>* CEdgar_State::Normal_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Normal(pActor, fTimeDelta, _iAnimIndex);
}

CState<CEdgar>* CEdgar_State::Walk_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Walk_Aim_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Run_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_High_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_High_Left_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_High_Right_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_Low_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_Low_Over_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_Low_Left_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Cover_Low_Right_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Crouch_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Hit_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::CutScene_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Normal(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{


	return nullptr;
}

CState<CEdgar>* CEdgar_State::Walk(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

CState<CEdgar>* CEdgar_State::Run(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return nullptr;
}

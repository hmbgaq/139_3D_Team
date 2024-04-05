#include "..\Public\Edgar_State_CutScene.h"

void CEdgar_State_CutScene::Initialize(CEdgar* pActor)
{
	__super::Initialize(pActor);
}

CState<CEdgar>* CEdgar_State_CutScene::Update(CEdgar* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CEdgar_State_CutScene::Release(CEdgar* pActor)
{
	__super::Release(pActor);
}

CState<CEdgar>* CEdgar_State_CutScene::Update_State(CEdgar* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return CutScene_State(pActor, fTimeDelta, _iAnimIndex);
}

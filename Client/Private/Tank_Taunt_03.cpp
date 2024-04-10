#include "..\Public\Tank_Taunt_03.h"

void CTank_Taunt_03::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Look_At_Target();

}

CState<CTank>* CTank_Taunt_03::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	return nullptr;


}

void CTank_Taunt_03::Release(CTank* pActor)
{
	__super::Release(pActor);
}

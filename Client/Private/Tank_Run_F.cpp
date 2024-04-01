#include "..\Public\Tank_Run_F.h"

void CTank_Run_F::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);

}

CState<CTank>* CTank_Run_F::Update(CTank* pActor, _float fTimeDelta)
{
	//CCharacter* pTarget = pActor->Get_Target();
	//if (pTarget)
	//{
	//	pActor->Look_At_Target();
	//}

	pActor->Look_At_Target_Lerp(fTimeDelta);

	pActor->Go_Straight(fTimeDelta * 0.5f);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CTank_Run_F::Release(CTank* pActor)
{
	__super::Release(pActor);
}

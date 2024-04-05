#include "..\Public\Player_Winchester_LowerHolster.h"
#include "GameInstance.h"
#

void CPlayer_Winchester_LowerHolster::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation_Upper(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, 80);
	pActor->Set_Splitted(true);
	//pActor->Set_StiffnessRate_Upper(0.4f);
}

CState<CPlayer>* CPlayer_Winchester_LowerHolster::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	pActor->Aim_Walk(fTimeDelta);


	//if (pActor->Is_UpperAnimation_End())
	//{
	//	return Normal(pActor, fTimeDelta, g_iAnimIndex);
	//}

	//return nullptr;

	return Normal(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_Winchester_LowerHolster::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Splitted(false);
	//pActor->Set_StiffnessRate_Upper(1.f);

	pActor->Set_Weapon_Enable(PLAYER_WEAPON_WINCHESTER, false);

	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_CameraOffset(_float3(1.f, 0.5f, -3.f));
	}
}

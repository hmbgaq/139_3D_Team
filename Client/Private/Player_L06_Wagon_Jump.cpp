#include "..\Public\Player_L06_Wagon_Jump.h"

void CPlayer_L06_Wagon_Jump::Initialize(CPlayer* pActor)
{
	//__super::Initialize(pActor);

	_vector vPos = XMLoadFloat3(&pActor->Calc_Front_Pos(_float3(-1.f, 0.f, 0.4f)));
	pActor->Look_At_OnLand(vPos);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	CWeapon* pWeapon = pActor->Set_Weapon_Enable(PLAYER_WEAPON_WHIP, true);
	pWeapon->Set_Animation(0, CModel::ANIM_STATE::ANIM_STATE_NORMAL, 0);
}

CState<CPlayer>* CPlayer_L06_Wagon_Jump::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_L06_Wagon_Jump::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_Weapon_Enable(PLAYER_WEAPON_WHIP, false);
}

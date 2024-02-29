#include "..\Public\ManHeavy_KnockFrontCannonball_F_02.h"

void CManHeavy_KnockFrontCannonball_F_02::Initialize(CBandit_Heavy* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CBandit_Heavy>* CManHeavy_KnockFrontCannonball_F_02::Update(CBandit_Heavy* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CManHeavy_KnockFrontCannonball_F_02::Release(CBandit_Heavy* pActor)
{
	__super::Release(pActor);
}

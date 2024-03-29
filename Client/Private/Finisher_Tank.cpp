#include "..\Public\Finisher_Tank.h"

void CFinisher_Tank::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CTank>* CFinisher_Tank::Update(CTank* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		pActor->Set_Dead(true);
	}

	return nullptr;
	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CFinisher_Tank::Release(CTank* pActor)
{
	__super::Release(pActor);
}

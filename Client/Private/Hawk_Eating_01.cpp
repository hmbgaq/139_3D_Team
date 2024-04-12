#include "..\Public\Hawk_Eating_01.h"
#include "Collider.h"


void CHawk_Eating_01::Initialize(CHawk* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

}

CState<CHawk>* CHawk_Eating_01::Update(CHawk* pActor, _float fTimeDelta)
{
	return nullptr;
}

void CHawk_Eating_01::Release(CHawk* pActor)
{
	__super::Release(pActor);
}

#include "..\Public\VampireCommander_Walk_F.h"

void CVampireCommander_Walk_F::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	pActor->m_bLookAt = true;

}

CState<CVampireCommander>* CVampireCommander_Walk_F::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	pActor->Go_Straight(fTimeDelta * 0.5f);


	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_Walk_F::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

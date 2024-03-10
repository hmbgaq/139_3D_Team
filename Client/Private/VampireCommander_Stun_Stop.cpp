#include "VampireCommander_Stun_Stop.h"

void CVampireCommander_Stun_Stop::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_Stun_Stop::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	//만약에 Stun 내에서 끝장을 내지 못하면 요기로 타는거고 아니라면 cut scene으로 처리 할거임
	//거의 쓸 일 없을거 같은데 이거 
	return nullptr;
}

void CVampireCommander_Stun_Stop::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

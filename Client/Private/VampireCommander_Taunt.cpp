#include "VampireCommander_Taunt.h"
#include "Effect_Manager.h"

void CVampireCommander_Taunt::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	
	m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "MonsterSoundWaveVampire.json", pActor, true, "Head");

}

CState<CVampireCommander>* CVampireCommander_Taunt::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Taunt::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);

	EFFECT_MANAGER->Return_ToPool(m_pEffect);
// 	if (m_pEffect != nullptr)
// 		Safe_Release(m_pEffect);
}

CState<CVampireCommander>* CVampireCommander_Taunt::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return Taunt_State(pActor, fTimeDelta, _iAnimIndex);
	
}

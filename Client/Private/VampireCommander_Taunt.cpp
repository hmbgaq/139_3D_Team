#include "VampireCommander_Taunt.h"
#include "MasterCamera.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"

void CVampireCommander_Taunt::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	
	m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "MonsterSoundWaveVampire.json", pActor, true, "Head");
	CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
	pSpringCam->Set_ShakeCameraTime(0.5f);
	pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.2f));
	pActor->Apply_Shake_And_Blur(Power::Medium);
	pActor->m_bAuraStart = true;
	m_pGameInstance->Play_Sound(L"VAMPIRE_TAUNT", L"commander_lesser_vo_taunt001.wav", SOUND_ENEMY_VOICE, 10.f);

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

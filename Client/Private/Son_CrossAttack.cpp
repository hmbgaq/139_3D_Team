#include "Son_CrossAttack.h"
#include "Son_CrossAttackLoop.h"
#include "GameInstance.h"

void CSon_CrossAttack::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bLookAt = false;
	pActor->Get_Weapon(L"Weapon_head")->Set_Enable_Collisions(true);

	pActor->Set_MonsterAttackState(true);

	_int iRandomSound = m_pGameInstance->Random_Int(1, 3);
	wstring strSoundKey = L"";

	switch (iRandomSound)
	{
	case 1:
		strSoundKey = L"Tentacle_Attack_01";
		break;
	case 2:
		strSoundKey = L"Tentacle_Attack_02";
		break;
	case 3:
		strSoundKey = L"Tentacle_Attack_03";
		break;
	}

	m_pGameInstance->Play_Sound(L"SON_ATTACK", strSoundKey, SOUND_ENEMY_ATTACK2, 7.f);
}

CState<CSon>* CSon_CrossAttack::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_CrossAttackLoop;
	}

	return nullptr;
}

void CSon_CrossAttack::Release(CSon* pActor)
{
	__super::Release(pActor);
}

#include "Son_LongAttack.h"
#include "Son_Idle.h"
#include "GameInstance.h"

void CSon_LongAttack::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->m_bLookAt = false;
	pActor->Get_Weapon(L"Weapon_head")->Set_Enable_Collisions(true);
	pActor->Set_MonsterAttackState(true);

	_int iRandomSound = m_pGameInstance->Random_Int(1, 5);
	wstring strSoundKey = L"";

	switch (iRandomSound)
	{
	case 1:
		strSoundKey = L"grand_parasiter_worms_vo_attack_slam001";
		break;
	case 2:
		strSoundKey = L"grand_parasiter_worms_vo_attack_slam002";
		break;
	case 3:
		strSoundKey = L"grand_parasiter_worms_vo_attack_slam003";
		break;
	case 4:
		strSoundKey = L"grand_parasiter_worms_vo_attack_slam004";
		break;
	case 5:
		strSoundKey = L"grand_parasiter_worms_vo_attack_slam005";
		break;
	}

	m_pGameInstance->Play_Sound(L"SON_ATTACK", strSoundKey, SOUND_ENEMY_ATTACK2, 7.f);
}

CState<CSon>* CSon_LongAttack::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}
	
void CSon_LongAttack::Release(CSon* pActor)
{
	__super::Release(pActor);
}

#include "Son_ShortAttack.h"
#include "Son_Idle.h"
#include "GameInstance.h"
void CSon_ShortAttack::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Get_Weapon(L"Weapon_head")->Set_Enable_Collisions(true);
	pActor->Set_MonsterAttackState(true);

	_int iRandomSound = m_pGameInstance->Random_Int(1, 5);
	wstring strSoundKey = L"";

	switch (iRandomSound)
	{
	case 1:
		strSoundKey = L"grand_parasiter_worms_attack_whoosh001.wav";
		break;
	case 2:
		strSoundKey = L"grand_parasiter_worms_attack_whoosh002.wav";
		break;
	case 3:
		strSoundKey = L"grand_parasiter_worms_attack_whoosh003.wav";
		break;
	case 4:
		strSoundKey = L"grand_parasiter_worms_attack_whoosh004.wav";
		break;
	case 5:
		strSoundKey = L"grand_parasiter_worms_attack_whoosh005.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"SON_ATTACK", strSoundKey, SOUND_ENEMY_ATTACK2, 7.f);
}

CState<CSon>* CSon_ShortAttack::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_ShortAttack::Release(CSon* pActor)
{
	__super::Release(pActor);
}

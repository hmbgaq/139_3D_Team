#include "VampireCommander_Hit.h"
#include "GameInstance.h"

void CVampireCommander_Hit::Initialize(CVampireCommander* pActor)
{

	__super::Initialize(pActor);

	wstring strSoundKey = L"";

	_int iRandomSound = m_pGameInstance->Random_Int(1, 7);
	switch (iRandomSound)
	{
	case 1:
		strSoundKey = L"commander_lesser_vo_hit001.wav";
		break;
	case 2:
		strSoundKey = L"commander_lesser_vo_hit002.wav";
		break;
	case 3:
		strSoundKey = L"commander_lesser_vo_hit003.wav";
		break;
	case 4:
		strSoundKey = L"commander_lesser_vo_hit004.wav";
		break;
	case 5:
		strSoundKey = L"commander_lesser_vo_hit005.wav";
		break;

	case 6:
		strSoundKey = L"commander_lesser_vo_hit006.wav";
		break;

	case 7:
		strSoundKey = L"commander_lesser_vo_hit007.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"VAMPIRE_HIT", strSoundKey, SOUND_ENEMY_HIT, 6.5f);
}

CState<CVampireCommander>* CVampireCommander_Hit::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	return nullptr;
}

void CVampireCommander_Hit::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

CState<CVampireCommander>* CVampireCommander_Hit::Update_State(CVampireCommander* pActor, _float fTimeDelta, _uint _iAnimIndex)
{
	return HitNormal_State(pActor, fTimeDelta, _iAnimIndex);
}

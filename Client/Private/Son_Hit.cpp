#include "Son_Hit.h"
#include "Son_Idle.h"
#include "GameInstance.h"

void CSon_Hit::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);


	_int iRandomSound = m_pGameInstance->Random_Int(1, 5);
	wstring strSoundKey = L"";

	switch (iRandomSound)
	{
	case 1 :
		strSoundKey = L"grand_parasiter_vo_worms_hit001.wav";
		break;
	case 2:
		strSoundKey = L"grand_parasiter_vo_worms_hit002.wav";
		break;
	case 3:
		strSoundKey = L"grand_parasiter_vo_worms_hit003.wav";
		break;
	case 4:
		strSoundKey = L"grand_parasiter_vo_worms_hit004.wav";
		break;
	case 5:
		strSoundKey = L"grand_parasiter_vo_worms_hit005.wav";
		break;
	}

	//! 유정: 공유받은 리소스에 해당 파일 없음. 임시주석
	//m_pGameInstance->Play_Sound(L"SON_HIT", strSoundKey, SOUND_ENEMY_HIT3, 7.f);
}

CState<CSon>* CSon_Hit::Update(CSon* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_Hit::Release(CSon* pActor)
{
	__super::Release(pActor);
}

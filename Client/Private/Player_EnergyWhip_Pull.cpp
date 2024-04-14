#include "Player_EnergyWhip_Pull.h"
#include "Weapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "Bone.h"
#include "SMath.h"

void CPlayer_EnergyWhip_Pull::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, 15);

	pActor->Search_Target(20.f);

}

CState<CPlayer>* CPlayer_EnergyWhip_Pull::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	CCharacter* pTarget = pActor->Get_Target();

	if (nullptr == pTarget)
	{
		return Normal(pActor, fTimeDelta, g_iAnimIndex);
	}
	else if (false == m_bFlags[0])
	{
		pTarget->Hitted_Electrocute();

		Sound_Pull();
		pActor->Play_Voice_Zapper_Dash();



		m_bFlags[0] = true;
	}
	else if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(19);
		if (true == m_bFlags[1])
		{
			pActor->Apply_Shake_And_Blur(Power::Medium);
		}
	}
	else if (false == m_bFlags[2])
	{
		_float fDistance = 1.2f;
		m_bFlags[2] = fDistance >= pActor->Calc_Distance();

		_float3 vFront = pActor->Calc_Front_Pos(_float3(0.f, 0.f, fDistance / 2.f));
		pTarget->Dragged(fTimeDelta, vFront);

	}
	else if(false == m_bFlags[3])
	{
		// 당겨졌을 때 이펙트 생성
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_TeleportPunch.json", pActor, pActor->Calc_Front_Pos(_float3(0.f, 0.f, 1.2f)), true, pActor->Get_Position());	//pActor->Calc_Front_Pos() // 플레이어 앞 위치 계산

		m_bFlags[3] = true;
	}

	if (pActor->Is_Animation_End())
	{
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CPlayer_EnergyWhip_Pull::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

void CPlayer_EnergyWhip_Pull::Sound_Pull()
{
	wstring strFileName = L"";
	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"Player_EnergyMode_WhipAttack_01.wav";
		break;
	case 1:
		strFileName = L"Player_EnergyMode_WhipAttack_02.wav";
		break;
	case 2:
		strFileName = L"Player_EnergyMode_WhipAttack_03.wav";
		break;
	case 3:
		strFileName = L"Player_EnergyMode_WhipAttack_04.wav";
		break;
	case 4:
		strFileName = L"Player_EnergyMode_WhipAttack_05.wav";
		break;
	case 5:
		strFileName = L"Player_EnergyMode_WhipAttack_06.wav";
		break;
	default:
		strFileName = L"Player_EnergyMode_WhipAttack_01.wav";
		break;
	}
	m_pGameInstance->Play_Sound(L"PLAYER_ATTACK", strFileName, CHANNELID::SOUND_PLAYER_ATTACK, 10.f);
}

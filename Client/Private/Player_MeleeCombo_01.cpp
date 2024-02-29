#include "..\Public\Player_MeleeCombo_01.h"

#include "Player_Empowered_Idle.h"
#include "Effect.h"
#include "Clone_Manager.h"

void CPlayer_MeleeCombo_01::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);
	string Test = "../Bin/DataFiles/Data_Animation/";
	pActor->Set_EventNotify(Test, "Test2_AnimationData");
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CPlayer>* CPlayer_MeleeCombo_01::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	
		if (pActor->Get_TrackPosition() <= pActor->Get_CharcterDesc().EffectOnTrackPosition)
			pActor->bTest = true;

		if (pActor->bTest == true)
		{
			if (pActor->Get_TrackPosition() >= pActor->Get_CharcterDesc().EffectOnTrackPosition)
			{
				CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, pActor->Get_CharcterDesc().EffectFileName + ".json");
				pEffect->Set_Position(pActor->Get_CharcterDesc().EffectPosition);
				pActor->bTest = false;
			}
		}

	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	//콜라이더 제어 예시
// 	if (m_fCurrentTrackPosition >= m_iColliderOnTrackPosition && m_fCurrentTrackPosition < m_iColliderOffTrackPosition)
// 	{
// 		m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(true);
// 	}
// 	else
// 	{
// 		m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(false);
// 	}
}

void CPlayer_MeleeCombo_01::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

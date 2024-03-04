#include "VampireCommander_BloodRange_Start.h"
#include "VampireCommander_BloodRange_Loop.h"

void CVampireCommander_BloodRange_Start::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);
	//string Test = "Data_Animation/";
	//pActor->Set_EventNotify(Test, "Test2_AnimationData.json");
	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CVampireCommander>* CVampireCommander_BloodRange_Start::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		return new CVampireCommander_BloodRange_Loop();
	}

	return nullptr;
	//__super::Update(pActor, fTimeDelta);
//  예시
// 	if (pActor->Get_TrackPosition() <= pActor->Get_CharcterDesc().EffectOnTrackPosition)
// 		pActor->bTest = true;
// 
// 	if (pActor->bTest == true)
// 	{
// 		if (pActor->Get_TrackPosition() >= pActor->Get_CharcterDesc().EffectOnTrackPosition)
// 		{
// 			//왼손 오른손 무기화 시켜서 그 무기의 좌표를 여기에 있는 셋 포지션안에 집어 넣으면 그위치에서 터질거임 지금 로컬 값으로 되어 있어서 0점 근처에서만터짐
// 			CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(m_pGameInstance->Get_NextLevel(), LAYER_EFFECT, pActor->Get_CharcterDesc().EffectFileName + ".json");
// 			_float3 EffectBonePosition = {};
// 			EffectBonePosition.x = pActor->Get_Body()->Get_BonePtr("RightHandIK")->Get_CombinedTransformationFloat4x4()._41;
// 			EffectBonePosition.y = pActor->Get_Body()->Get_BonePtr("RightHandIK")->Get_CombinedTransformationFloat4x4()._42;
// 			EffectBonePosition.z = pActor->Get_Body()->Get_BonePtr("RightHandIK")->Get_CombinedTransformationFloat4x4()._43;
// 			pEffect->Set_Position(EffectBonePosition);
// 			pActor->bTest = false;
// 		}
// 	}

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_BloodRange_Start::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

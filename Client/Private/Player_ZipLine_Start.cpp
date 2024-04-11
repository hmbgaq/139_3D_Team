#include "..\Public\Player_ZipLine_Start.h"
#include "Player_ZipLine_Start_to_Loop.h"
#include "Environment_Interact.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Bone.h"

void CPlayer_ZipLine_Start::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);

	pActor->Set_Interection(true);

	

	CEnvironment_Interact* pInteractObject = pActor->Get_InteractObject();

	m_vArrivalPosition = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedPosition(pInteractObject->Get_Transform()->Get_WorldMatrix());
	_float3 vZipLinePosition = pInteractObject->Get_Position();

	_float3 vActorPosition = { vZipLinePosition.x + 0.5f, pActor->Get_Position().y, vZipLinePosition.z + 1.5f};
	pActor->Set_Position(vActorPosition);
	pActor->Get_Transform()->Look_At(m_vArrivalPosition);


	

}

CState<CPlayer>* CPlayer_ZipLine_Start::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	pActor->Get_Transform()->Look_At(m_vArrivalPosition);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		return new CPlayer_ZipLine_Start_to_Loop();
	}


	return nullptr;

}

void CPlayer_ZipLine_Start::Release(CPlayer* pActor)
{
	//!_float4 ZipLineRootBonePosition = m_pModelCom->Get_BonePtr("Root")->Get_CombinedPosition(WorldMatrix);
	//!_float4 ZipLineFinalBonePosition = m_pModelCom->Get_BonePtr("Rope_030")->Get_CombinedPosition(WorldMatrix);

	CEnvironment_Interact* pInteractObject = pActor->Get_InteractObject();

	_float4 ZipLineFinalBonePosition = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedPosition(pInteractObject->Get_Transform()->Get_WorldMatrix());

	m_vZipLineDir = ZipLineFinalBonePosition - pActor->Get_HandPos();

	pActor->Get_Transform()->Look_At_Direction(XMLoadFloat3(&m_vZipLineDir));

	pActor->Set_InteractDir(m_vZipLineDir);
	////todo_vector vDir = ZipLineFinalBonePosition - m_pPlayer->Get_HandPos();

	
	__super::Release(pActor);
	//pActor->Set_RootMove_End(false);
}

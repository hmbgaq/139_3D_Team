#include "..\Public\Player_ZipLine_Start_to_Loop.h"
#include "Player_ZipLine_Loop.h"
#include "Environment_Interact.h"
#include "Transform.h"
#include "Navigation.h"
#include "GameInstance.h"
#include "Bone.h"

void CPlayer_ZipLine_Start_to_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true);
	
	m_fZipLineSpeed = pActor->Get_Transform()->Get_Speed() + 2.f;
		
	CEnvironment_Interact* pInteractObject = pActor->Get_InteractObject();

	_float4x4 BoneMatrix = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedTransformationFloat4x4();
	
	_float4 vBonePos = { BoneMatrix._41, BoneMatrix._42, BoneMatrix._43, 1.f};
	_vector vWorldPos = XMVector3TransformCoord(vBonePos, pInteractObject->Get_Transform()->Get_WorldMatrix());
	vWorldPos.m128_f32[3] = 1.f;
	

	m_vArrivalPosition = vWorldPos;

	//m_vArrivalPosition = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedPosition(pInteractObject->Get_Transform()->Get_WorldMatrix());
	//m_vArrivalPosition.w = 1.f;
	pActor->Get_Transform()->Look_At(XMLoadFloat4(&m_vArrivalPosition));
}

CState<CPlayer>* CPlayer_ZipLine_Start_to_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);
	pActor->Get_Transform()->Look_At(m_vArrivalPosition);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
		if (true == m_bFlags[0])
		{
			return new CPlayer_ZipLine_Loop();
		}
		
	}
	//else
	//{
	//	return new CPlayer_ZipLine_Start_to_Loop();
	//}
	//pActor->Get_Transform()->Go_Target(m_vArrivalPosition, fTimeDelta);
	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_ZipLine_Start_to_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	m_vArrivalPosition = {};
}

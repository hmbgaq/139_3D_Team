#include "..\Public\Player_ZipLine_Loop.h"
#include "Player_ZipLine_Stop.h"
#include "Environment_Interact.h"
#include "Transform.h"
#include "Navigation.h"
#include "GameInstance.h"
#include "Bone.h"

void CPlayer_ZipLine_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, true);
	pActor->Set_Rotate_In_CameraDir(false);
	

	CEnvironment_Interact* pInteractObject = pActor->Get_InteractObject();

	_float4x4 BoneMatrix = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedTransformationFloat4x4();

	_float4 vBonePos = { BoneMatrix._41, BoneMatrix._42, BoneMatrix._43, 1.f };
	_vector vWorldPos = XMVector3TransformCoord(vBonePos, pInteractObject->Get_Transform()->Get_WorldMatrix());
	vWorldPos.m128_f32[3] = 1.f;


	m_vArrivalPosition = vWorldPos;

	//m_vArrivalPosition = pInteractObject->Get_ModelCom()->Get_BonePtr("Rope_030")->Get_CombinedPosition(pInteractObject->Get_Transform()->Get_WorldMatrix());
	//m_vArrivalPosition.w = 1.f;
	pActor->Get_Transform()->Look_At(XMLoadFloat4(&m_vArrivalPosition));	

	m_fZipLineSpeed = pActor->Get_Transform()->Get_Speed() + 2.f;
	m_fTimeAcc = 0.f;

	//TODOAdd_RootBone_ForTarget
}

CState<CPlayer>* CPlayer_ZipLine_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);
	pActor->Get_Transform()->Look_At(XMLoadFloat4(&m_vArrivalPosition));

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(27);
	}
	else
	{
		pActor->Remove_Ladder_Count();

		if (false == pActor->Is_Exist_Ladder_Count())
		{
			return new CPlayer_ZipLine_Stop();
		}
		else
		{

			return new CPlayer_ZipLine_Loop();
		}
	}

		//_vector vDir = XMLoadFloat3(&m_vZipLineDir);
	
		//pActor->Get_Transform()->Move_Position(vDir, pActor->Get_Transform()->Get_Speed(), fTimeDelta);
	//CTransform* pActorTransform = pActor->Get_Transform();

	//XMVector3TransformNormal

	//_float4x4 pActorTransform->Get_WorldFloat4x4()
	//
	//pActorTransform->Add_RootBone_ForTarget(pActor->Get_AddRootMotion(), pActor->Get_Navigation(), pActor->Get_InteractObject()->Get_Transform());

	
	//_float4 vTargetPosition = m_vArrivalPosition;
	//vTargetPosition.y -= 5.5f;
	//pActor->Get_Transform()->Go_Target(vTargetPosition, fTimeDelta);	

	//! 로프의 첫뼈의 위치(Root)와 끝뼈의 위치(Rope_030) 방향벡터를 구해준다
	//! 이 방향벩벡터를 플레이어에게 던져주자.
	return nullptr;

}

void CPlayer_ZipLine_Loop::Release(CPlayer* pActor)
{
	
	__super::Release(pActor);
	m_vArrivalPosition = {};
	//pActor->Set_RootMove_End(false);
}

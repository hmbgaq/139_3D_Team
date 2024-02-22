#include "stdafx.h"
#include "Preset_PhysXControllerDesc.h"
#include "PhysXController.h"
#include "GameInstance.h"

PxCapsuleControllerDesc Preset::PhysXControllerDesc::PlayerSetting(CTransform* pTransform)
{
	PxCapsuleControllerDesc Out_Desc;

	Out_Desc.height = 2.f;
	Out_Desc.radius = 0.4f;
	Out_Desc.density = 5.f;
	//Out_Desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	_vector vPos = pTransform->Get_Position();
	PxExtendedVec3 vPosFromPx = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] };
	vPosFromPx.y += Out_Desc.height * 0.5f;
	Out_Desc.position = vPosFromPx;
	Out_Desc.stepOffset = 0.5f;

	PxMaterial* pMaterial = nullptr;
	CGameInstance::GetInstance()->Create_Material(1.f, 1.f, 0.f, &pMaterial);
	Out_Desc.material = pMaterial;
	Out_Desc.maxJumpHeight;

	return Out_Desc;
}


PxCapsuleControllerDesc Preset::PhysXControllerDesc::BossBatSetting(CTransform* pTransform)
{
	PxCapsuleControllerDesc Out_Desc;

	Out_Desc.height = 4.f;
	Out_Desc.radius = 5.5f;
	Out_Desc.density = 5.f;
	//Out_Desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	_vector vPos = pTransform->Get_Position();
	PxExtendedVec3 vPosFromPx = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] };
	//vPosFromPx.y += Out_Desc.height * 0.5f;
	Out_Desc.position = vPosFromPx;
	Out_Desc.stepOffset = 0.f;

	PxMaterial* pMaterial = nullptr;
	CGameInstance::GetInstance()->Create_Material(1.f, 1.f, 0.f, &pMaterial);
	Out_Desc.material = pMaterial;
	Out_Desc.maxJumpHeight;

	return Out_Desc;
}

PxCapsuleControllerDesc Preset::PhysXControllerDesc::CameraSetting(CTransform* pTransform)
{
	PxCapsuleControllerDesc Out_Desc;
	Out_Desc.behaviorCallback;
	Out_Desc.height = 0.1f;
	Out_Desc.radius = 0.3f;
	Out_Desc.density = 5.f;
	Out_Desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

	_vector vPos = pTransform->Get_Position();
	PxExtendedVec3 vPosFromPx = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] };
	Out_Desc.position = vPosFromPx;
	Out_Desc.stepOffset = 0.5f;

	PxMaterial* pMaterial = nullptr;
	CGameInstance::GetInstance()->Create_Material(1.f, 1.f, 0.f, &pMaterial);
	Out_Desc.material = pMaterial;
	Out_Desc.maxJumpHeight;

	return Out_Desc;
}

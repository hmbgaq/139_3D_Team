#include "Preset_PhysXColliderDesc.h"
#include "PhysXCollider.h"
#include "GameInstance.h"
#include "Transform.h"
#include "SMath.h" 
#include "Client_Defines.h"



void Preset::PhysXColliderDesc::GroundSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, const _bool bTrigger)
{
    Out_Desc.eShape = PHYSXCOLLIDER_TYPE::MESH;
    Out_Desc.eActorType = PHYSXACTOR_TYPE::STATIC;
    Out_Desc.iFilterType = (_uint)PHYSX_COLLISION_LAYER::GROUND;
    Out_Desc.fDensity = 0.f;
    PxMaterial* pMaterial = nullptr;
    CGameInstance::GetInstance()->Create_Material(0.f, 0.f, 0.f, &pMaterial);
    Out_Desc.pMaterial = pMaterial;
    Out_Desc.vAngles = { 0.f, 0.f, 0.f, 0.f };
    Out_Desc.vPosition = { 0.f, 0.f, 0.f, 1.f };
    Out_Desc.vScale = { 1.f, 1.f, 1.f };
    Out_Desc.bTrigger = bTrigger;
};


void Preset::PhysXColliderDesc::DynamicPropSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, CTransform* pTransform)
{
    Out_Desc.eShape = PHYSXCOLLIDER_TYPE::CONVEXMODEL;
    Out_Desc.eActorType = PHYSXACTOR_TYPE::STATIC;
    Out_Desc.iFilterType = (_uint)PHYSX_COLLISION_LAYER::DYNAMIC_PROP;
    Out_Desc.fDensity = 5.f;
    Out_Desc.pConvecMesh = nullptr;

    _float3 PitchYawRoll = SMath::Extract_PitchYawRollFromRotationMatrix(SMath::Get_RotationMatrix(pTransform->Get_WorldMatrix()));
    Out_Desc.vAngles = XMLoadFloat3(&PitchYawRoll);
    _float3 vScale = pTransform->Get_Scaled();

    _vector vPos = pTransform->Get_Position();
    // vPos.m128_f32[1] += fCenterY * vScale.y;
    Out_Desc.vPosition = vPos;

    Out_Desc.vScale = XMLoadFloat3(&vScale);
    PxMaterial* pMaterial = nullptr;
    CGameInstance::GetInstance()->Create_Material(0.5f, 0.5f, 0.f, &pMaterial);
    Out_Desc.pMaterial = pMaterial;
    Out_Desc.bTrigger = false;
};

void Preset::PhysXColliderDesc::DynamicPieceSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, CTransform* pTransform)
{
    Out_Desc.eShape = PHYSXCOLLIDER_TYPE::CONVEXMODEL;
    Out_Desc.eActorType = PHYSXACTOR_TYPE::DYNAMIC;
    Out_Desc.iFilterType = (_uint)PHYSX_COLLISION_LAYER::DYNAMIC_PEICE;
    Out_Desc.fDensity = 5.f;
    Out_Desc.pConvecMesh = nullptr;

    _float3 PitchYawRoll = SMath::Extract_PitchYawRollFromRotationMatrix(SMath::Get_RotationMatrix(pTransform->Get_WorldMatrix()));
    Out_Desc.vAngles = XMLoadFloat3(&PitchYawRoll);
    _float3 vScale = pTransform->Get_Scaled();

    _vector vPos = pTransform->Get_Position();
    // vPos.m128_f32[1] += fCenterY * vScale.y;
    Out_Desc.vPosition = vPos;

    Out_Desc.vScale = XMLoadFloat3(&vScale);
    PxMaterial* pMaterial = nullptr;
    CGameInstance::GetInstance()->Create_Material(0.5f, 0.5f, 0.5f, &pMaterial);
    Out_Desc.pMaterial = pMaterial;
    Out_Desc.bTrigger = false;
}

void Preset::PhysXColliderDesc::ConvexStaticPropSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, CTransform* pTransform)
{
    Out_Desc.eShape = PHYSXCOLLIDER_TYPE::CONVEXMODEL;
    Out_Desc.eActorType = PHYSXACTOR_TYPE::STATIC;
    Out_Desc.iFilterType = (_uint)PHYSX_COLLISION_LAYER::STATIC_PROP;
    Out_Desc.fDensity = 5.f;
    Out_Desc.pConvecMesh = nullptr;

    _float3 PitchYawRoll = SMath::Extract_PitchYawRollFromRotationMatrix(SMath::Get_RotationMatrix(pTransform->Get_WorldMatrix()));
    Out_Desc.vAngles = XMLoadFloat3(&PitchYawRoll);
    _float3 vScale = pTransform->Get_Scaled();

    _vector vPos = pTransform->Get_Position();
    // vPos.m128_f32[1] += fCenterY * vScale.y;
    Out_Desc.vPosition = vPos;

    Out_Desc.vScale = XMLoadFloat3(&vScale);
    PxMaterial* pMaterial = nullptr;
    CGameInstance::GetInstance()->Create_Material(0.5f, 0.5f, 0.5f, &pMaterial);
    Out_Desc.pMaterial = pMaterial;
    Out_Desc.bTrigger = false;
}
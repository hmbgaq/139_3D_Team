#include "stdafx.h"
#include "Son_Projectile.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Player.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Son.h"
#include "Bone.h"

CSon_Projectile::CSon_Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CSon_Projectile::CSon_Projectile(const CSon_Projectile& rhs)
	: CProjectile(rhs)
{
}

HRESULT CSon_Projectile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon_Projectile::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 20.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pSon = CData_Manager::GetInstance()->Get_Son();


	//_float4x4 BoneMatrix = m_pSon->Get_Body()->Get_BonePtr("Center")->Get_CombinedTransformationMatrix();
	//_float4x4 MotherMatrix = m_pSon->Get_Transform()->Get_WorldMatrix();
	//_float4x4 Temp = BoneMatrix * MotherMatrix;
	//
	//m_pTransformCom->Set_WorldMatrix(Temp);


	m_vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION) + 1.0f * CData_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_UP);
	m_vPlayerPos.m128_f32[3] = 1.f;

	m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 10.f;


	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect("Parasiter/", "Yellow_Blood_Test_02.json", this);
	//m_pEffect = EFFECT_MANAGER->Play_Effect("Yellow_Blood_Test_02.json", this);


	return S_OK;
}

void CSon_Projectile::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	if (m_bFirst == true)
	{
		m_pTransformCom->Look_At(m_vPlayerPos);
		m_pEffect = EFFECT_MANAGER->Play_Effect("Son_Test_06.json", this);
		m_bFirst = false;
	}
}

void CSon_Projectile::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);

	//m_fRadian += fTimeDelta;
	//_float4x4 Temp = {};
	//Temp = XMMatrixRotationRollPitchYaw(m_fRadian, 0.f, 0.0f);
	//
	//m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * Temp);
	////m_pTransformCom->RotationToProjectile(Get_Transform()->Get_State(CTransform::STATE_LOOK), m_fRadian);
}

void CSon_Projectile::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CSon_Projectile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSon_Projectile::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CSon_Projectile::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		pTarget_Character->Set_Hitted(m_fDamage, pTarget_Character->Calc_Look_Dir_XZ(m_pTransformCom->Get_Position()), m_fForce, 1.f, m_eHitDirection, m_eHitPower);


		// 이펙트 생성
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Normal.json", m_pTransformCom->Get_Position());
		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());
		EFFECT_MANAGER->Play_Effect("Hit_Distortion.json", m_pTransformCom->Get_Position());
	}
	//m_pCollider->Set_Enable(false);
	this->Set_Dead(true);

	EFFECT_MANAGER->Return_ToPool(m_pEffect);
	m_pEffect = nullptr;
	//m_pEffect->Set_Dead(true);	// 이펙트 죽이기 (EffectOut : 당장 안쓰는 이펙트라고 해서 일단 빼뒀습니다. [성희])
}

void CSon_Projectile::OnCollisionStay(CCollider* other)
{
}

void CSon_Projectile::OnCollisionExit(CCollider* other)
{
}

HRESULT CSon_Projectile::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Son_Projectile"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.fRadius = { 50.f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

CSon_Projectile* CSon_Projectile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CSon_Projectile* pInstance = new CSon_Projectile(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSon_Projectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon_Projectile::Clone(void* pArg)
{
	CSon_Projectile* pInstance = new CSon_Projectile(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSon_Projectile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSon_Projectile::Pool()
{
	return new CSon_Projectile(*this);
}

void CSon_Projectile::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
	{
		EFFECT_MANAGER->Return_ToPool(m_pEffect);
		m_pEffect = nullptr;
		//m_pEffect->Set_Dead(true);	// 이펙트 죽이기
	}

	//if(nullptr != m_pEffect)


}

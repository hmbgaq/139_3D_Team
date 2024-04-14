#include "stdafx.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Weapon_Heavy_Vampiric_Zombie.h"
#include "Model.h"
#include "AttackObject.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"
#include "Effect.h"
#include "SMath.h"
#include "Character.h"

#include "Effect_Trail.h"

CWeapon_Heavy_Vampiric_Zombie::CWeapon_Heavy_Vampiric_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CWeapon(pDevice, pContext, strPrototypeTag)
{
}

CWeapon_Heavy_Vampiric_Zombie::CWeapon_Heavy_Vampiric_Zombie(const CWeapon_Heavy_Vampiric_Zombie& rhs)
	: CWeapon(rhs)
{
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Load_Json());

	FAILED_CHECK(Option_Setting());

	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Model */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Heavy_Vampiric_Zombie_Weapon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom)));

	/* For. Com_Shader */
	FAILED_CHECK(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	///* For.Com_Collider */
	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::MONSTER_ATTACK);
	BoundingDesc.fRadius = { 0.6f };
	BoundingDesc.vCenter = _float3(0.f, 1.3f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;



	//! 유정: 트레일
	m_pTrail = EFFECT_MANAGER->Ready_Trail(iNextLevel, LAYER_EFFECT, "Heavy_Vampiric_Zombie_Trail_03.json");
	m_pTrail->Set_Play(false);		// 시작은 끄기

	m_pTrail_Post = EFFECT_MANAGER->Ready_Trail(iNextLevel, LAYER_EFFECT, "Heavy_Vampiric_Zombie_Trail_Distortion_02.json");
	m_pTrail_Post->Set_Play(false);		// 시작은 끄기


	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Load_Json()
{
	string path = "../Bin/DataFiles/Data_Weapon/Monster/Bandit_Heavy/Axe.json";
	json In_Json;
	CJson_Utility::Load_Json(path.c_str(), In_Json);
	m_pTransformCom->Load_FromJson(In_Json);

	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Option_Setting()
{
	m_iRenderPass = ECast(MONSTER_SHADER::COMMON_ORIGIN);

	return S_OK;
}

void CWeapon_Heavy_Vampiric_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon_Heavy_Vampiric_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bAttack)
	{
		

		_float3 vPosition = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43};
		//m_pTransformCom->Get_Position();
		//EFFECT_MANAGER->Play_Effect("Monster/", "Vampire_Zombie_GroundAttack.json", nullptr, m_pTransformCom->Get_Position());
		EFFECT_MANAGER->Play_Effect("Monster/", "Vampire_Zombie_GroundAttack.json", nullptr, vPosition);
		m_bAttack = false;
	}
	//if (m_pGameInstance->Key_Down(DIK_F))
	//{
	//	string path = "../Bin/DataFiles/Data_Weapon/Monster/Bandit_Heavy/Axe.json";
	//	{
	//		json Out_Json;
	//		m_pTransformCom->Write_Json(Out_Json);
	//		CJson_Utility::Save_Json(path.c_str(), Out_Json);
	//	}
	//}
}

void CWeapon_Heavy_Vampiric_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		//! 유정: 트레일 
		if (nullptr != m_pTrail)
		{
			m_pTrail->Tick_Trail(fTimeDelta, m_WorldMatrix);
		}

		if (nullptr != m_pTrail_Post)
		{
			m_pTrail_Post->Tick_Trail(fTimeDelta, m_WorldMatrix);
		}

		m_pModelCom->Play_Animation(fTimeDelta, _float3(0.f, 0.f, 0.f));
	}

}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Bind_ShaderResources()
{
	FAILED_CHECK(__super::Bind_ShaderResources());

	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(m_iRenderPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon_Heavy_Vampiric_Zombie::Render_Shadow()
{
	__super::Render_Shadow();
	
	return S_OK;
}

void CWeapon_Heavy_Vampiric_Zombie::Attack(CCollider* other)
{
	if (ECast(COLLISION_LAYER::PLAYER_PARRYING) == other->Get_Layer())
	{
		return;
	}

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (pTarget_Character != nullptr)
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.5f;

		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		_vector vHitDir = Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1;

		pTarget_Character->Set_Hitted(m_fDamage, vHitDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);

		Activate_Collisions(false);

		Play_Sound_Attack();
	}
}

void CWeapon_Heavy_Vampiric_Zombie::OnCollisionEnter(CCollider* other)
{
	Attack(other);
}

void CWeapon_Heavy_Vampiric_Zombie::OnCollisionStay(CCollider* other)
{
	Attack(other);
}

void CWeapon_Heavy_Vampiric_Zombie::OnCollisionExit(CCollider* other)
{

}

void CWeapon_Heavy_Vampiric_Zombie::Play_Sound_Attack()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 5);
	switch (iRand)
	{
	case 0:
		strFileName = L"bandit_heavy_attack_strong_slam_impact001.wav";
		break;
	case 1:
		strFileName = L"bandit_heavy_attack_strong_slam_impact002.wav";
		break;
	case 2:
		strFileName = L"bandit_heavy_attack_strong_slam_impact003.wav";
		break;
	case 3:
		strFileName = L"bandit_heavy_attack_strong_slam_impact004.wav";
		break;
	case 4:
		strFileName = L"bandit_heavy_attack_strong_slam_impact005.wav";
		break;
	default:
		strFileName = L"bandit_heavy_attack_strong_slam_impact001.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"ZENU_ATTACK", strFileName, CHANNELID::SOUND_ENEMY_ATTACK, 10.f);
}

void CWeapon_Heavy_Vampiric_Zombie::Play_Trail(_bool bPlay)
{
	if (nullptr != m_pTrail)
		m_pTrail->Set_Play(bPlay);

	if (nullptr != m_pTrail_Post)
		m_pTrail_Post->Set_Play(bPlay);
}

#pragma region Create, Clone, Pool, Free

CWeapon_Heavy_Vampiric_Zombie* CWeapon_Heavy_Vampiric_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CWeapon_Heavy_Vampiric_Zombie* pInstance = new CWeapon_Heavy_Vampiric_Zombie(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon_Heavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Heavy_Vampiric_Zombie::Clone(void* pArg)
{
	CWeapon_Heavy_Vampiric_Zombie* pInstance = new CWeapon_Heavy_Vampiric_Zombie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon_Heavy_Vampiric_Zombie");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon_Heavy_Vampiric_Zombie::Pool()
{
	return new CWeapon_Heavy_Vampiric_Zombie(*this);
}

void CWeapon_Heavy_Vampiric_Zombie::Free()
{
	__super::Free();


	if (nullptr != m_pTrail)
	{
		m_pTrail->Set_Play(false);
		m_pTrail->Get_Desc()->bRender = false;
		m_pTrail->Set_Object_Owner(nullptr);
		m_pTrail = nullptr;
		//Safe_Release(m_pTrail);
	}

	if (nullptr != m_pTrail_Post)
	{
		m_pTrail_Post->Set_Play(false);
		m_pTrail_Post->Get_Desc()->bRender = false;
		m_pTrail_Post->Set_Object_Owner(nullptr);
		m_pTrail_Post = nullptr;
		//Safe_Release(m_pTrail);
	}

}


#pragma endregion

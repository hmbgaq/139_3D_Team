#include "Weapon.h"
#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"
#include "AttackObject.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CAttackObject(pDevice, pContext, strPrototypeTag)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CAttackObject(rhs)
	, m_pColliders(rhs.m_pColliders)
	, m_iColliderSize(rhs.m_iColliderSize)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	m_pParentTransform = (reinterpret_cast<WEAPON_DESC*>(pArg))->m_pParentTransform;
	if (nullptr == m_pParentTransform)
		return E_FAIL;
	Safe_AddRef(m_pParentTransform);

	m_pSocketBone = (reinterpret_cast<WEAPON_DESC*>(pArg))->m_pSocketBone;
	if (nullptr == m_pSocketBone)
		return E_FAIL;
	Safe_AddRef(m_pSocketBone);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CWeapon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (_uint i = 0; i < m_iColliderSize; ++i)
		m_pColliders[i]->Update(m_WorldMatrix);

	//m_pCollider->Update(m_WorldMatrix);

}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_bIsFollow)
	{
		_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransform->Get_WorldMatrix());
	}



#ifdef _DEBUG
	for (_uint i = 0; i < m_iColliderSize; ++i) 
	{
		if (true == m_pColliders[i]->Get_Enable()) 
		{
			m_pGameInstance->Add_DebugRender(m_pColliders[i]);
		}
	}
		
#endif

	if (nullptr == m_pModelCom)
		return;

	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this), );
	}
}

HRESULT CWeapon::Render()
{
	FAILED_CHECK(__super::Render());

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));

	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_iCurrnetLevel)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_iCurrnetLevel)));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(2);
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon::Render_CSM(_uint i)
{
	return E_NOTIMPL;
}


//CCharacter* CWeapon::Get_Target_Character(CCollider* other)
//{
//	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
//		return nullptr;
//
//	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
//	if (nullptr == pTarget_Character)
//		return nullptr;
//
//	return pTarget_Character;
//}
//
//CCharacter* CWeapon::Get_PartOwner()
//{
//	if (nullptr == m_pOwner || m_pOwner->Is_Dead() || false == m_pOwner->Get_Enable())
//		return nullptr;
//
//	return dynamic_cast<CCharacter*>(m_pOwner);
//}
//
//CWeapon* CWeapon::Set_Damage(_float _fDamage)
//{
//	m_fDamage = _fDamage;
//
//	return this;
//}
//
//CWeapon* CWeapon::Set_Direction(Direction _eHitDirection)
//{
//	m_eHitDirection = _eHitDirection;
//
//	return this;
//}
//
//CWeapon* CWeapon::Set_Power(Power _eHitPower)
//{
//	m_eHitPower = _eHitPower;
//
//	return this;
//}

CWeapon* CWeapon::Activate_Collisions(_bool _bActivate)
{
	for (CCollider* pCollider : m_pColliders)
	{
		pCollider->Set_Enable(_bActivate);
	}

	return this;
}

//CWeapon* CWeapon::Set_Force(_float _fForce)
//{
//	m_fForce = _fForce;
//
//	return this;
//}

CWeapon* CWeapon::Set_Dir(_float3 _vDir)
{
	m_vDir = _vDir;

	return this;
}

//CWeapon* CWeapon::Set_KnockUp(_bool _bKnockUp)
//{
//	m_bKnockUp = _bKnockUp;
//
//	return this;
//}

void CWeapon::Set_Enable_Collisions(_bool bEnable)
{
	for (CCollider* pCollider : m_pColliders)
	{
		pCollider->Set_Enable(bEnable);
	}
}

_float3 CWeapon::Get_WorldPosition()
{
	_float3 result = { 0.f, 0.f, 0.f };
	result.x = m_WorldMatrix._41;
	result.y = m_WorldMatrix._42;
	result.z = m_WorldMatrix._43;

	return result;
}

_float3 CWeapon::Get_MuzzlePos()
{
	return Calc_Front_Pos(m_vMuzzlePos_Local);
}

_float3 CWeapon::Calc_Front_Pos(_float3 vDiff)
{
	_float3 vRealDiff = { -vDiff.z, -vDiff.x, vDiff.y };

	_vector vFront = XMVector3TransformCoord(vRealDiff, m_WorldMatrix);
	_float3 vResult;
	XMStoreFloat3(&vResult, vFront);

	return vResult;
}

void CWeapon::Fire(const wstring& strBulletTag, const wstring& strLayerTag, _float3 vTargetPos, CCharacter* pTarget)
{
	CGameObject* pBullet = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, strLayerTag, strBulletTag);
	//_float3 vSpawnPos = Get_WorldPosition();
	_float3 vSpawnPos = Get_MuzzlePos();
	_vector vTargetVector = XMLoadFloat3(&Calc_Front_Pos(vTargetPos));

	pBullet->Set_Position(vSpawnPos);
	pBullet->Get_Transform()->Look_At(vTargetVector);

	if (nullptr != pTarget)
	{
		_vector vTargetVector = XMLoadFloat3(&pTarget->Get_WeaknessPos());
		pBullet->Get_Transform()->Look_At(vTargetVector);
	}
}

void CWeapon::Set_Animation(_uint _iNextAnimation, CModel::ANIM_STATE _eAnimState, _uint iTargetKeyFrameIndex)
{
	m_pModelCom->Set_Animation(_iNextAnimation, _eAnimState, false, m_pModelCom->Get_TickPerSecond() / 10.f, iTargetKeyFrameIndex);
}

_bool CWeapon::Is_Animation_End()
{
	return m_pModelCom->Is_AnimEnd();
}

CModel::ANIM_STATE CWeapon::Get_AnimState()
{
	return m_pModelCom->Get_AnimState();
}

HRESULT CWeapon::Bind_ShaderResources()
{
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	return S_OK;
}

void CWeapon::Play_Hit_Sound(Power ePower)
{
	//Play_Hit_Sound_Base();

	switch (ePower)
	{
	case Engine::Heavy:
		Play_Hit_Sound_Heavy();
		break;
	default:
		Play_Hit_Sound_Normal();
		break;
	}
}

void CWeapon::Free()
{
	__super::Free();
	for (_uint i = 0; i < m_iColliderSize; ++i) 
	{
	   Safe_Release(m_pColliders[i]);
	}
	m_pColliders.clear();
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pSocketBone);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	//Safe_Release(m_pCollider);
}

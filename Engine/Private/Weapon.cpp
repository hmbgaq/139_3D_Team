#include "..\Public\Weapon.h"

#include "GameInstance.h"
#include "Character.h"

#include "Bone.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
	: CGameObject(rhs)
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

	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransform->Get_WorldMatrix());


#ifdef _DEBUG
	for (_uint i = 0; i < m_iColliderSize; ++i)
		m_pGameInstance->Add_DebugRender(m_pColliders[i]);
#endif

	if (nullptr == m_pModelCom)
		return;


	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return;
	}
}

HRESULT CWeapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float4			vLightPos = Engine::g_vLightPos;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(Engine::g_vLightPos.x, Engine::g_vLightPos.y, Engine::g_vLightPos.z, Engine::g_vLightPos.w), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, Engine::g_fLightFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin((_uint)1);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}


CCharacter* CWeapon::Get_Target_Character(CCollider* other)
{
	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
		return nullptr;

	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
	if (nullptr == pTarget_Character)
		return nullptr;

	return pTarget_Character;
}

CWeapon* CWeapon::Set_Damage(_int _iDamage)
{
	m_iDamage = _iDamage;

	return this;
}

CWeapon* CWeapon::Set_Direction(Direction _eHitDirection)
{
	m_eHitDirection = _eHitDirection;

	return this;
}

CWeapon* CWeapon::Set_Power(Power _eHitPower)
{
	m_eHitPower = _eHitPower;

	return this;
}

CWeapon* CWeapon::Activate_Collisions(_bool _bActivate)
{
	for (CCollider* pCollider : m_pColliders)
	{
		pCollider->Set_Enable(_bActivate);
	}

	return this;
}

CWeapon* CWeapon::Set_Force(_float _fForce)
{
	m_fForce = _fForce;

	return this;
}

CWeapon* CWeapon::Set_Dir(_float3 _vDir)
{
	m_vDir = _vDir;

	return this;
}

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





_float3 CWeapon::Calc_Front_Pos(_float3 vDiff)
{
	_float3 vRealDiff = { -vDiff.z, -vDiff.x, vDiff.y };

	_vector vFront = XMVector3TransformCoord(vRealDiff, m_WorldMatrix);
	_float3 vResult;
	XMStoreFloat3(&vResult, vFront);

	return vResult;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
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

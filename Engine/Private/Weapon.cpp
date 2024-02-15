#include "..\Public\Weapon.h"

#include "GameInstance.h"

#include "Bone.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
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

	//if (FAILED(Ready_Components(m_pGameInstance->Get_NextLevel())))
	//	return E_FAIL;


	return S_OK;
}

void CWeapon::Priority_Tick(_float fTimeDelta)
{
	if (!m_bUse)
		return;

	__super::Priority_Tick(fTimeDelta);
}

void CWeapon::Tick(_float fTimeDelta)
{
	if (!m_bUse)
		return;

	__super::Tick(fTimeDelta);

	for (_uint i = 0; i < m_iColliderSize; ++i)
		m_pColliders[i]->Update(m_WorldMatrix);

	//Collision_Chcek();
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	if (!m_bUse)
		return;

	__super::Late_Tick(fTimeDelta);

	_matrix		SocketMatrix = m_pSocketBone->Get_CombinedTransformationMatrix();

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * m_pParentTransform->Get_WorldMatrix());

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
	if (!m_bUse)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
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
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1280.0f / 720.0f, 0.1f, Engine::g_fLightFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
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

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pSocketBone);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

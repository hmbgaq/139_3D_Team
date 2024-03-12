#include "..\Public\Body.h"
#include "GameInstance.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CBody::CBody(const CBody& rhs)
	: CGameObject(rhs)
{
}

CBone* CBody::Get_BonePtr(const _char* pBoneName)
{
	return m_pModelCom->Get_BonePtr(pBoneName);
}

HRESULT CBody::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CBody::Initialize(void* pArg)
{
	m_pParentTransform = (reinterpret_cast<BODY_DESC*>(pArg))->m_pParentTransform;
	if (nullptr == m_pParentTransform)
		return E_FAIL;
	Safe_AddRef(m_pParentTransform);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	#ifdef _DEBUG
Set_MouseMove(fTimeDelta);
#endif // _DEBUG
}

void CBody::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_vMovePos = { 0.f, 0.f, 0.f };

	m_pColliderCom->Update(m_WorldMatrix);

	//if (m_bDissolve)
	//	m_fDissolveWeight += fTimeDelta;
}

void CBody::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());


	if (true == m_pGameInstance->isIn_WorldPlanes(m_pParentTransform->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pModelCom->Play_Animation(fTimeDelta, m_vMovePos);

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;

		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this)))
			return;
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	
}

HRESULT CBody::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_uint iPass = m_iShaderPass; // false == m_bDissolve ? 0 : 3;

	//if (FAILED(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float))))
	//	return E_FAIL;


	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(iPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	_float			g_fLightFar = Engine::g_fLightFar;
	_float4			vLightPos = Engine::g_vLightPos;



	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(Engine::g_vLightPos.x, Engine::g_vLightPos.y, Engine::g_vLightPos.z, Engine::g_vLightPos.w), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinsizeX / g_iWinsizeY, 0.1f, Engine::g_fLightFar));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightFar", &g_fLightFar, sizeof(_float))))
	//	return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	_uint iPass = 2; // false == m_bDissolve ? 2 : 3;

	//if (FAILED(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveWeight", &m_fDissolveWeight, sizeof(_float))))
	//	return E_FAIL;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(iPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_int CBody::Get_CurrentAnimIndex()
{
	return m_pModelCom->Get_CurrentAnimIndex();
}

void CBody::Set_Animation(_uint _iNextAnimation, CModel::ANIM_STATE _eAnimState, _bool _bIsTransition, _bool _bUseAnimationPos, _uint iTargetKeyFrameIndex)
{
	m_pModelCom->Set_Animation(_iNextAnimation, _eAnimState, _bIsTransition, m_pModelCom->Get_TickPerSecond() / 10.f, iTargetKeyFrameIndex);
	m_pModelCom->Set_UseAnimationPos(_bUseAnimationPos);
}

_bool CBody::Is_Animation_End()
{
	return m_pModelCom->Is_AnimEnd();
}

_bool CBody::Is_UpperAnimation_End()
{
	return m_pModelCom->Is_UpperAnimEnd();
}

_bool CBody::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_pModelCom->Is_Inputable_Front(_iIndexFront);
}
_float CBody::Get_TrackPosition()
{
	return m_pModelCom->Get_TrackPosition();
}
#ifdef _DEBUG

void CBody::Set_MouseMove(_float fTimeDelta)
{
	_float2 vMouseMove = { 0.f, 0.f };


	_long	MouseMove = 0;

	_float fSpeed = 10.f;

	vMouseMove.x = (_float)m_pGameInstance->Get_DIMouseMove(DIMS_X);
	vMouseMove.y = (_float)m_pGameInstance->Get_DIMouseMove(DIMS_Y);

	vMouseMove *= fSpeed * fTimeDelta;


	//if (CGameInstance::GetInstance()->Key_Pressing(DIK_V))
	//	vMouseMove.x += fSpeed;

	//if (CGameInstance::GetInstance()->Key_Pressing(DIK_B))
	//	vMouseMove.x -= fSpeed;


	//if (CGameInstance::GetInstance()->Key_Pressing(DIK_F))
	//	vMouseMove.y -= fSpeed;

	//if (CGameInstance::GetInstance()->Key_Pressing(DIK_G))
	//	vMouseMove.y += fSpeed;

	m_pModelCom->Set_MouseMove(vMouseMove);

}

_bool CBody::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;
	
	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;
	
	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);

}

CCharacter* CBody::Get_Owner()
{
	return m_pOwner;
}

void CBody::Set_Owner(CCharacter* pOwner)
{
	m_pOwner = pOwner;
}
#endif

void CBody::Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState)
{
	m_pModelCom->Set_Animation_Upper(_iAnimationIndex, _eAnimState, m_pModelCom->Get_TickPerSecond() / 10.f);
	m_pModelCom->Set_Splitted(true);
}

HRESULT CBody::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
	//	return E_FAIL;

// 	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
// 		return E_FAIL;


	return S_OK;
}

void CBody::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

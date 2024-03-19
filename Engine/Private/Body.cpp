#include "..\Public\Body.h"
#include "GameInstance.h"
#include "Character.h"

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

void CBody::Collider_Off()
{
	m_pColliderCom->Set_Enable(false);
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
}

void CBody::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_vMovePos = { 0.f, 0.f, 0.f };

	m_pColliderCom->Update(m_WorldMatrix);

	//if (m_pGameInstance->Key_Down(DIK_9)) 
	//{
	//	m_fRotateUpperY -= 0.1f;
	//}
	//if (m_pGameInstance->Key_Down(DIK_0))
	//{
	//	m_fRotateUpperY += 0.1f;
	//}

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

	FAILED_CHECK(Bind_ShaderResources());

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPass = m_iShaderPass; 

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", (_uint)i, aiTextureType_SPECULAR);

		m_pShaderCom->Begin(iPass);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CBody::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_pGameInstance->Get_NextLevel());

	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(2);

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
_bool CBody::Compare_TrackPosition_Is_Over(_float fTrackPosition)
{
	return m_pModelCom->Compare_TrackPosition_Is_Over(fTrackPosition);
}
void CBody::Set_TrackPosition(_int iNewTrackPosition)
{
	return m_pModelCom->Set_TrackPosition(iNewTrackPosition);
}

void CBody::OnCollisionEnter(CCollider* other)
{
}
	
void CBody::OnCollisionStay(CCollider* other)
{
	CCharacter* pTarget_Character = Get_Target_Character(other);
	if (nullptr != pTarget_Character)
	{
		_vector vTargetPos = pTarget_Character->Get_Position_Vector();

		pTarget_Character->Add_Force(Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1	, 9.f * m_pGameInstance->Get_TimeDelta());
	}
}

void CBody::OnCollisionExit(CCollider* other)
{
}

void CBody::Set_MouseMove(_float fTimeDelta)
{
	_float2 vMouseMove = { 0.f, 0.f };

	//_float fSpeed = 10.f;

	vMouseMove.x = (_float)m_pGameInstance->Get_DIMouseMove(DIMS_X) * 12.f * fTimeDelta;
	vMouseMove.y = (_float)m_pGameInstance->Get_DIMouseMove(DIMS_Y) * 3.5f * fTimeDelta;
	//vMouseMove.y = (_float)m_pGameInstance->Get_DIMouseMove(DIMS_Y) * 10.f * fTimeDelta;

	//vMouseMove *= fSpeed * fTimeDelta;


	_float2 vResult = vMouseMove;

	m_fRotateUpperY += vMouseMove.y;
	//m_fRotateUpperY += vMouseMove.y / (10.f / 3.5f);
	

	vResult.y = m_fRotateUpperY - m_fShootingReaction;

	vResult.x += m_fRotateUpperX;

	m_pModelCom->Set_MouseMove(vResult);

}

CCharacter* CBody::Get_Target_Character(CCollider* other)
{
	if (nullptr == other || nullptr == other->Get_Owner() || nullptr == other->Get_Owner()->Get_Object_Owner())
		return nullptr;

	CCharacter* pTarget_Character = dynamic_cast<CCharacter*>(other->Get_Owner()->Get_Object_Owner());
	if (nullptr == pTarget_Character)
		return nullptr;

	return pTarget_Character;
}

#ifdef _DEBUG

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

//CCharacter* CBody::Get_Owner()
//{
//	return m_pOwner;
//}
//
//void CBody::Set_Owner(CCharacter* pOwner)
//{
//	m_pOwner = pOwner;
//}
#endif

void CBody::Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState, _uint iTargetKeyFrameIndex)
{
	m_pModelCom->Set_Animation_Upper(_iAnimationIndex, _eAnimState, m_pModelCom->Get_TickPerSecond() / 10.f, iTargetKeyFrameIndex);
	m_pModelCom->Set_Splitted(true);
	
}

void CBody::Set_RotateUpperX(MoveDirection eDirection)
{
	switch (eDirection)
	{
	case MoveDirection::FrontLeft:
	case MoveDirection::BackRight:
		m_fRotateUpperX = 45.f / 2.f;
		break;
	case MoveDirection::FrontRight:
	case MoveDirection::BackLeft:
		m_fRotateUpperX = -45.f / 2.f;
		break;
	case MoveDirection::Left:
		m_fRotateUpperX = 45.f;
		break;
	case MoveDirection::Right:
		m_fRotateUpperX = -45.f;
		break;
	default:
		m_fRotateUpperX = 0;
		break;
	}

	//switch (eDirection)
	//{
	//case Engine::Left:
	//	m_fRotateUpperX = 45.f;
	//	break;
	//case Engine::Right:
	//	m_fRotateUpperX = -45.f;
	//	break;
	//default:
	//	m_fRotateUpperX = 0;
	//	break;
	//}
}

void CBody::Activate_ShootingReaction(_float fHeight)
{
	m_fShootingReactionTarget += fHeight;
}

void CBody::Update_ShootingReaction(_float fTimeDelta)
{
	_float fReactionValue = m_fShootingReactionTarget * min(m_fShootingReactionTarget, 1.f / fTimeDelta) * fTimeDelta;
	m_fShootingReactionTarget -= fReactionValue;
	m_fShootingReaction += fReactionValue;
	m_fShootingReaction = max((1 - fTimeDelta * max(m_fShootingReaction / 2, 2)) * m_fShootingReaction, 0);
}

void CBody::Reset_UpperAngle()
{
	m_fRotateUpperX = { 0.f };
	m_fRotateUpperY = { 2.6f };

	m_fShootingReaction = { 0.f };
	m_fShootingReactionTarget = { 0.f };
}

HRESULT CBody::Bind_ShaderResources()
{
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

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

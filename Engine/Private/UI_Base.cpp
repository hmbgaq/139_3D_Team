#include "UI_Base.h"
#include "GameInstance.h"

_int g_iWinSizeX = 1280;
_int g_iWinSizeY = 720;

CUI_Base::CUI_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CUI_Base::CUI_Base(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Base::Initialize(void* pArg)
{
	m_tUIInfo = *(UI_DESC*)pArg;


	//m_fX = pDesc->fX;
	//m_fY = pDesc->fY;
	//m_fSizeX = pDesc->fSizeX;
	//m_fSizeY = pDesc->fSizeY;

	m_isEnable = m_tUIInfo.bEnable;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, 0.f, 0.f);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr != Find_Component(g_pTransformTag))
		return E_FAIL;

	m_Components.emplace(g_pTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);


	if (false == m_tUIInfo.bWorldUI)
	{
		if (m_tUIInfo.bFrame == true)
			m_pTransformCom->Set_Scaling(m_tUIInfo.fSizeX, m_tUIInfo.fSizeY * 2, 0.5f);
		else
			m_pTransformCom->Set_Scaling(m_tUIInfo.fSizeX, m_tUIInfo.fSizeY, 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIInfo.fX - (_float)g_iWinSizeX * 0.5f, -m_tUIInfo.fY + (_float)g_iWinSizeY * 0.5f, 0.f, 1.f));

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

		SetUp_ScreenPosRect(m_tUIInfo.fX, m_tUIInfo.fY, m_tUIInfo.fSizeX, m_tUIInfo.fSizeY);
		SetUp_UV(0);
	}

	return S_OK;
}

void CUI_Base::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Base::Tick(_float fTimeDelta)
{
}

void CUI_Base::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CUI_Base::Render()
{


	return S_OK;
}

void CUI_Base::SetUp_UV(_uint iTextureIndex)
{
	//!Get_TextureSize
	//! 
	//! m_tUIInfo.fTexSizeX

	_uint iTextureWidth = 0, iTextureHeight = 0;

	/* TextureSize 조절 */
	//m_pTextureCom->Get_TextureSize(&iTextureWidth, &iTextureHeight, iTextureIndex);

	m_tUIInfo.fTexSizeX = (_float)iTextureWidth;
	m_tUIInfo.fTexSizeY = (_float)iTextureHeight;


	//m_tTexUVInfo.fOriginLeft = iTextureWidth - iTextureWidth;
	//m_tTexUVInfo.fOriginRight = iTextureWidth / iTextureWidth;
	//m_tTexUVInfo.fOriginTop = iTextureHeight - iTextureHeight;
	//m_tTexUVInfo.fOriginBottom = iTextureHeight / iTextureHeight;


	//m_tTexUVInfo.fLeft = m_tTexUVInfo.fOriginLeft / m_tUIInfo.fTexSizeX;
	//m_tTexUVInfo.fRight = m_tTexUVInfo.fOriginRight / m_tUIInfo.fTexSizeX;
	//m_tTexUVInfo.fTop = m_tTexUVInfo.fOriginTop / m_tUIInfo.fTexSizeY;
	//m_tTexUVInfo.fBottom = m_tTexUVInfo.fOriginBottom / m_tUIInfo.fTexSizeY;



}

void CUI_Base::SetUp_WorldToScreen(_fvector vWorldPos)
{
	_vector vTargetPos = {};
	_float4 vViewPort = {};

	vTargetPos = vWorldPos;
	vTargetPos.m128_f32[1] += 2.f;
	vTargetPos = XMVector3Transform(vTargetPos, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	XMStoreFloat4(&vViewPort, vTargetPos);

	m_fWorldToScreenX = (vViewPort.x + 1.f) * (g_iWinSizeX >> 1);
	m_fWorldToScreenY = abs((vViewPort.y - 1.f) * (g_iWinSizeY >> 1));

	if (g_iWinSizeX < m_fWorldToScreenX || m_fWorldToScreenX < 0 || g_iWinSizeY < m_fWorldToScreenY || m_fWorldToScreenY < 0)
	{
		m_fWorldToScreenX = -300.f;
		m_fWorldToScreenY = -300.f;
	}

	return;
}

//void CUI_Base::SetUp_ScreenToWorld(_float fScreenX, _float fScreenY)
//{
//	// 화면 좌표를 정규화된 좌표로 변환
//	_float fNDCX = (2.f * fScreenX / g_iWinSizeX) - 1.f;
//	_float fNDCY = 1.f - (2.f * fScreenY / g_iWinSizeY);
//
//	// 정규화된 좌표를 뷰-프로젝션 역행렬로 변환
//	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
//	_matrix VPMatrix = ViewMatrix * m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
//
//	XMMATRIX matInvVP = XMMatrixInverse(nullptr, VPMatrix);
//	_vector vWorldPos = XMVectorSet(fNDCX, fNDCY, 0.f, 1.f);
//	vWorldPos = XMVector3TransformCoord(vWorldPos, matInvVP);
//
//	// 화면 좌표를 월드 좌표로 저장
//
//	//XMStoreFloat3(&m_vWorldPos, vWorldPos);
//}

HRESULT CUI_Base::SetUp_Transform(_float fPosX, _float fPosY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_tUIInfo.fX = fPosX;
	m_tUIInfo.fY = fPosY;
	m_tUIInfo.fSizeX = fSizeX;
	m_tUIInfo.fSizeY = fSizeY;
	//_float fAspectRatio = m_tUIInfo.fSizeY / m_tUIInfo.fSizeX;
	//
	//// Calculate the adjusted size to maintain the original image proportions
	//_float fAdjustedSizeX = m_tUIInfo.fSizeX;
	//_float fAdjustedSizeY = m_tUIInfo.fSizeX * fAspectRatio;
	// 크기 조정
	//m_pTransformCom->Set_Scaling(fAdjustedSizeX, fAdjustedSizeY, 1.f);
	_float fCalcSizeX = fSizeX * m_tUIInfo.fTexSizeX;
	_float fCalcSizeY = fSizeY * m_tUIInfo.fTexSizeY;


	m_pTransformCom->Set_Scaling(fCalcSizeX, fCalcSizeY, 1.f);

	// 위치 이동
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tUIInfo.fX - (_float)g_iWinSizeX * 0.5f, -m_tUIInfo.fY + (_float)g_iWinSizeY * 0.5f, 0.f, 1.f));

	// View Matrix 및 Projection Matrix 설정
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_tUIInfo.fX = fPosX;
	m_tUIInfo.fY = fPosY;
	m_tUIInfo.fSizeX = fSizeX;
	m_tUIInfo.fSizeY = fSizeY;

	// Calculate the correct aspect ratio to maintain image proportions


	return S_OK;
}

HRESULT CUI_Base::SetUp_BillBoarding()
{
	_matrix CamWorldMatrix;

	CamWorldMatrix = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	_float3 vScale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, CamWorldMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, CamWorldMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, CamWorldMatrix.r[2]);
	m_pTransformCom->Set_Scaling(vScale.x, vScale.y, vScale.z);


	return S_OK;
}

HRESULT CUI_Base::SetUp_ScreenPosRect(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY)
{
	m_ScreenPosRect.left = static_cast<LONG>(m_tUIInfo.fX - (m_tUIInfo.fSizeX * 0.5f));
	m_ScreenPosRect.top = static_cast<LONG>(m_tUIInfo.fY - (m_tUIInfo.fSizeY * 0.5f));
	m_ScreenPosRect.right = static_cast<LONG>(m_tUIInfo.fX + (m_tUIInfo.fSizeX * 0.5f));
	m_ScreenPosRect.bottom = static_cast<LONG>(m_tUIInfo.fY + (m_tUIInfo.fSizeY * 0.5f));

	return S_OK;
}






HRESULT CUI_Base::Ready_Components()
{


	return S_OK;
}

HRESULT CUI_Base::Bind_ShaderResources()
{


	return S_OK;
}

void CUI_Base::Free()
{
}



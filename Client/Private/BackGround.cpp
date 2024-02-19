#include "stdafx.h"
#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	m_bIsPoolObject = false;
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{	
	BACKGROUND_DESC*		pDesc = (BACKGROUND_DESC*)pArg;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Ready_Components());

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CBackGround::Priority_Tick(_float fTimeDelta)
{
}

void CBackGround::Tick(_float fTimeDelta)
{
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this), );
}

HRESULT CBackGround::Render()
{
	FAILED_CHECK(Bind_ShaderResources());

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
	/* For.Com_Shader */
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	/* For.Com_VIBuffer */
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)));
	}

	/* For.Com_Texture */
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)));
	}

	return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix));
	FAILED_CHECK(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture"));

	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CBackGround*		pInstance = new CBackGround(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBackGround::Clone(void* pArg)
{
	CBackGround*		pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}


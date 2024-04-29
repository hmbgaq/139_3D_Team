#include "stdafx.h"
#include "Sky.h"
#include "GameInstance.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CSky::CSky(const CSky & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{	
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(Ready_Components());

	switch ((LEVEL)m_iCurrnetLevel)
	{
		case LEVEL::LEVEL_INTRO: // 사실상 여기가 테스트맵 
		{
			m_eSkyType = CSky::SKYTYPE::SKY8;
			break;
		}
		case  LEVEL::LEVEL_GAMEPLAY: // 시작 사막맵 ( 확정 ) 
		{
			m_eSkyType = CSky::SKYTYPE::SKY_STAGE1;
			break;
		}
		case  LEVEL::LEVEL_INTRO_BOSS: // 1스테이지 보스 (미정)
		{
			m_eSkyType = CSky::SKYTYPE::SKY_STAGE1BOSS;
			break;
		}
		case  LEVEL::LEVEL_SNOWMOUNTAIN: // 스노우맵 (확정) 
		{
			m_eSkyType = CSky::SKYTYPE::SKY_STAGE2;
			break;
		}
		case LEVEL::LEVEL_SNOWMOUNTAINBOSS: // 스노우맵 보스 (미정)
		{
			m_eSkyType = CSky::SKYTYPE::SKY_STAGE2BOSS;
			break;
		}
		case  LEVEL::LEVEL_TOOL: // 툴레벨 (미정)
		{
			m_eSkyType = CSky::SKYTYPE::SKY4;
			break;
		}
	}
	
	return S_OK;
}

void CSky::Priority_Tick(_float fTimeDelta)
{
}

void CSky::Tick(_float fTimeDelta)
{
}

void CSky::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
	
	FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this), );
}

HRESULT CSky::Render()
{
#ifdef _DEBUG
	if (m_bRender_Tool)
	{
#endif // _DEBUG

		FAILED_CHECK(Bind_ShaderResources());
		m_pShaderCom->Begin(0);
		m_pVIBufferCom->Bind_VIBuffers();
		m_pVIBufferCom->Render();

#ifdef _DEBUG
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CSky::Ready_Components()
{
	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));

	/* For.Com_Texture */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom)));
	m_iMaxTextureCnt = m_pTextureCom->Get_TextureCount();

	/* For.Com_VIBuffer */
	FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom)));

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_eSkyType));
	
	return S_OK;
}

CSky * CSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CSky*		pInstance = new CSky(pDevice, pContext, strPrototypeTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSky::Clone(void* pArg)
{
	CSky*		pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSky::Pool()
{
	return new CSky(*this);
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);	
	Safe_Release(m_pShaderCom);
}




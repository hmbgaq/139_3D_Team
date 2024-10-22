#include "stdafx.h"
#include "UI_Weakness.h"
#include "GameInstance.h"
#include "Json_Utility.h"


CUI_Weakness::CUI_Weakness(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Weakness::CUI_Weakness(const CUI_Weakness& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Weakness::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Weakness::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	//m_tUIInfo.bWorld = true;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_bActive = false;
	//m_tUIInfo.bWorld = true;
	m_vAxis = { 0.f, 0.f, 1.f, 0.f };

	if(m_iCurrnetLevel == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		m_fActive_Distance = 10.f;
	}
	else
	{
		m_fActive_Distance = 60.f;
	}

	

	m_bActive = true;

	m_tUIInfo.iRenderGroup = 18; // World

	return S_OK;
}

void CUI_Weakness::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Weakness::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_V))
		m_fOffsetY -= 0.1f;
	if (m_pGameInstance->Key_Down(DIK_B))
		m_fOffsetY += 0.1f;


	if (m_bActive == true)
	{
		__super::Tick(fTimeDelta);

		m_pColliderCom->Update(matTargetWorld);
	}

}

void CUI_Weakness::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();
	if (m_bActive == true)
	{
		m_pTransformCom->Turn(m_vAxis, -fTimeDelta * 1.5f);

		//if (m_fTime + m_fActiveTime < GetTickCount64())
		{
			if (m_fScaleX > 40.f)
			{
				Change_SizeX((-m_fChangeScale));
			}

			if (m_fScaleY > 40.f)
			{
				Change_SizeY((-m_fChangeScale));
			}

			if (m_fScaleX <= 40.f && m_fScaleY <= 40.f)
			{
				m_fScaleX = 40.f;
				m_fScaleY = 40.f;
				m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, 1.f);
			}
		}

#ifdef _DEBUG
		m_pGameInstance->Add_DebugRender(m_pColliderCom);
#endif	

		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}

}

HRESULT CUI_Weakness::Render()
{
	if (m_bActive == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		//! 이 셰이더에 0번째 패스로 그릴거야.
		m_pShaderCom->Begin(0); //! Shader_PosTex 7번 패스 = VS_MAIN,  PS_UI_HP

		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CUI_Weakness::UI_Ready(_float fTimeDelta)
{
}

void CUI_Weakness::UI_Enter(_float fTimeDelta)
{
}

void CUI_Weakness::UI_Loop(_float fTimeDelta)
{
}

void CUI_Weakness::UI_Exit(_float fTimeDelta)
{
}

void CUI_Weakness::Set_TargetPosition(_vector vTargetPosition)
{
	m_vTargetPosition = vTargetPosition;
}

void CUI_Weakness::Check_TargetWorld()
{
	if (m_tUIInfo.bWorld == true)
	{
		if (m_bActive == false)
			return;

		// 체력바를 띄운다.
		_float4 vCamPos = m_pGameInstance->Get_CamPosition();
		_vector vTempForDistance = m_pTransformCom->Get_Position() = XMLoadFloat4(&vCamPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vTempForDistance));

		// Distance가 0.1f보다 클경우만 띄움.
		if (fDistance > 0.1f)
		{
			_float3 vTemp = m_pTransformCom->Get_Scaled();
			_vector vScale = XMVectorSet(vTemp.x, vTemp.y, vTemp.z, 0.f);

			_vector vTargetPos = m_vTargetPosition;
			_float4 vTargetTemp;
			XMStoreFloat4(&vTargetTemp, vTargetPos);
			vCamPos.y = vTargetTemp.y;

			_vector vLook = XMVector3Normalize(m_pTransformCom->Get_Position() - XMLoadFloat4(&vCamPos));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			//_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)); // Y빌보드를 막기 위해 Up을 0, 1, 0으로 설정함

			m_World.r[CTransform::STATE_RIGHT] = XMVectorScale(vRight, vTemp.x);
			m_World.r[CTransform::STATE_UP] = XMVectorScale(vUp, vTemp.y);
			m_World.r[CTransform::STATE_LOOK] = XMVectorScale(vLook, vTemp.z);

			//vTargetTemp.y += m_fOffset;
			m_World.r[CTransform::STATE_POSITION] = XMLoadFloat4(&vTargetTemp);

			m_pTransformCom->Set_WorldMatrix(m_World);
			//m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
		}
	}
}

void CUI_Weakness::Set_ColliderRadius(_float fRadius)
{
	m_pColliderCom->Set_Radius(fRadius);
}

HRESULT CUI_Weakness::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("sparks"),
		TEXT("Com_Texture_Lock"), reinterpret_cast<CComponent**>(&m_pTextureCom[SPARKS]))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::WEAKNESS);
	BoundingDesc.fRadius = 70.f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Weakness::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	for (_int i = (_int)0; i < (_int)TEXTURE_END; ++i)
	{
		switch (i)
		{
		case CUI_Weakness::SPARKS:
		{
			if (FAILED(m_pTextureCom[i]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
			break;
		}
		default:
			break;
		}
	}

	return S_OK;
}

json CUI_Weakness::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_Weakness::Load_Desc()
{

}

CUI_Weakness* CUI_Weakness::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_Weakness* pInstance = new CUI_Weakness(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Weakness");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Weakness::Pool()
{
	return new CUI_Weakness(*this);
}

CGameObject* CUI_Weakness::Clone(void* pArg)
{
	CUI_Weakness* pInstance = new CUI_Weakness(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Weakness");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Weakness::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

}

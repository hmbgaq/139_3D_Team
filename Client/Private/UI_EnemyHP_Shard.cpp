#include "stdafx.h"
#include "UI_EnemyHP_Shard.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "GameObject.h"
#include "Character.h"

CUI_EnemyHP_Shard::CUI_EnemyHP_Shard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_EnemyHP_Shard::CUI_EnemyHP_Shard(const CUI_EnemyHP_Shard& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_EnemyHP_Shard::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_EnemyHP_Shard::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	//m_tUIInfo.bWorld = true;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	m_bActive = false;
	m_tUIInfo.bWorld = true;

	//m_fMaxHP = 100.f;
	//m_fCurHP = m_fMaxHP;
	//m_fCurHP = 0.f;

	m_fVariationSpeed = 4.5;

	//m_tUIInfo.iRenderGroup = 10;

	return S_OK;
}

void CUI_EnemyHP_Shard::Priority_Tick(_float fTimeDelta)
{

}

void CUI_EnemyHP_Shard::Tick(_float fTimeDelta)
{

	if (m_pCharacterOwner == nullptr)
		return;

	__super::Tick(fTimeDelta);

	//if (m_pOwner != nullptr)
	//	Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());

		/* Owner HP */
	if (m_pCharacterOwner != nullptr)
		m_fCurHP = m_pCharacterOwner->Get_CurHP();

		// 회복
	if (m_fPreHP < m_fCurHP)
		m_fPreHP = m_fCurHP;

	//if (m_pGameInstance->Key_Down(DIK_J))
	//	m_fCurHP -= 10.f;
	//if (m_pGameInstance->Key_Down(DIK_K))
	//{
	//	m_fCurHP += 10.f;
	//	//m_fPreHP = m_fCurHP;
	//}

	//m_pData_Manager->Limit_HP();

	if (m_bActive == true)
	{
		//m_fCurHP -= fTimeDelta; // 감소시킬수록 게이지가 증가됨 (텍스처가 씌워짐)
		//m_iShaderNum = 5; // 원형 게이지 pass

		//if (m_fCurHP <= 0.f) // 전부 찼을 때 (0)
		//{
		//	// 활성화
		//	m_fCurHP = 0.f;
		//}
		//else
		//{
		//	// 비활성화
		//}


		m_fTimeAcc += fTimeDelta * 0.1f;

		if (m_fCurHP < m_fPreHP)
			m_bLerp = false;

		if (!m_bLerp && m_fPreHP > m_fCurHP)
		{
			m_fPreHP -= fTimeDelta * m_fVariationSpeed/* * (m_fMaxHP / 6.f)*/;

			if (m_fPreHP <= m_fCurHP)
			{
				m_fPreHP = m_fCurHP;
				m_bLerp = true;
			}
		}

		//m_fTimeAcc += fTimeDelta * 0.1f;

		//if (m_fCurHP > m_fPreHP)
		//	m_bLerp = false;

		//if (!m_bLerp && m_fPreHP < m_fCurHP)
		//{
		//	m_fPreHP += fTimeDelta * m_fVariationSpeed/* * (m_fMaxHP / 6.f)*/;

		//	if (m_fPreHP >= m_fCurHP)
		//	{
		//		m_fPreHP = m_fCurHP;
		//		m_bLerp = true;
		//	}
		//}

		//m_fTimeAcc += fTimeDelta * 0.1f;

		//if (m_fCurHP < m_fPreHP)
		//	m_bLerp = false;

		//if (!m_bLerp && m_fPreHP > m_fCurHP)
		//{
		//	m_fPreHP -= fTimeDelta * m_fVariationSpeed * (m_fMaxHP / 4.f);

		//	if (m_fPreHP <= m_fCurHP)
		//	{
		//		m_fPreHP = m_fCurHP;
		//		m_bLerp = true;
		//	}
		//}
	}
}

void CUI_EnemyHP_Shard::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();
	if (m_bActive == true)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
			return;
	}
}

HRESULT CUI_EnemyHP_Shard::Render()
{
	if (m_bActive == true)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		//! 이 셰이더에 0번째 패스로 그릴거야.
		m_pShaderCom->Begin(9); //!

		//! 내가 그리려고 하는 정점, 인덱스 버퍼를 장치에 바인딩해
		m_pVIBufferCom->Bind_VIBuffers();

		//! 바인딩된 정점, 인덱스를 그려
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CUI_EnemyHP_Shard::UI_Ready(_float fTimeDelta)
{
}

void CUI_EnemyHP_Shard::UI_Enter(_float fTimeDelta)
{
}

void CUI_EnemyHP_Shard::UI_Loop(_float fTimeDelta)
{
}

void CUI_EnemyHP_Shard::UI_Exit(_float fTimeDelta)
{
}

void CUI_EnemyHP_Shard::Set_TargetPosition(_vector vTargetPosition)
{
	m_vTargetPosition = vTargetPosition;
}

void CUI_EnemyHP_Shard::Check_TargetWorld()
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

HRESULT CUI_EnemyHP_Shard::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	wstring strPrototag;
	m_pGameInstance->String_To_WString(m_tUIInfo.strProtoTag, strPrototag);

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("EnemyHUD_Shard_Hp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("EnemyHUD_Shard_Hp_Pre"),
		TEXT("Com_Texture_Pre"), reinterpret_cast<CComponent**>(&m_pTexturePreCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EnemyHP_Shard::Bind_ShaderResources()
{
	/* 공통 */
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_LerpHP", &m_fPreHP, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurrentHP", &m_fCurHP, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxHP", &m_fMaxHP, sizeof(_float))))
		return E_FAIL;


	// 현재 체력을 계산해서 던져준다.
	_float fRatio = (m_fMaxHP - m_fPreHP) / m_fMaxHP;
	_float fCurRatio = (m_fMaxHP - m_fCurHP) / m_fMaxHP;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &fRatio, sizeof(_float))))
		return E_FAIL;	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurRatio", &fCurRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartPoint", &m_vStartPoint, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndPoint", &m_vEndPoint, sizeof(_float2))))
		return E_FAIL;


	if (FAILED(m_pTexturePreCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexturePreShard")))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EnemyHP_Shard::Setting_Owner()
{
	if (m_pOwner != nullptr) // Owner 지정해줘야 합니다.
	{
		m_pCharacterOwner = dynamic_cast<CCharacter*>(m_pOwner);

		m_fCurHP = m_pCharacterOwner->Get_CurHP();
		m_fPreHP = m_fCurHP;
		m_fMaxHP = m_pCharacterOwner->Get_MaxHP();
	}
	else
	{
		m_fCurHP = 100.f;
		m_fPreHP = 100.f;
		m_fMaxHP = 100.f;
	}

	return S_OK;
}

json CUI_EnemyHP_Shard::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_EnemyHP_Shard::Load_Desc()
{

}

CUI_EnemyHP_Shard* CUI_EnemyHP_Shard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_EnemyHP_Shard* pInstance = new CUI_EnemyHP_Shard(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_EnemyHP_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_EnemyHP_Shard::Pool()
{
	return new CUI_EnemyHP_Shard(*this);
}

CGameObject* CUI_EnemyHP_Shard::Clone(void* pArg)
{
	CUI_EnemyHP_Shard* pInstance = new CUI_EnemyHP_Shard(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_EnemyHP_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_EnemyHP_Shard::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}



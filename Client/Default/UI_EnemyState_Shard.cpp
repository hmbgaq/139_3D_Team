#include "UI_EnemyState_Shard.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "GameObject.h"
#include "Character.h"

CUI_EnemyState_Shard::CUI_EnemyState_Shard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_EnemyState_Shard::CUI_EnemyState_Shard(const CUI_EnemyState_Shard& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_EnemyState_Shard::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_EnemyState_Shard::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	//m_tUIInfo.bWorld = true;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	//Setting_Owner();

	//m_bActive = false;
	m_bActive = true;
	m_tUIInfo.bWorld = true;
	m_vAxis = { 0.f, 0.f, 1.f, 0.f };
	m_fAlpha = 1.f;
	m_fAlphaSpeed = 2.f;

	return S_OK;
}

void CUI_EnemyState_Shard::Priority_Tick(_float fTimeDelta)
{

}

void CUI_EnemyState_Shard::Tick(_float fTimeDelta)
{
	//if (m_pCharacterOwner == nullptr)
	//	return;



	if (m_pGameInstance->Key_Down(DIK_V))
		m_fOffsetY -= 0.1f;
	if (m_pGameInstance->Key_Down(DIK_B))
		m_fOffsetY += 0.1f;
	if (m_pGameInstance->Key_Down(DIK_J))
		m_bActive = !m_bActive;

	__super::Tick(fTimeDelta);

	//if(m_pOwner != nullptr)
	//	Set_WorldMatrix(m_pOwner->Get_Transform()->Get_WorldMatrix());

	

	if (m_bActive == true)
	{
		if (m_pCharacterOwner != nullptr)
		{
			if (m_pCharacterOwner->Get_MonsterAttackState() == true)
			{
				if (m_bAppear == false) // 안보이는 상태니까
				{
					m_bAppear = Alpha_Minus(fTimeDelta); // 보이게 알파가 생기게 해준다.
				}
				else // 보이는 상태니까
				{
					m_bAppear = Alpha_Plus(fTimeDelta); // 안보이게 알파가 지워지게 해준다.
				}
			}
		}
	}
}

void CUI_EnemyState_Shard::Late_Tick(_float fTimeDelta)
{
	//if (m_tUIInfo.bWorldUI == true)
	//	Compute_OwnerCamDistance();

	if (m_bActive == true)
	{
		if (m_pCharacterOwner != nullptr)
		{
			if (m_pCharacterOwner->Get_MonsterAttackState() == true)
			{
				if (FAILED(m_pGameInstance->Add_RenderGroup((CRenderer::RENDERGROUP)m_tUIInfo.iRenderGroup, this)))
					return;
			}
		}
	}
}

HRESULT CUI_EnemyState_Shard::Render()
{
	if (m_pCharacterOwner != nullptr)
	{
		if (m_pCharacterOwner->Get_MonsterAttackState() == true)
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
		}
	}

	return S_OK;
}

void CUI_EnemyState_Shard::UI_Ready(_float fTimeDelta)
{
}

void CUI_EnemyState_Shard::UI_Enter(_float fTimeDelta)
{
}

void CUI_EnemyState_Shard::UI_Loop(_float fTimeDelta)
{
}

void CUI_EnemyState_Shard::UI_Exit(_float fTimeDelta)
{
}

void CUI_EnemyState_Shard::Set_TargetPosition(_vector vTargetPosition)
{
	m_vTargetPosition = vTargetPosition;
}

void CUI_EnemyState_Shard::Check_TargetWorld()
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

HRESULT CUI_EnemyState_Shard::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("special_attack_icon"),
		TEXT("Com_Texture_Attack"), reinterpret_cast<CComponent**>(&m_pTextureCom[ATTACK]))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_enemy_hp_big_white_center"),
		TEXT("Com_Texture_Electric"), reinterpret_cast<CComponent**>(&m_pTextureCom[ELECTRIC]))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("ui_enemy_hp_big_white_center"),
		TEXT("Com_Texture_Skull"), reinterpret_cast<CComponent**>(&m_pTextureCom[SKULL]))))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("skull"),
		TEXT("Com_Texture_SkullRed"), reinterpret_cast<CComponent**>(&m_pTextureCom[SKULLRED]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_EnemyState_Shard::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (m_pCharacterOwner != nullptr)
	{
		if (m_pCharacterOwner->Get_MonsterAttackState() == true)
		{
			if (FAILED(m_pTextureCom[m_eCurState]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
				return E_FAIL;
		}
	}

	//switch (m_eCurState)
	//{
	//case CUI_EnemyState_Shard::NONE:
	//	break;
	//case CUI_EnemyState_Shard::ATTACK:
	//	if (FAILED(m_pTextureCom[m_eCurState]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	break;
	//case CUI_EnemyState_Shard::ELECTRIC:
	//	if (FAILED(m_pTextureCom[m_eCurState]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	break;
	//case CUI_EnemyState_Shard::SKULL:
	//	if (FAILED(m_pTextureCom[m_eCurState]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	break;
	//case CUI_EnemyState_Shard::SKULLRED:
	//	if (FAILED(m_pTextureCom[m_eCurState]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
	//		return E_FAIL;
	//	break;
	//case CUI_EnemyState_Shard::TEXTURE_END:
	//	break;
	//default:
	//	break;
	//}

	return S_OK;
}

HRESULT CUI_EnemyState_Shard::Setting_Owner()
{
	if (m_pOwner != nullptr) // Owner 지정해줘야 합니다.
	{
		m_pCharacterOwner = dynamic_cast<CCharacter*>(m_pOwner);
	}
	else
	{

	}

	return S_OK;
}

json CUI_EnemyState_Shard::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_EnemyState_Shard::Load_Desc()
{

}

CUI_EnemyState_Shard* CUI_EnemyState_Shard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_EnemyState_Shard* pInstance = new CUI_EnemyState_Shard(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_EnemyState_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_EnemyState_Shard::Pool()
{
	return new CUI_EnemyState_Shard(*this);
}

CGameObject* CUI_EnemyState_Shard::Clone(void* pArg)
{
	CUI_EnemyState_Shard* pInstance = new CUI_EnemyState_Shard(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_EnemyState_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_EnemyState_Shard::Free()
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



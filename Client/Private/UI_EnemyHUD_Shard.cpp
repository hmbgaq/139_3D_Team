#include "UI_EnemyHUD_Shard.h"
#include "GameInstance.h"
#include "Json_Utility.h"
#include "GameObject.h"
#include "UI_Manager.h"
#include "UI_EnemyHP_Shard.h"
#include "UI_EnemyState_Shard.h"

CUI_EnemyHUD_Shard::CUI_EnemyHUD_Shard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_EnemyHUD_Shard::CUI_EnemyHUD_Shard(const CUI_EnemyHUD_Shard& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_EnemyHUD_Shard::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_EnemyHUD_Shard::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	if (FAILED(__super::Initialize(&m_tUIInfo))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_EnemyHUD_Shard::Priority_Tick(_float fTimeDelta)
{

}

void CUI_EnemyHUD_Shard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_bool bIsDead = { false };

	if (m_bDeadOwner == true)
	{
		bIsDead = true;
	}
	else if (nullptr == m_pOwner) 
	{
		bIsDead = true;
	}
	else if (false == m_pOwner->Get_Enable() || true == m_pOwner->Is_Dead())
	{
		bIsDead = true;
	}
	

	if (true == bIsDead)
	{
		DeadEnemyHUD();
		Set_Dead(true);
	}

}

void CUI_EnemyHUD_Shard::Late_Tick(_float fTimeDelta)
{

}

HRESULT CUI_EnemyHUD_Shard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_EnemyHUD_Shard::UI_Ready(_float fTimeDelta)
{
}

void CUI_EnemyHUD_Shard::UI_Enter(_float fTimeDelta)
{
}

void CUI_EnemyHUD_Shard::UI_Loop(_float fTimeDelta)
{
}

void CUI_EnemyHUD_Shard::UI_Exit(_float fTimeDelta)
{
}

void CUI_EnemyHUD_Shard::UI_Setting()
{

}

HRESULT CUI_EnemyHUD_Shard::Ready_ChildHUD()
{
	json json_in;

	//char filePath[MAX_PATH];

	if (m_pOwner == nullptr) // MessageBox No Name
		MessageBox(g_hWnd, L"받아온 pOwner가 nullptr입니다.", L"몬스터 HUD 생성 실패", MB_OK);

	string strFile;

	strFile = "../Bin/DataFiles/Data_UI/EnemyHUD/EnemyHUDShard.json";

	CJson_Utility::Load_Json(strFile.c_str(), json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		/* 저장순서랑 맞는지 확인하기 */
		if (object.contains("Parent"))
			tUI_Info.bParent = object["Parent"];					// 1. Parent
		if (object.contains("World"))
			tUI_Info.bWorld = object["World"];						// 2. World
		if (object.contains("Group"))
			tUI_Info.bGroup = object["Group"];						// 3. Group
		if (object.contains("Alpha"))
			tUI_Info.fAlpha = object["Alpha"];						// 4. Alpha
		if (object.contains("AlphaTrue"))
			tUI_Info.fAlphaTrue = object["AlphaTrue"];				// 0. Alpha
		if (object.contains("ObjectNum"))
			tUI_Info.iObjectNum = object["ObjectNum"];				// 5. ObjectNum
		if (object.contains("ShaderNum"))
			tUI_Info.iShaderNum = object["ShaderNum"];				// 6. ShaderPathNum
		if (object.contains("UINum"))								// "ObjectName" 키가 있으면
			tUI_Info.iUINum = object["UINum"];
		if (object.contains("UIName"))								// "ObjectName" 키가 있으면
			tUI_Info.strUIName = object["UIName"];
		if (object.contains("ObjectName"))							// "ObjectName" 키가 있으면
			tUI_Info.strObjectName = object["ObjectName"];			// 7. ObjectName
		if (object.contains("LayerTag"))
			tUI_Info.strLayerTag = object["LayerTag"];				// 8. LayerTag
		if (object.contains("CloneTag"))
			tUI_Info.strCloneTag = object["CloneTag"];				// 9. CloneTag
		if (object.contains("ProtoTag"))
			tUI_Info.strProtoTag = object["ProtoTag"];				// 10. ProtoTag
		if (object.contains("FilePath"))
			tUI_Info.strFilePath = object["FilePath"];				// 11. FilePath
		if (object.contains("MapTextureTag"))
			tUI_Info.strMapTextureTag = object["MapTextureTag"];	// 12. MapTexture
		if (object.contains("ColorR"))
			tUI_Info.vColor.m128_f32[0] = object["ColorR"];			// 13. R
		if (object.contains("ColorG"))
			tUI_Info.vColor.m128_f32[1] = object["ColorG"];			// 14. G
		if (object.contains("ColorB"))
			tUI_Info.vColor.m128_f32[2] = object["ColorB"];			// 15. B
		if (object.contains("ColorA"))
			tUI_Info.vColor.m128_f32[3] = object["ColorA"];			// 16. A
		if (object.contains("ColorMode"))
			tUI_Info.eColorMode = object["ColorMode"];				// 16. Mode
		if (object.contains("RenderGroup"))
			tUI_Info.iRenderGroup = object["RenderGroup"];			// 16. RenderGroup

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CGameObject* pGameObject = m_pGameInstance->Add_CloneObject_And_Get(m_pGameInstance->Get_NextLevel(), TEXT("Layer_EnemyHUDShard"), wstrClonetag, &tUI_Info);
		if (pGameObject == nullptr)
			return E_FAIL;

		CUI* pUI_Object = dynamic_cast<CUI*>(pGameObject);
		if (pUI_Object == nullptr)
			return E_FAIL;

		pUI_Object->Set_Object_Owner(m_pOwner);
		//pUI_Object->Setting_Owner();

		/* HP */
		string strCloneTag_HPBar = "Prototype_GameObject_UI_EnemyHP_Shard";
		if (pUI_Object->Get_UIDesc().strCloneTag == strCloneTag_HPBar)
		{
			dynamic_cast<CUI_EnemyHP_Shard*>(pUI_Object)->Setting_Owner();
		}

		/* State */
		string strCloneTag_State = "Prototype_GameObject_UI_EnemyState_Shard";
		if (pUI_Object->Get_UIDesc().strCloneTag == strCloneTag_State)
		{
			dynamic_cast<CUI_EnemyState_Shard*>(pUI_Object)->Setting_Owner();
		}

		pUI_Object->Set_RenderGroup(CRenderer::RENDERGROUP::RENDER_UI_WORLD); // World

		m_vecEnemyHUD.push_back(pUI_Object);

		pUI_Object->Get_Transform()->Load_FromJson(object); // 17. TransformCom
		pUI_Object->Load_FromJson(object); // 18. Load Data
	}

	return S_OK;
}

void CUI_EnemyHUD_Shard::Set_EnemyHUD_World(_matrix matWorld, _float3 vOffsetPos)
{
	if (m_vecEnemyHUD.empty())
		return;

	for (auto& iter : m_vecEnemyHUD)
	{
		iter->SetUp_WorldToScreen(matWorld, vOffsetPos);
	}
}

void CUI_EnemyHUD_Shard::ActiveEnemyHUD()
{
	if (m_vecEnemyHUD.empty())
		return;

	for (auto& iter : m_vecEnemyHUD)
	{
		iter->Set_Alpha(!0.f);		// UI 알파값 초기화
		iter->Set_Active(true);		// UI 활성화
		iter->Set_AnimPlay(true);	// UI Animation 재생
		iter->Set_Disappear(false);	// UI 사라짐 Off
	}
}

void CUI_EnemyHUD_Shard::NonActiveEnemyHUD()
{
	if (m_vecEnemyHUD.empty())
		return;

	for (auto& iter : m_vecEnemyHUD)
	{
		iter->Set_Alpha(1.f);			// UI 알파값 초기화
		iter->Set_Active(false);		// UI 활성화
		iter->Set_AnimPlay(false);		// UI Animation 재생
		iter->Set_Disappear(true);		// UI 사라짐 Off
	}
}

void CUI_EnemyHUD_Shard::DeadEnemyHUD()
{
	if (m_vecEnemyHUD.empty())
		return;

	for (auto& iter : m_vecEnemyHUD)
	{
		iter->Set_Dead(true);			// 삭제
	}

	m_vecEnemyHUD.clear(); // 비우기
}

HRESULT CUI_EnemyHUD_Shard::Ready_Components()
{

	return S_OK;
}

HRESULT CUI_EnemyHUD_Shard::Bind_ShaderResources()
{
	/* 공통 */
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

json CUI_EnemyHUD_Shard::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);

	return out_json;
}

void CUI_EnemyHUD_Shard::Load_Desc()
{

}

CUI_EnemyHUD_Shard* CUI_EnemyHUD_Shard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CUI_EnemyHUD_Shard* pInstance = new CUI_EnemyHUD_Shard(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_EnemyHUD_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_EnemyHUD_Shard::Pool()
{
	return new CUI_EnemyHUD_Shard(*this);
}

CGameObject* CUI_EnemyHUD_Shard::Clone(void* pArg)
{
	CUI_EnemyHUD_Shard* pInstance = new CUI_EnemyHUD_Shard(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_EnemyHUD_Shard");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_EnemyHUD_Shard::Free()
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



#include "stdafx.h"
#include "UI_Text.h"
#include "GameInstance.h"
#include "Json_Utility.h"

CUI_Text::CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CUI(pDevice, pContext, strPrototypeTag)
{

}

CUI_Text::CUI_Text(const CUI_Text& rhs)
	: CUI(rhs)
{
}

HRESULT CUI_Text::Initialize_Prototype()
{
	//TODO 원형객체의 초기화과정을 수행한다.
	/* 1.서버로부터 값을 받아와서 초기화한다 .*/
	/* 2.파일입출력을 통해 값을 받아온다.*/

	return S_OK;
}

HRESULT CUI_Text::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

	/* Kind */
	m_eKind = CUI::TEXT;

	if (FAILED(__super::Initialize(pArg))) //!  트랜스폼 셋팅, m_tUIInfo의 bWorldUI 가 false 인 경우에만 직교위치 셋팅
		return E_FAIL;

	return S_OK;
}

void CUI_Text::Priority_Tick(_float fTimeDelta)
{

}

void CUI_Text::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Text::Late_Tick(_float fTimeDelta)
{

}

HRESULT CUI_Text::Render()
{

	return S_OK;
}

HRESULT CUI_Text::Ready_Components()
{
	//if (FAILED(__super::Ready_Components())); // Ready : Texture / MapTexture

	//! For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* 효과가 필요한 녀석은 Map텍스쳐도 추가해주기 */
	return S_OK;
}

HRESULT CUI_Text::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CUI_Text::RenderTextWithLineBreak(const string& text, size_t MaxLineLength)
{
	size_t startPos = 0;
	while (startPos < text.size()) {
		// 현재 줄에서 출력할 문자열의 길이 결정
		size_t lineLength = min(MaxLineLength, text.size() - startPos);

		// 줄바꿈 문자 삽입
		if (startPos + lineLength < text.size()) 
		{
			size_t breakPos = text.find_last_of(" \t\n", startPos + lineLength);
			if (breakPos != std::string::npos && breakPos > startPos) {
				lineLength = breakPos - startPos + 1;
			}
		}

		// 현재 줄 출력
		std::cout << text.substr(startPos, lineLength) << std::endl;

		// 다음 줄 시작 위치 설정
		startPos += lineLength;
	}
}

void CUI_Text::Add_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	TEXTINFO* LoadInfo = new TEXTINFO;

	LoadInfo->fPosX = fPosX;
	LoadInfo->fPosY = fPosY;
	LoadInfo->vColor = vColor;
	LoadInfo->fScale = fScale;
	LoadInfo->vOrigin.x = vOrigin.x;
	LoadInfo->vOrigin.y = vOrigin.y;
	LoadInfo->fRotation = fRotation;

	LoadInfo->strTextKey = strTextKey;
	LoadInfo->strFontTag = strFontTag;
	LoadInfo->strText = strText;

	LoadInfo->vColor.m128_f32[0] = vColor.m128_f32[0];
	LoadInfo->vColor.m128_f32[1] = vColor.m128_f32[1];
	LoadInfo->vColor.m128_f32[2] = vColor.m128_f32[2];
	LoadInfo->vColor.m128_f32[3] = vColor.m128_f32[3];

	m_mapText.emplace(LoadInfo->strTextKey, LoadInfo);
}

CUI_Text::TEXTINFO* CUI_Text::Find_Text(const string& strTextTag)
{
	auto	iter = m_mapText.find(strTextTag);

	if (iter == m_mapText.end())
		return nullptr;

	return iter->second;
}

HRESULT CUI_Text::Find_Change(const string& strTextTag)
{
	auto	iter = m_mapText.find(strTextTag);

	if (iter == m_mapText.end())
		return E_FAIL;

	m_pTextInfo = iter->second;


	m_pGameInstance->String_To_WString(m_pTextInfo->strFontTag, m_strFontTag);
	m_pGameInstance->String_To_WString(m_pTextInfo->strText, m_strText);

	strTextKey = m_pTextInfo->strTextKey;

	m_fPosX = m_pTextInfo->fPosX;
	m_fPosY = m_pTextInfo->fPosY;
	m_fScale = m_pTextInfo->fScale;
	m_vOrigin.x = m_pTextInfo->vOrigin.x;
	m_vOrigin.y = m_pTextInfo->vOrigin.y;
	m_fRotation = m_pTextInfo->fRotation;

	m_vColor.m128_f32[0] = m_pTextInfo->vColor.m128_f32[0];
	m_vColor.m128_f32[1] = m_pTextInfo->vColor.m128_f32[1];
	m_vColor.m128_f32[2] = m_pTextInfo->vColor.m128_f32[2];
	m_vColor.m128_f32[3] = m_pTextInfo->vColor.m128_f32[3];

	return S_OK;
}

void CUI_Text::Change_Text(string strTextKey, string strFontTag, string strText, _float fPosX, _float fPosY, _vector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	m_pGameInstance->String_To_WString(strFontTag, m_strFontTag);
	m_pGameInstance->String_To_WString(strText, m_strText);

	m_fPosX = fPosX;
	m_fPosY = fPosY;
	m_fScale = fScale;
	m_vOrigin.x = vOrigin.x;
	m_vOrigin.y = vOrigin.y;
	m_fRotation = fRotation;

	m_vColor.m128_f32[0] = vColor.m128_f32[0];
	m_vColor.m128_f32[1] = vColor.m128_f32[1];
	m_vColor.m128_f32[2] = vColor.m128_f32[2];
	m_vColor.m128_f32[3] = vColor.m128_f32[3];
}

void CUI_Text::Change_Text(TEXTINFO tTextInfo)
{
	m_pGameInstance->String_To_WString(tTextInfo.strFontTag, m_strFontTag);
	m_pGameInstance->String_To_WString(tTextInfo.strText, m_strText);

	m_fPosX = tTextInfo.fPosX;
	m_fPosY = tTextInfo.fPosY;
	m_fScale = tTextInfo.fScale;
	m_vOrigin.x = tTextInfo.vOrigin.x;
	m_vOrigin.y = tTextInfo.vOrigin.y;
	m_fRotation = tTextInfo.fRotation;

	m_vColor.m128_f32[0] = tTextInfo.vColor.m128_f32[0];
	m_vColor.m128_f32[1] = tTextInfo.vColor.m128_f32[1];
	m_vColor.m128_f32[2] = tTextInfo.vColor.m128_f32[2];
	m_vColor.m128_f32[3] = tTextInfo.vColor.m128_f32[3];
}

json CUI_Text::Save_Desc(json& out_json)
{
	/* 기본정보 저장 */
	__super::Save_Desc(out_json);


	/* 추가정보 저장 */


	//m_pTransformCom->Write_Json(out_json);

	return out_json;
}

void CUI_Text::Load_Desc()
{

}

void CUI_Text::Free()
{
	__super::Free();

	for (auto& pTexture : m_pTextureCom)
	{
		if (pTexture != nullptr)
			Safe_Release(pTexture);
	}

	for (auto& pText : m_mapText)
		Safe_Delete(pText.second);

	m_mapText.clear();
}

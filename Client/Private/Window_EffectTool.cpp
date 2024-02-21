#include "stdafx.h"
#include "Window_EffectTool.h"

#include "imNeoSequencer/imgui_neo_sequencer.h"

#include "GameInstance.h"

#include "Effect.h"
#include "Effect_Rect.h"
#include "Effect_Particle.h"
#include "Effect_Instance.h"


CWindow_EffectTool::CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{

	Safe_AddRef(m_pGameInstance);
}


HRESULT CWindow_EffectTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	//if (FAILED(Ready_Layer_Greed(TEXT("Layer_Greed"))))
	//	return E_FAIL;

	return S_OK;
}

void CWindow_EffectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ShowDialog();

#pragma region 재생바 창
	SetUp_ImGuiDESC(" Play ", ImVec2{ 400.f, 300.f },  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | /*ImGuiWindowFlags_NoResize | /*ImGuiWindowFlags_NoMove |*/ ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.8f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	Update_PlayBarArea_Particle();

	__super::End();
#pragma endregion 재생바 창


#pragma region 시퀀서 창

	SetUp_ImGuiDESC(" Sequencer ", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |  /*ImGuiWindowFlags_NoResize | */ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.8f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	Update_Demo_Sequencer();

	__super::End();
#pragma endregion 시퀀서 창



#pragma region 이펙트 툴

	SetUp_ImGuiDESC(u8"이펙트 툴", ImVec2{ 300.f, 800.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | /*ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |*/ ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.8f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ImGui::Text("Mouse Pos : %d, %d", pt.x, pt.y);
	Update_SaveLoad();


	Update_EffectList();


	if (ImGui::BeginTabBar("Tab_Effect", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(" Particle "))
		{
			//Update_ParticleTab();
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Texture "))
		{
			//ImGui::Text(" Texture Effect ");
			//Update_TextureTab();
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Mesh "))
		{
			//ImGui::Text(" Mesh Effect ");
			//Update_MeshTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Trail "))
		{
			//ImGui::Text(" Mesh Effect ");
			//Update_TrailTab();
			ImGui::EndTabItem();
		}

	}
	ImGui::EndTabBar();

	__super::End();
#pragma endregion 이펙트 툴

}

void CWindow_EffectTool::Render()
{

}

void CWindow_EffectTool::Update_PlayBarArea_Particle()
{
	/* 재생바 */
	if (nullptr != m_pCurEffect)
	{
		_float fSequenceTimePos = m_pCurEffectDesc->fSequenceAcc;
		ImGui::SliderFloat("SequenceTimePos", &fSequenceTimePos, 0.f, m_pCurEffectDesc->fSequenceTime);


		/* 재생, 정지, 리셋 */
		if (ImGui::Button("   Play   "))
		{
			m_pCurEffectDesc->bPlay = TRUE;

		}
		ImGui::SameLine();
		if (ImGui::Button("   Stop   "))
		{
			m_pCurEffectDesc->bPlay = FALSE;

		}
		ImGui::SameLine();
		if (ImGui::Button("   Reset  "))
		{
			m_pCurEffect->ReSet_Effect();
		}

		/* 루프 여부 */
		ImGui::RadioButton("Loop", &m_iLoop, 0);  ImGui::SameLine();
		ImGui::RadioButton("Once", &m_iLoop, 1);

		if (0 == m_iLoop)
		{
			m_pCurEffectDesc->bLoop = TRUE;
		}
		else if (1 == m_iLoop)
		{
			m_pCurEffectDesc->bLoop = FALSE;
		}


		/* 재생, 역재생 */
		if (ImGui::Button(" Normal "))
		{
			m_pCurEffectDesc->bReverse = FALSE;
		}

		ImGui::SameLine();
		if (ImGui::Button(" Reverse "))
		{
			m_pCurEffectDesc->bReverse = TRUE;
		}
		ImGui::SameLine(); HelpMarker(u8"재생, 역재생");

	}
}


void CWindow_EffectTool::Update_TrailTab()
{



}

void CWindow_EffectTool::Update_EffectList()
{
	/* 비어있는 이펙트 객체 생성 */
	if (ImGui::Button("     Create     "))
	{
		Create_EffectObject(TEXT("Layer_Effect"));
	}

	/* 이펙트 리스트 & 현재 이펙트 선택 */
	if (ImGui::ListBox(" Effects ", &m_iCurEffectIndex, m_szEffectNames, m_pEffects.size(), (_int)6))
	{
		wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]);
		m_pCurEffect = m_pEffects.find(strCurName)->second;

		m_iCurPartIndex = m_CurPartObjects.size();
		/* 문자열 초기화 */
		if (nullptr != m_szPartNames)
		{
			for (_int i = 0; i < m_iCurPartIndex; ++i)
			{
				m_szPartNames[i] = nullptr;
			}
			m_szPartNames = nullptr;
		}

		Update_CurMembers(strCurName);

		m_iCurPartIndex = m_CurPartObjects.size();
		m_szPartNames = new char* [m_iCurPartIndex];

		_int iCount = 0;
		for (auto& Pair : m_CurPartObjects)
		{
			const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
			m_szPartNames[iCount] = new char[utf8Str.length() + 1];
			strcpy(m_szPartNames[iCount], utf8Str.c_str());

			iCount++;
		}

		m_iCurPartIndex = 0;
		if (!m_CurPartObjects.empty())
		{
			m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.begin()->second);
		}
		else
		{
			m_pCurPartEffect = nullptr;
		}
	}


	if (nullptr != m_pCurEffect)
	{
		/* 이펙트 파트오브젝트 리스트 & 현재 파트오브젝트 선택 */
		if (ImGui::ListBox(" Parts ", &m_iCurPartIndex, m_szPartNames, m_CurPartObjects.size(), (_int)6))
		{
			wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szPartNames[m_iCurPartIndex]);
			m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.find(strCurName)->second);

			//Update_CurParameters_Parts();
		}

		if (ImGui::DragFloat3("Times_Effect", m_vTimes_Effect, 0.2f, 0.f))
		{
			m_pCurEffectDesc->fWaitingTime = m_vTimes_Effect[0];
			m_pCurEffectDesc->fLifeTime = m_vTimes_Effect[1];
			m_pCurEffectDesc->fRemainTime = m_vTimes_Effect[2];
		}


		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();
			if (CEffect_Void::PARTICLE == eType_Effect)
			{
				m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
				m_pParticlePointDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom()->Get_Desc();

				/* 선택 파티클 활성화 */
				if (ImGui::Button(" Active Particle_Part "))
				{
					m_pParticleDesc->bActive_Tool = TRUE;
				}
				ImGui::SameLine();
				/* 선택 파티클 비활성화 */
				if (ImGui::Button(" UnActive Particle_Part "))
				{
					m_pParticleDesc->bActive_Tool = FALSE;
				}

				if (ImGui::DragFloat3("Times_Particle", m_vTimes_Particle, 0.2f, 0.f))
				{
					//if (m_vTimes_Particle[0] > m_vTimes_Particle[1])
					//{

					//}

					m_pCurPartEffect->Set_WaitingTime(m_vTimes_Particle[0]);
					m_pCurPartEffect->Set_LifeTime(m_vTimes_Particle[1]);
					m_pParticleDesc->fRemainTime = m_vTimes_Particle[2];
				}

				if (ImGui::Button(" ReSet_Particle "))
				{
					dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom()->ReSet();
				}

			}	

			if (CEffect_Void::RECT == eType_Effect)
			{

			}

			if (CEffect_Void::INSTANCE == eType_Effect)
			{

			}

			if (CEffect_Void::MESH == eType_Effect)
			{

			}

		}


		// 위치 표시
		_float4 vPos = m_pCurEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Effect Pos  : %.2f %.2f %.2f", vPos.x, vPos.y, vPos.z);


		/* 선택 이펙트 활성화 */
		if (ImGui::Button(" Active_Effect "))
		{
			m_pCurEffectDesc->bActive_Tool = TRUE;
		}
		/* 선택 이펙트 비활성화 */
		ImGui::SameLine();	
		if (ImGui::Button(" UnActive_Effect "))
		{
			m_pCurEffectDesc->bActive_Tool = FALSE;
		}




		// =========================================

		ImGui::SeparatorText("");
		if (ImGui::Button(" Add Particle "))
		{
			Add_Part_Particle();
		}


		ImGui::SeparatorText("");
	}


}

HRESULT CWindow_EffectTool::Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner)
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	CEffect::EFFECT_DESC	tEffectDesc = {};
	tEffectDesc.fSpeedPerSec = { 5.f };
	tEffectDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

	tEffectDesc.bActive_Tool	= { TRUE };
	tEffectDesc.bPlay			= { TRUE };
	tEffectDesc.bLoop			= { TRUE };
	tEffectDesc.bFinished		= { FALSE };
	tEffectDesc.bReverse		= { FALSE };
	tEffectDesc.bRender			= { TRUE };

	tEffectDesc.fTimeAcc		= { 0.f };
	tEffectDesc.fWaitingAcc		= { 0.f };
	tEffectDesc.fSequenceAcc	= { 0.f };

	tEffectDesc.fLifeTime = { 3.f };
	tEffectDesc.fWaitingTime = { 0.f };
	tEffectDesc.fRemainTime = { 0.f };
	tEffectDesc.fSequenceTime = tEffectDesc.fWaitingTime + tEffectDesc.fLifeTime + tEffectDesc.fRemainTime;
	tEffectDesc.fLifeTimeRatio = min(1.f, tEffectDesc.fTimeAcc / tEffectDesc.fLifeTime);
	tEffectDesc.fDissolveStartTime = { 1.f };

	tEffectDesc.pOwner = pOwner;
	if (nullptr != pOwner)
	{
		tEffectDesc.bParentPivot = TRUE;
		tEffectDesc.matPivot = pOwner->Get_Transform()->Get_WorldFloat4x4();
		XMStoreFloat4x4(&tEffectDesc.matOffset, XMMatrixIdentity());
	}
	else
	{
		tEffectDesc.bParentPivot = FALSE;
		XMStoreFloat4x4(&tEffectDesc.matPivot, XMMatrixIdentity());
		XMStoreFloat4x4(&tEffectDesc.matOffset, XMMatrixIdentity());
	}

	CEffect* pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Effect"), &tEffectDesc));

#pragma region 이름 붙여서 컨테이너에 넣기
	wstring strName = TEXT("");
	wstring strFrontName = TEXT("Effect");

	_int iMaxNum = -1;
	wstring strPin = TEXT("");

	for (auto& iter : m_pEffects)
	{
		if (nullptr == iter.second)
			continue;

		if (strFrontName == m_pGameInstance->Remove_LastNumChar(iter.first, 4))
		{
			_int iPinNum = stoi(m_pGameInstance->Get_LastNumChar(iter.first, 3));

			if (iMaxNum < iPinNum)
				iMaxNum = iPinNum;
		}
	}
	/* 최댓값이 -1이라는 것은 해당 이름과 같은게 없으므로 고유번호를 000으로 세팅한다. */
	if (-1 == iMaxNum)
		strName = strFrontName + L"_000";
	else /* 아니라면 최댓값에 + 1을 하여 고유 번호로 세팅한다. */
	{
		_int iPinNum = iMaxNum + 1;

		if (0 == iPinNum / 10)
			strPin = L"_00" + to_wstring(iPinNum);
		else if (0 == iPinNum / 100)
			strPin = L"_0" + to_wstring(iPinNum);
		else
			strPin = L"_" + to_wstring(iPinNum);

		strName = strFrontName + strPin;
	}
	m_pEffects.emplace(strName, pEffect);
	m_pCurEffect = pEffect;
#pragma endregion


#pragma region 이름 리스트 초기화&업데이트
	m_iCurEffectIndex = m_pEffects.size();
	/* 문자열 초기화 */
	if (nullptr != m_szEffectNames)
	{
		for (_int i = 0; i < m_iCurEffectIndex; ++i)
		{
			m_szEffectNames[i] = nullptr;
		}
		m_szEffectNames = nullptr;
	}
	m_szEffectNames = new char* [m_iCurEffectIndex];
	_int iCount = 0;
	for (auto& Pair : m_pEffects)
	{
		const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
		m_szEffectNames[iCount] = new char[utf8Str.length() + 1];
		strcpy(m_szEffectNames[iCount], utf8Str.c_str());

		iCount++;
	}
	m_iCurEffectIndex -= 1;
#pragma endregion


	Update_CurMembers(m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]));

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Particle()
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Particle::PARTICLE_DESC	tParticleDesc = {};
		tParticleDesc.fSpeedPerSec		= { 5.f };
		tParticleDesc.fRotationPerSec	= { XMConvertToRadians(50.0f) };

		tParticleDesc.eType = CEffect_Particle::SINGLE;
		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Particle_Base");
		//tDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

		//tDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 /*1*/ };

		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

		tParticleDesc.strShaderTag = TEXT("Prototype_Component_Shader_Particle_Point");
		tParticleDesc.iShaderPassIndex = { 0 };
		tParticleDesc.iRenderGroup = { 7 };

		tParticleDesc.iCurInstanceCnt = { (_uint)m_iNumInstance };
		tParticleDesc.iMaxInstanceCnt = { (_uint)m_iMaxNumInstance };

		tParticleDesc.fUV_RotDegree = { 0.f };

		tParticleDesc.pOwner = m_pCurEffect;


#pragma region 리스트 문자열 관련
		wstring strName = TEXT("");
		wstring strFrontName = TEXT("Part_Particle");

		_int iMaxNum = -1;
		wstring strPin = TEXT("");

		for (auto& iter : m_CurPartObjects)
		{
			if (nullptr == iter.second)
				continue;

			if (strFrontName == m_pGameInstance->Remove_LastNumChar(iter.first, 4))
			{
				_int iPinNum = stoi(m_pGameInstance->Get_LastNumChar(iter.first, 3));

				if (iMaxNum < iPinNum)
					iMaxNum = iPinNum;
			}
		}
		/* 최댓값이 -1이라는 것은 해당 이름과 같은게 없으므로 고유번호를 000으로 세팅한다. */
		if (-1 == iMaxNum)
			strName = strFrontName + L"_000";
		else /* 아니라면 최댓값에 + 1을 하여 고유 번호로 세팅한다. */
		{
			_int iPinNum = iMaxNum + 1;

			if (0 == iPinNum / 10)
				strPin = L"_00" + to_wstring(iPinNum);
			else if (0 == iPinNum / 100)
				strPin = L"_0" + to_wstring(iPinNum);
			else
				strPin = L"_" + to_wstring(iPinNum);

			strName = strFrontName + strPin;
		}

		tParticleDesc.strPartTag = strName;

		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Particle"), strName, &tParticleDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Find_PartObject(strName));
		m_pCurPartEffect->Set_EffectType(CEffect_Void::PARTICLE);
		
		m_iCurPartIndex = m_CurPartObjects.size();
		/* 문자열 초기화 */
		if (nullptr != m_szPartNames)
		{
			for (_int i = 0; i < m_iCurPartIndex; ++i)
			{
				m_szPartNames[i] = nullptr;
			}
			m_szPartNames = nullptr;
		}

		m_szPartNames = new char* [m_iCurPartIndex];

		_int iCount = 0;
		for (auto& Pair : m_CurPartObjects)
		{
			const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
			m_szPartNames[iCount] = new char[utf8Str.length() + 1];
			strcpy(m_szPartNames[iCount], utf8Str.c_str());

			iCount++;
		}
		m_iCurPartIndex -= 1;
#pragma endregion


	}

	return S_OK;
}

void CWindow_EffectTool::Update_CurMembers(wstring strName)
{
	if (nullptr != m_pCurEffect)
	{
		if (TEXT("") != strName)
		{
			const string utf8Str = m_pGameInstance->Wstring_To_UTF8(strName);
			m_cCurEffectName = new char[utf8Str.length() + 1];
			strcpy(m_cCurEffectName, utf8Str.c_str());
		}

		m_pCurEffectDesc = m_pCurEffect->Get_Desc();
		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
	}
}

void CWindow_EffectTool::Update_CurParameters()
{

}

void CWindow_EffectTool::Update_Demo_Sequencer()
{

	if (nullptr != m_pCurEffect)
	{
		int32_t startFrame = -2;
		int32_t endFrame = static_cast<int32_t>((m_pCurEffectDesc->fSequenceTime) + 2);

		m_iCurSequenceTimePos = static_cast<int32_t>(m_pCurEffectDesc->fSequenceAcc);

		// 시퀀서
		if (ImGui::BeginNeoSequencer("Sequencer", &m_iCurSequenceTimePos, &startFrame, &endFrame, { 0, 0 },
			ImGuiNeoSequencerFlags_EnableSelection |
			ImGuiNeoSequencerFlags_Selection_EnableDragging |
			ImGuiNeoSequencerFlags_Selection_EnableDeletion)
			)
		{
			_bool	bTransformOpen = true;
			if (ImGui::BeginNeoGroup(m_cCurEffectName, &bTransformOpen))
			{
				if (!m_CurPartObjects.empty())
				{		
					_int iCount = 0;
					for (auto& iter : m_CurPartObjects)
					{		
						vector<ImGui::FrameIndexType> keys = {};

						if(!keys.empty())
							keys.clear();

						CEffect_Void* pPartObj = dynamic_cast<CEffect_Void*>(iter.second);
						_float fWaitingTime = pPartObj->Get_WaitingTime();
						_float fPartLifeTime = pPartObj->Get_LifeTime();
						_float fSequenceTime = pPartObj->Get_SequenceTime();

						keys.push_back((_int)fWaitingTime);
						keys.push_back((_int)fPartLifeTime);
						keys.push_back((_int)fSequenceTime);

						if (ImGui::BeginNeoTimelineEx(m_szPartNames[iCount]))
						{
							for (auto&& v : keys)
							{
								ImGui::NeoKeyframe(&v);
								// Per keyframe code here
							}


							ImGui::EndNeoTimeLine();
						}

						iCount++;
					}


					
				}
				ImGui::EndNeoGroup();
			}

			ImGui::EndNeoSequencer();
		}
	}

}



void CWindow_EffectTool::Update_SaveLoad()
{
}

HRESULT CWindow_EffectTool::Save_Function(string strPath, string strFileName)
{
	return S_OK;
}

HRESULT CWindow_EffectTool::Load_Function(string strPath, string strFileName)
{
	return S_OK;
}

HRESULT CWindow_EffectTool::Ready_Layer_Greed(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC	tDesc = {};
	tDesc.fSpeedPerSec = { 0.f };
	tDesc.fRotationPerSec = { XMConvertToRadians(0.0f) };

	FAILED_CHECK(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Greed"), &tDesc));

	return S_OK;
}


CWindow_EffectTool* CWindow_EffectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_EffectTool* pInstance = new CWindow_EffectTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_EffectTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_EffectTool::Free()
{
	__super::Free();

	for (auto& Pair : m_CurPartObjects)
		Safe_Release(Pair.second);
	m_CurPartObjects.clear();

	for (auto& Pair : m_pEffects)
		Safe_Release(Pair.second);
	m_pEffects.clear();


	if (nullptr != m_pCurEffect)
		Safe_Release(m_pCurEffect);

	if (nullptr != m_pCurPartEffect)
		Safe_Release(m_pCurPartEffect);


	m_cCurEffectName = nullptr;
	m_cCurPartName = nullptr;

	if (nullptr != m_szEffectNames)
	{
		for (_int i = 0; i < m_iCurEffectIndex; ++i)
		{
			m_szEffectNames[i] = nullptr;
		}
		m_szEffectNames = nullptr;
	}

	if (nullptr != m_szPartNames)
	{
		for (_int i = 0; i < m_iCurPartIndex; ++i)
		{
			m_szPartNames[i] = nullptr;
		}
		m_szPartNames = nullptr;
	}


	Safe_Release(m_pGameInstance);
}
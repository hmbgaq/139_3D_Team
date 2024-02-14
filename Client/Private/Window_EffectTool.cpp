#include "stdafx.h"
#include "Window_EffectTool.h"

#include "GameInstance.h"

#include "Particle_Edit.h"

CWindow_EffectTool::CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{

	Safe_AddRef(m_pGameInstance);
}


HRESULT CWindow_EffectTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;


	return S_OK;
}

void CWindow_EffectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save"))
			{

			}
			if (ImGui::MenuItem("Load"))
			{

			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	if (ImGui::BeginTabBar("Tab_Effect", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(" Particle "))
		{
			//if (ImGui::CollapsingHeader(" Particle ", ImGuiTabBarFlags_None))
			{
				/* 기본 파티클 생성 */
				if (ImGui::Button(" Create New Particle "))
				{
					Create_NewParticle();
				}

				/* 선택 파티클 삭제 */
				ImGui::SameLine();
				if (ImGui::Button(" Delete Particle "))
				{

				}


				/* 파티클 리스트 & 현재 파티클 선택 */
				if (ImGui::ListBox(" Particle List ", &m_iCurParticleIndex, m_szParticleNames, m_pParticles.size(), 6))
				{
					wstring strCurName = CharToWstring(m_szParticleNames[m_iCurParticleIndex]);
					m_pCurParticle = m_pParticles.find(strCurName)->second;

					m_vRotationX[1] = m_pCurParticle->Get_RotX();
					m_vRotationY[1] = m_pCurParticle->Get_RotY();
					m_vRotationZ[1] = m_pCurParticle->Get_RotZ();
				}
	

				/* 재생바 */
				if (nullptr != m_pCurParticle)
				{		
					_float fLifeTimePosition = m_pCurParticle->Get_TimePosition();
					ImGui::SliderFloat("Play_Particle", &fLifeTimePosition, 0.f, m_pCurParticle->Get_Desc()->vLifeTime.y);


					///* 현재 재생 위치(포지션) 이동 */
					//if (ImGui::SliderFloat(" ", &m_fLifeTimePositionEdit, 0.f, m_pCurParticle->Get_Desc()->vLifeTime.y))
					//{
					//	m_pCurParticle->Set_Play(FALSE);
					//	m_pCurParticle->Set_TimePosition(m_fLifeTimePositionEdit);
					//}
				}


				/* 회전 범위 */
				if (nullptr != m_pCurParticle)
				{
					/* RotX */
					ImGui::DragFloat2("RotationX", m_vRotationX, 1.f, 0.f);

					if (0 > m_vRotationX[0])
						m_vRotationX[0] = 0.f;

					if (m_vRotationX[0] > m_vRotationX[1])
						m_vRotationX[1] = m_vRotationX[0];

					//m_pCurParticle->Set_RotX(vRotationX[0]);
					m_pCurParticle->Set_RotX(m_vRotationX[1]);


					/* RotY */
					ImGui::DragFloat2("RotationY", m_vRotationY, 1.f, 0.f);

					if (0 > m_vRotationY[0])
						m_vRotationY[0] = 0.f;

					if (m_vRotationY[0] > m_vRotationY[1])
						m_vRotationY[1] = m_vRotationY[0];

					//m_pCurParticle->Set_RotX(vRotationX[0]);
					m_pCurParticle->Set_RotY(m_vRotationY[1]);


					/* RotZ */
					ImGui::DragFloat2("RotationZ", m_vRotationZ, 1.f, 0.f);

					if (0 > m_vRotationZ[0])
						m_vRotationZ[0] = 0.f;

					if (m_vRotationZ[0] > m_vRotationZ[1])
						m_vRotationZ[1] = m_vRotationZ[0];

					//m_pCurParticle->Set_RotX(vRotationX[0]);
					m_pCurParticle->Set_RotZ(m_vRotationZ[1]);
				}


				/* 가속도 */
				if (nullptr != m_pCurParticle)
				{
					ImGui::SliderFloat("AccPos_Particle", &m_fParticleAccPosition, 0.f, 1.f);
					m_pCurParticle->Set_AccPosition(m_fParticleAccPosition);


					ImGui::SliderFloat("ACC_Particle", &m_fParticleAcceleration, 0.f, 100.f);
					m_pCurParticle->Set_Acceleration(m_fParticleAcceleration);
				}


				/* 재생, 역재생 */
				ImGui::SeparatorText("Play Type");
				ImGui::RadioButton("Normal", &m_iPlayType, 0);  ImGui::SameLine();	
				ImGui::RadioButton("Reverse", &m_iPlayType, 1);
		
				if (0 == m_iPlayType)
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_ReversePlay(FALSE);
					}
				}
				else if (1 == m_iPlayType)
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_ReversePlay(TRUE);
					}
				}


				/* 루프 여부 */
				ImGui::SeparatorText("Loop");
				ImGui::RadioButton("Loop", &m_iLoop, 0);  ImGui::SameLine();
				ImGui::RadioButton("Once", &m_iLoop, 1);

				if (0 == m_iLoop)
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_Loop(TRUE);
					}
				}
				else if (1 == m_iLoop)
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_Loop(FALSE);
					}
				}


				/* 재생, 정지, 리셋 */
				ImGui::SeparatorText(" ");
				if (ImGui::Button(" Play "))
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_Play(TRUE);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button(" Stop "))
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->Set_Play(FALSE);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					if (nullptr != m_pCurParticle)
					{
						m_pCurParticle->ReSet();
					}
				}


				/* 전체 재생, 정지, 리셋 */
				if (ImGui::Button(" All Play "))
				{
					if (!m_pParticles.empty())
					{
						for (auto& Pair : m_pParticles)
						{
							Pair.second->Set_Play(TRUE);
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button(" All Stop "))
				{
					if (!m_pParticles.empty())
					{
						for (auto& Pair : m_pParticles)
						{
							Pair.second->Set_Play(FALSE);
						}
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("All Reset"))
				{
					if (!m_pParticles.empty())
					{
						for (auto& Pair : m_pParticles)
						{
							Pair.second->ReSet();
						}
					}
				}


			}
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Mesh "))
		{
			ImGui::Text(" Mesh Effect ");

			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Sprite "))
		{
			ImGui::Text(" Sprite Effect ");

			ImGui::EndTabItem();
		}

	}
	ImGui::EndTabBar();

	__super::End();
}

void CWindow_EffectTool::Render()
{

}


HRESULT CWindow_EffectTool::Ready_Layer_Particle(const wstring& strLayerTag)
{
	CGameObject::GAMEOBJECT_DESC	tDesc = {};
	tDesc.fSpeedPerSec = { 0.f };
	tDesc.fRotationPerSec = { XMConvertToRadians(0.0f) };

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Particle_Edit"), &tDesc)))
		return E_FAIL;

	CVIBuffer_Particle_Point* pBuffer = dynamic_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Get_Component(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Com_VIBuffer"), m_pParticles.size()));

	wstring strName = TEXT("Particle_");
	strName = strName + to_wstring(m_pParticles.size() + 1);
	m_pParticles.emplace(strName, pBuffer);

	m_pCurParticle = pBuffer;
	m_vRotationX[1] = m_pCurParticle->Get_RotX();
	m_vRotationY[1] = m_pCurParticle->Get_RotY();
	m_vRotationZ[1] = m_pCurParticle->Get_RotZ();
	
	m_fParticleAcceleration = m_pCurParticle->Get_Acceleration();
	m_fParticleAccPosition = m_pCurParticle->Get_AccPosition();

	return S_OK;
}


string CWindow_EffectTool::WstringToUTF8(const wstring& wstr)
{
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (utf8Length == 0)
	{
		return string();
	}

	string utf8Str(utf8Length + 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], utf8Length + 1, nullptr, nullptr);

	return utf8Str;
}


wstring CWindow_EffectTool::CharToWstring(char* szChar)
{
	return wstring(szChar, szChar + strlen(szChar));
}

HRESULT CWindow_EffectTool::Create_NewParticle()
{
	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	m_iCurParticleIndex = m_pParticles.size();

	/* 문자열 초기화 */
	if (nullptr != m_szParticleNames)
	{
		for (_int i = 0; i < m_iCurParticleIndex; ++i)
		{
			m_szParticleNames[i] = nullptr;
		}
		m_szParticleNames = nullptr;
	}

	m_szParticleNames = new char* [m_iCurParticleIndex];

	_int iCount = 0;
	for (auto& Pair : m_pParticles)
	{
		const string utf8Str = WstringToUTF8(Pair.first);
		m_szParticleNames[iCount] = new char[utf8Str.length() + 1];
		strcpy(m_szParticleNames[iCount], utf8Str.c_str());

		iCount++;
	}

	m_iCurParticleIndex -= 1;

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

	for (auto& Pair : m_pParticles)
		Safe_Release(Pair.second);
	m_pParticles.clear();

	if(nullptr != m_pCurParticle)
		Safe_Release(m_pCurParticle);

	if (nullptr != m_szParticleNames)
	{
		for (_int i = 0; i < m_iCurParticleIndex; ++i)
		{
			m_szParticleNames[i] = nullptr;
		}
		m_szParticleNames = nullptr;
	}

	Safe_Release(m_pGameInstance);
}
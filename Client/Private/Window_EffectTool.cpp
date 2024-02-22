#include "stdafx.h"
#include "Window_EffectTool.h"

#include "imNeoSequencer/imgui_neo_sequencer.h"

#include "GameInstance.h"

#include "Effect_Particle.h"
#include "Effect_Texture.h"
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

	SetUp_ImGuiDESC(" Sequencer ", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |  /*ImGuiWindowFlags_NoResize |*/  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.8f));
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
	Update_SaveLoad_Particle();

	if (ImGui::BeginTabBar("Tab_Effect", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(" Particle "))
		{
			Update_ParticleTab();
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Texture "))
		{
			//ImGui::Text(" Texture Effect ");
			Update_TextureTab();
			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(" Mesh "))
		{
			//ImGui::Text(" Mesh Effect ");
			Update_MeshTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Trail "))
		{
			//ImGui::Text(" Mesh Effect ");
			Update_TrailTab();
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
	if (nullptr != m_pCurParticle)
	{
		_float fLifeTimePosition = m_pCurParticle->Get_VIBufferCom()->Get_TimePosition();
		ImGui::SliderFloat("TimePos", &fLifeTimePosition, 0.f, m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vMinMaxLifeTime.y);
	}

	/* 재생, 정지, 리셋 */
	if (ImGui::Button("   Play   "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Play(TRUE);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("   Stop   "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Play(FALSE);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("   Reset  "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->ReSet();
		}
	}


	/* 루프 여부 */
	ImGui::RadioButton("Loop", &m_iLoop, 0);  ImGui::SameLine();
	ImGui::RadioButton("Once", &m_iLoop, 1);

	if (0 == m_iLoop)
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Loop(TRUE);
		}
	}
	else if (1 == m_iLoop)
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Loop(FALSE);
		}
	}


	/* 재생, 역재생 */
	if (ImGui::Button(" Normal "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_ReversePlay(FALSE);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button(" Reverse "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_ReversePlay(TRUE);
		}
	}
	ImGui::SameLine(); HelpMarker(u8"재생, 역재생");

}

void CWindow_EffectTool::Update_ParticleTab()
{
	//if (ImGui::CollapsingHeader(" Particle ", ImGuiTabBarFlags_None))
	{
		Update_Demo_Particle();

		//ImGui::SeparatorText("");
		Update_ListArea_Particle();

		ImGui::SeparatorText("");
		Update_PlayArea_Particle();

		//ImGui::SeparatorText("");
		//Update_Demo_Sequencer();

		ImGui::SeparatorText("");
		Update_AppearArea_Particle();

		ImGui::SeparatorText("");
		Update_ActionArea_Particle();

		ImGui::SeparatorText("");
		Update_WorldPositionArea_Particle();

		ImGui::SeparatorText("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

	}
}

void CWindow_EffectTool::Update_TextureTab()
{
	Update_Demo_Texture();

	/* 텍스처 이펙트 리스트 & 현재 텍스처 이펙트 선택 */
	if (ImGui::ListBox(" List_Tex ", &m_iCurEffectTextureIndex, m_szEffectTextureNames, m_pEffectTextures.size(), (_int)4))
	{
		wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectTextureNames[m_iCurEffectTextureIndex]);
		m_pCurEffectTexture = m_pEffectTextures.find(strCurName)->second;

		Update_CurParameters_Tex();
	}

	/* 선택 파티클 활성화 */
	if (ImGui::Button(" Active Texture "))
	{
		if (nullptr != m_pCurEffectTexture)
		{
			m_pCurEffectTexture->Set_Active(TRUE);
		}
	}
	/* 선택 파티클 비활성화 */
	ImGui::SameLine();
	if (ImGui::Button(" UnActive Texture "))
	{
		if (nullptr != m_pCurEffectTexture)
		{
			m_pCurEffectTexture->Set_Active(FALSE);
		}
	}

	ImGui::Separator();
	Update_Texture_Edit();

}

void CWindow_EffectTool::Update_MeshTab()
{
	Update_Demo_Effect_Instance();

	ImGui::Separator();
	Update_Trnasform_Effect_Instance();

}

void CWindow_EffectTool::Update_TrailTab()
{



}

void CWindow_EffectTool::Update_Demo_Sequencer()
{
	if (nullptr != m_pCurParticle)
	{
		int32_t startFrame = -2;
		int32_t endFrame = (int32_t)((m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vMinMaxLifeTime.y) + 2);


		m_iCurrentFrame = (int32_t)m_pCurParticle->Get_VIBufferCom()->Get_TimePosition();

		if (ImGui::BeginNeoSequencer("Sequencer", &m_iCurrentFrame, &startFrame, &endFrame, { 0, 0 },
			ImGuiNeoSequencerFlags_EnableSelection |
			ImGuiNeoSequencerFlags_Selection_EnableDragging |
			ImGuiNeoSequencerFlags_Selection_EnableDeletion)
			)
		{
			_bool	bTransformOpen = true;
			if (ImGui::BeginNeoGroup(m_cCurParticleName, &bTransformOpen))
			{
				vector<ImGui::FrameIndexType> keys = {};

				//if (!m_mapCutScene.empty())
				//{
				//	keys.clear();	// 깨끗하게 비우고

				//	// 컷신의 키 프레임 위치 불러오기
				//	_int MaxCnt = Find_CutScene(ConverCtoWC2(m_cMyCutScene))->Get_MaxFrameIdx();

				//	for (int i = 0; i <= MaxCnt; ++i)
				//	{
				//		_int ikeyPos = (_int)((Find_CutScene(ConverCtoWC2(m_cMyCutScene))->Get_KeyFrame(i).fKeyTime) * 10);
				//		keys.push_back(ikeyPos);
				//	}
				//}

				// 파티클
				if (ImGui::BeginNeoTimeline("Particle", keys))
				{
					ImGui::EndNeoTimeLine();
				}
				ImGui::EndNeoGroup();
			}

			ImGui::EndNeoSequencer();
		}
	}

}

void CWindow_EffectTool::Update_Demo_Particle()
{
	if (ImGui::Button(" Sphere_Demo "))
	{
		Create_New_Particle();

		json In_Json;
		char filePath[MAX_PATH] = "../Bin/DataFiles/Data_Effect/Particle_Info/Particle_TestSphere_Normal_Backup_Info";
		CJson_Utility::Load_Json(filePath, In_Json);

		m_pCurParticle->Load_FromJson(In_Json);

		Update_CurParameters();
	}
	ImGui::SameLine();
	if (ImGui::Button(" Sphere_Reverse_Demo "))
	{
		Create_New_Particle();

		json In_Json;
		char filePath[MAX_PATH] = "../Bin/DataFiles/Data_Effect/Particle_Info/Particle_TestSphere_Reverse_Backup_Info";
		CJson_Utility::Load_Json(filePath, In_Json);

		m_pCurParticle->Load_FromJson(In_Json);

		Update_CurParameters();
	}
}

void CWindow_EffectTool::Update_ListArea_Particle()
{
	/* 기본 파티클 생성 */
	if (ImGui::Button("     Create     "))
	{
		Create_New_Particle();
	}

	///* 선택 파티클 삭제 */
	//ImGui::SameLine();
	//if (ImGui::Button(" Delete "))
	//{

	//}

	if (nullptr != m_pCurParticle)
	{
		_float4 vParticlePos = m_pCurParticle->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Particle Pos  : %.2f %.2f %.2f", vParticlePos.x, vParticlePos.y, vParticlePos.z);
	}

	/* 파티클 리스트 & 현재 파티클 선택 */
	if (ImGui::ListBox(" List_Particle ", &m_iCurParticleIndex, m_szParticleNames, m_pParticles.size(), (_int)4))
	{
		wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szParticleNames[m_iCurParticleIndex]);
		m_pCurParticle = m_pParticles.find(strCurName)->second;

		Update_CurParameters();
	}


	/* 선택 파티클 활성화 */
	if (ImGui::Button(" Active Particle "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Set_Active(TRUE);
		}
	}
	/* 선택 파티클 비활성화 */
	ImGui::SameLine();
	if (ImGui::Button(" UnActive Particle "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Set_Active(FALSE);
		}
	}

	/* 전체 재생, 정지, 리셋 */
	if (ImGui::Button(" All Play "))
	{
		if (!m_pParticles.empty())
		{
			for (auto& Pair : m_pParticles)
			{
				Pair.second->Get_VIBufferCom()->Set_Play(TRUE);
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
				Pair.second->Get_VIBufferCom()->Set_Play(FALSE);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" All Reset "))
	{
		if (!m_pParticles.empty())
		{
			for (auto& Pair : m_pParticles)
			{
				Pair.second->Get_VIBufferCom()->ReSet();
			}
		}
	}

}

void CWindow_EffectTool::Update_PlayArea_Particle()
{

	/* 텍스처 선택 (이미지 보이게 수정하기) */
	if (ImGui::CollapsingHeader(" Texture ", ImGuiTabBarFlags_None))
	{
		if (nullptr != m_pCurParticle)
		{
			if (ImGui::InputInt("Texture", &m_iTextureIndex, 1))
			{
				if (13 <= m_iTextureIndex)
					m_iTextureIndex = 13;

				if (0 > m_iTextureIndex)
					m_iTextureIndex = 0;

				m_pCurParticle->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, m_iTextureIndex);
			}

			if (ImGui::InputInt("Mask_Tex", &m_iMaskTexIndex), 1)
			{
				if (17 < m_iMaskTexIndex)
					m_iMaskTexIndex = 17;

				if (0 > m_iMaskTexIndex)
					m_iMaskTexIndex = 0;

				m_pCurParticle->Set_TextureIndex(CEffect::TEXTURE_MASK, m_iMaskTexIndex);
			}

			if (ImGui::InputInt("Noise_Tex", &m_iNoiseTexIndex), 1)
			{
				if (5 < m_iNoiseTexIndex)
					m_iNoiseTexIndex = 5;

				if (0 > m_iNoiseTexIndex)
					m_iNoiseTexIndex = 0;

				m_pCurParticle->Set_TextureIndex(CEffect::TEXTURE_NOISE, m_iNoiseTexIndex);
			}

		}
	}


	/* 파티클 업데이트 모양(타입) */
	ImGui::SeparatorText(" Action Type ");
	if (ImGui::Button(" Sphere "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Spark "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::SPARK);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Fall "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::FALL);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Rise "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::RISE);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Blink "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::BLINK);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Tornado "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::TORNADO);
		}
	}

	if (nullptr != m_pCurParticle)
	{
		if (CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE == m_pCurParticle->Get_VIBufferCom()->Get_Desc()->eType_Action)
		{
			if (ImGui::DragFloat("MaxLength", &m_fMaxLengthPosition, 1.f, 0.1f, 360.f))
				m_pCurParticle->Get_VIBufferCom()->Set_MaxLengthPosition(m_fMaxLengthPosition);
		}

	}


	/* 파티클 페이드 인아웃 사용 */
	ImGui::SeparatorText(" Fade Type ");
	if (ImGui::Button(" Fade_None "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Fade(CVIBuffer_Particle_Point::TYPE_FADE::FADE_NONE);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Fade_In "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Fade(CVIBuffer_Particle_Point::TYPE_FADE::FADE_IN);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Fade_Out "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Fade(CVIBuffer_Particle_Point::TYPE_FADE::FADE_OUT);
		}
	}


}

void CWindow_EffectTool::Update_AppearArea_Particle()
{


	/* 파티클 개수 */
	if (nullptr != m_pCurParticle)
	{
		ImGui::Text("MaxInstance : %d", m_iMaxNumInstance);

		if (ImGui::DragInt("Instance Count", &m_iNumInstance, 1, 1, m_iMaxNumInstance))
			m_pCurParticle->Get_VIBufferCom()->Set_NumInstance(m_iNumInstance);

	}

	/* UV 회전 */
	ImGui::SeparatorText("");
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::DragFloat("Uv Degree", &m_fRotateUvDegree, 1.f, 0.f, 360.f))
			m_pCurParticle->Set_RotateUvDegree(m_fRotateUvDegree);
	}


	/* 추가 크기조절 */
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::DragFloat("AddScale_1", &m_fAddScale, 1.f, 0.f, 360.f))
		{
			m_pCurParticle->Get_VIBufferCom()->Set_AddScale(m_fAddScale, m_fAddScale);
		}

		if (ImGui::DragFloat2("AddScale_2", m_vAddScale, 1.f, 0.f, 360.f))
		{
			if (0 > m_vAddScale[0])
				m_vAddScale[0] = 0.f;

			if (0 > m_vAddScale[1])
				m_vAddScale[1] = 0.f;

			m_pCurParticle->Get_VIBufferCom()->Set_AddScale(m_vAddScale[0], m_vAddScale[1]);
		}
	}

	/* Colors */
	ImGui::ColorEdit3("Start_Color_Particle", m_fColor_Start_Particle, ImGuiColorEditFlags_None);
	if (nullptr != m_pCurParticle)
	{
		m_pCurParticle->Set_Color_Offset(m_fColor_Start_Particle[0], m_fColor_Start_Particle[1], m_fColor_Start_Particle[2], m_fColor_Start_Particle[3]);
	}

	ImGui::ColorEdit3("End_Color_Particle", m_fColor_End_Particle, ImGuiColorEditFlags_None);
	if (nullptr != m_pCurParticle)
	{                                                                                                                                                                                                   
		//m_pCurParticle->Set_Color_Offset(m_fColor_End_Particle[0], m_fColor_End_Particle[1], m_fColor_End_Particle[2], m_fColor_End_Particle[3]);
	}


}

void CWindow_EffectTool::Update_ActionArea_Particle()
{
	if (nullptr != m_pCurParticle)
	{
		CVIBuffer_Particle_Point* pVIBuffer = m_pCurParticle->Get_VIBufferCom();


		if (ImGui::DragFloat2("MinMaxLifeTime", m_vMinMaxLifeTime, 1.f, 0.f, 360.f))
		{
			if (m_vMinMaxLifeTime[0] > m_vMinMaxLifeTime[1])
				m_vMinMaxLifeTime[0] = m_vMinMaxLifeTime[1];

			m_pCurParticle->Get_VIBufferCom()->Set_LifeTime(m_vMinMaxLifeTime[0], m_vMinMaxLifeTime[1]);
		}


		if (ImGui::DragFloat2("MinMaxRange", m_vMinMaxRange, 1.f, 0.1f, 360.f))
		{
			if (m_vMinMaxRange[0] > m_vMinMaxRange[1])
				m_vMinMaxRange[0] = m_vMinMaxRange[1];

			pVIBuffer->Set_Range(m_vMinMaxRange[0], m_vMinMaxRange[1]);
		}


		/* 회전 범위(오프셋) */
		/* RotX */
		if (ImGui::DragFloat2("RotationX", m_vRotationOffsetX, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationOffsetX[0])
				m_vRotationOffsetX[0] = 0.f;

			if (m_vRotationOffsetX[0] > m_vRotationOffsetX[1])
				m_vRotationOffsetX[1] = m_vRotationOffsetX[0];

			pVIBuffer->Set_RotationOffset(MIN, AXIS_X, m_vRotationOffsetX[0]);
			pVIBuffer->Set_RotationOffset(MAX, AXIS_X, m_vRotationOffsetX[1]);
		}


		/* RotY */
		if (ImGui::DragFloat2("RotationY", m_vRotationOffsetY, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationOffsetY[0])
				m_vRotationOffsetY[0] = 0.f;

			if (m_vRotationOffsetY[0] > m_vRotationOffsetY[1])
				m_vRotationOffsetY[1] = m_vRotationOffsetY[0];

			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MIN, AXIS_Y, m_vRotationOffsetY[0]);
			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MAX, AXIS_Y, m_vRotationOffsetY[1]);
		}


		/* RotZ */
		if (ImGui::DragFloat2("RotationZ", m_vRotationOffsetZ, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationOffsetZ[0])
				m_vRotationOffsetZ[0] = 0.f;

			if (m_vRotationOffsetZ[0] > m_vRotationOffsetZ[1])
				m_vRotationOffsetZ[1] = m_vRotationOffsetZ[0];

			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MIN, AXIS_Z, m_vRotationOffsetZ[0]);
			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MAX, AXIS_Z, m_vRotationOffsetZ[1]);
		}
	}

	ImGui::SeparatorText("");
	/* 가속도 */
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::DragFloat("AccPos_Particle", &m_fParticleAccPosition, 0.1f, 0.f, 1.f))
			m_pCurParticle->Get_VIBufferCom()->Set_AccPosition(m_fParticleAccPosition);


		if (ImGui::DragFloat("ACC_Particle", &m_fParticleAcceleration, 0.5f, 0.f, 100.f))
			m_pCurParticle->Get_VIBufferCom()->Set_Acceleration(m_fParticleAcceleration);
	}

	/* 중력 사용 여부 */
	ImGui::SeparatorText("");
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::Button("Gravity"))
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Gravity(TRUE);
		}
		
		ImGui::SameLine();
		if (ImGui::Button("NoGravity"))
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Gravity(FALSE);
		}

		if (ImGui::DragFloat("UseGravityPosition", &m_fUseGravityPosition, 0.f, 0.f, 1.f))
			m_pCurParticle->Get_VIBufferCom()->Set_UseGravityPosition(m_fUseGravityPosition);


		if (ImGui::DragFloat("fGravityAcc", &m_fGravityAcc, -100.f, 0.f, 1.f))
			m_pCurParticle->Get_VIBufferCom()->Set_GravityAcc(m_fGravityAcc);

		ImGui::Text("TEST : %f, %f, %f", m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vTest.x, m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vTest.y, m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vTest.z );

	}



}

void CWindow_EffectTool::Update_WorldPositionArea_Particle()
{
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::DragFloat3("WorldPosition", m_vWorldPosition, 1.f, 0.f))
		{
			m_pCurParticle->Set_Position(_float3(m_vWorldPosition[0], m_vWorldPosition[1], m_vWorldPosition[2]));
		}
	}
}

void CWindow_EffectTool::Update_CurParameters(wstring strName)
{
	if (nullptr != m_pCurParticle)
	{
		if (TEXT("") != strName)
		{
			const string utf8Str = m_pGameInstance->Wstring_To_UTF8(strName);
			m_cCurParticleName = new char[utf8Str.length() + 1];
			strcpy(m_cCurParticleName, utf8Str.c_str());
		}
		CVIBuffer_Particle_Point* pVIBuffer = m_pCurParticle->Get_VIBufferCom();
		CVIBuffer_Particle_Point::PARTICLE_POINT_DESC* pDesc = pVIBuffer->Get_Desc();

		//m_iNumInstance = pVIBuffer->Get_NumInstance();

		m_vWorldPosition[0] = m_pCurParticle->Get_Transform()->Get_State(CTransform::STATE_POSITION).m128_f32[0];
		m_vWorldPosition[1] = m_pCurParticle->Get_Transform()->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		m_vWorldPosition[2] = m_pCurParticle->Get_Transform()->Get_State(CTransform::STATE_POSITION).m128_f32[2];

		m_iNumInstance = pDesc->iCurNumInstance;
		m_pCurParticle->Get_VIBufferCom()->Set_NumInstance(m_iNumInstance);

		m_vMinMaxLifeTime[0] = pDesc->vMinMaxLifeTime.x;
		m_vMinMaxLifeTime[1] = pDesc->vMinMaxLifeTime.y;

		m_vMinMaxRange[0] = pDesc->vMinMaxRange.x;
		m_vMinMaxRange[1] = pDesc->vMinMaxRange.y;

		m_vRotationOffsetX[0] = pDesc->vMinMaxRotationOffsetX.x;
		m_vRotationOffsetY[0] = pDesc->vMinMaxRotationOffsetY.x;
		m_vRotationOffsetZ[0] = pDesc->vMinMaxRotationOffsetZ.x;

		m_vRotationOffsetX[1] = pDesc->vMinMaxRotationOffsetX.y;
		m_vRotationOffsetY[1] = pDesc->vMinMaxRotationOffsetY.y;
		m_vRotationOffsetZ[1] = pDesc->vMinMaxRotationOffsetZ.y;

		m_fParticleAcceleration = pDesc->fAcceleration;
		m_fParticleAccPosition = pDesc->fAccPosition;


	}
}

void CWindow_EffectTool::Update_Demo_Texture()
{
	if (ImGui::Button("Create Demo Texture"))
	{
		Create_New_Texture(CEffect_Texture::TYPE::SINGLE);
	}

	ImGui::SameLine();
	if (ImGui::Button("Create Demo Sprite"))
	{
		Create_New_Texture(CEffect_Texture::TYPE::SPRITE);
	}
}

void CWindow_EffectTool::Update_Texture_Edit()
{
	if (nullptr != m_pCurEffectTexture)
	{
		CEffect_Texture::TYPE eType = m_pCurEffectTexture->Get_Desc()->eType;

		_int iMaxDiffuseCount = 0;
		if (CEffect_Texture::TYPE::SINGLE == eType)
			iMaxDiffuseCount = 3;
		else if (CEffect_Texture::TYPE::SPRITE == eType)
			iMaxDiffuseCount = 16;
		else
			iMaxDiffuseCount = 0;


		if (ImGui::InputInt("ShaderPass", &m_iShaderPassIndex_Tex, 1))
		{
			if (2 < m_iShaderPassIndex_Tex)
				m_iShaderPassIndex_Tex = 2;
			if (0 > m_iShaderPassIndex_Tex)
				m_iShaderPassIndex_Tex = 0;

			m_pCurEffectTexture->Set_ShaderPassIndex(m_iShaderPassIndex_Tex);
		}

		if (ImGui::InputFloat("DiscardValue", &m_fDiscardValue, 0.1f))
		{
			if (1.f < m_fDiscardValue)
				m_fDiscardValue = 1.f;
			if (0.f > m_fDiscardValue)
				m_fDiscardValue = 0.f;

			m_pCurEffectTexture->Set_DiscardValue(m_fDiscardValue);
		}


		if (ImGui::InputInt("Diffuse_Tex", &m_iTextureEffectIndex), 1)
		{
			if (iMaxDiffuseCount < m_iTextureEffectIndex)
				m_iTextureEffectIndex = iMaxDiffuseCount;

			if (0 > m_iTextureEffectIndex)
				m_iTextureEffectIndex = 0;

			m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, m_iTextureEffectIndex);

			_uint iX, iY;
			m_pCurEffectTexture->Get_TextureCom(CEffect::TEXTURE_DIFFUSE)->Get_TextureSize(&iX, &iY, m_iTextureEffectIndex);

			m_pCurEffectTexture->Set_SpriteSize(iX, iY);

			_float fSizeX, fSizeY;
			fSizeX = (_float)iX / m_iMaxVerHor[0];
			fSizeY = (_float)iY / m_iMaxVerHor[1];

			m_pCurEffectTexture->Set_AnimationSize(fSizeX, fSizeY);

			//m_pCurEffectTexture->Set_SpriteSize(1024.f, 1024.f);
			//m_pCurEffectTexture->Set_MaxVerHor(4, 4);
			//m_pCurEffectTexture->Set_AnimationSize(256.f, 256.f);
		}

		if (ImGui::InputInt("Mask_Tex", &m_iMaskTexIndex), 1)
		{
			if (17 < m_iMaskTexIndex)
				m_iMaskTexIndex = 17;

			if (0 > m_iMaskTexIndex)
				m_iMaskTexIndex = 0;

			m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE_MASK, m_iMaskTexIndex);
		}

		if (ImGui::InputInt("Noise_Tex", &m_iNoiseTexIndex), 1)
		{
			if (5 < m_iNoiseTexIndex)
				m_iNoiseTexIndex = 5;

			if (0 > m_iNoiseTexIndex)
				m_iNoiseTexIndex = 0;

			m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE_NOISE, m_iNoiseTexIndex);
		}

		if (CEffect_Texture::TYPE::SINGLE == eType)
		{
			if (ImGui::CollapsingHeader(" Distortion Preset ", ImGuiTabBarFlags_None))
			{
				if (ImGui::Button(" Fire "))
				{
					m_pCurEffectTexture->Set_ShaderPassIndex(2);
					m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE::TEXTURE_DIFFUSE, 1);
					m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE::TEXTURE_MASK, 17);
					m_pCurEffectTexture->Set_TextureIndex(CEffect::TEXTURE::TEXTURE_NOISE, 1);

					m_pCurEffectTexture->Set_NoiseTimeInterval(1.f);

					m_pCurEffectTexture->Set_ScrollSpeeds(1.f, 1.f, 1.f);
					m_pCurEffectTexture->Set_Scales(1.f, 1.f, 1.f);

					m_pCurEffectTexture->Set_Distortion1(0.1f, 0.1f);
					m_pCurEffectTexture->Set_Distortion2(0.f, 0.f);
					m_pCurEffectTexture->Set_Distortion3(0.f, 0.1f);

					m_pCurEffectTexture->Set_DistortionScale(1.f);
					m_pCurEffectTexture->Set_DistortionBias(1.f);

					Update_CurParameters_Tex();
				}

			}

			ImGui::SeparatorText("");
			if (ImGui::DragFloat("Noise_TimeInterval", &m_fNoiseIntervalTime, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_NoiseTimeInterval(m_fNoiseIntervalTime);
			}

			if (ImGui::DragFloat3("ScrollSpeeds", m_vScrollSpeeds, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_ScrollSpeeds(m_vScrollSpeeds[0], m_vScrollSpeeds[1], m_vScrollSpeeds[2]);
			}

			if (ImGui::DragFloat3("Noise_Scales", m_vScales, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_Scales(m_vScales[0], m_vScales[1], m_vScales[2]);
			}

			if (ImGui::DragFloat2("Distortion1", m_vDistortion1, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_Distortion1(m_vDistortion1[0], m_vDistortion1[1]);
			}

			if (ImGui::DragFloat2("Distortion2", m_vDistortion2, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_Distortion2(m_vDistortion2[0], m_vDistortion2[1]);
			}

			if (ImGui::DragFloat2("Distortion3", m_vDistortion3, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_Distortion3(m_vDistortion3[0], m_vDistortion3[1]);
			}

			if (ImGui::DragFloat("DistortionScale__", &m_fDistortionScale, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_DistortionScale(m_fDistortionScale);
			}

			if (ImGui::DragFloat("DistortionBias", &m_fDistortionBias, 1.f, 0.f))
			{
				m_pCurEffectTexture->Set_DistortionBias(m_fDistortionBias);
			}

		}

		if (CEffect_Texture::SPRITE == eType)
		{
			ImGui::SeparatorText("");
			if (ImGui::DragFloat("IntervalTime", &m_fIntervalTime, 1, 1))
			{
				m_pCurEffectTexture->Set_IntervalTime(m_fIntervalTime);
			}

			//if (ImGui::DragInt2("MaxVerHor", m_iMaxVerHor, 1, 1))
			//{
			//	m_pCurEffectTexture->Set_MaxVerHor(m_iMaxVerHor[0], m_iMaxVerHor[1]);
			//}
			if (ImGui::InputInt2("MaxVerHor", m_iMaxVerHor, 1))
			{
				_uint iX, iY;
				m_pCurEffectTexture->Get_TextureCom(CEffect::TEXTURE_DIFFUSE)->Get_TextureSize(&iX, &iY, m_iTextureEffectIndex);
				m_pCurEffectTexture->Set_SpriteSize(iX, iY);

				_float fSizeX, fSizeY;
				fSizeX = (_float)iX / m_iMaxVerHor[0];
				fSizeY = (_float)iY / m_iMaxVerHor[1];

				m_pCurEffectTexture->Set_AnimationSize(fSizeX, fSizeY);

				m_pCurEffectTexture->Set_MaxVerHor(m_iMaxVerHor[0], m_iMaxVerHor[0]);


				m_pCurEffectTexture->ReSet_Sprite();
			}


			if (ImGui::Button("Play_Tex"))
			{
				m_pCurEffectTexture->Set_Play(TRUE);
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop_Tex"))
			{
				m_pCurEffectTexture->Set_Play(FALSE);
			}

			ImGui::Text("Current Index : %d, %d", m_pCurEffectTexture->Get_Sprite_Desc()->iCurrentHor, m_pCurEffectTexture->Get_Sprite_Desc()->iCurrentVer);
		}


		ImGui::SeparatorText("");
		if (ImGui::DragFloat3("WorldPosition_Tex", m_vWorldPosition_Tex, 1.f, 0.f))
		{
			m_pCurEffectTexture->Set_Position(_float3(m_vWorldPosition_Tex[0], m_vWorldPosition_Tex[1], m_vWorldPosition_Tex[2]));
		}

		if (ImGui::DragFloat3("Scale_Tex", m_vScale_Tex, 1.f, 0.f))
		{
			m_pCurEffectTexture->Get_Transform()->Set_Scaling(m_vScale_Tex[0], m_vScale_Tex[1], m_vScale_Tex[2]);
		}

	}

}

void CWindow_EffectTool::Update_CurParameters_Tex()
{
	if (nullptr != m_pCurEffectTexture)
	{
		//if (TEXT("") != strName)
		//{
		//	const string utf8Str = m_pGameInstance->Wstring_To_UTF8(strName);
		//	m_cCurEffectTextureName = new char[utf8Str.length() + 1];
		//	strcpy(m_cCurEffectTextureName, utf8Str.c_str());
		//}

		CEffect_Texture::EFFECT_TEXTURE_DESC* pDesc = m_pCurEffectTexture->Get_Desc();
		CEffect_Texture::SPRITEUV_DESC* pSpriteDesc = m_pCurEffectTexture->Get_Sprite_Desc();
		CEffect_Texture::NOISE_DESC* pDistortionDesc = m_pCurEffectTexture->Get_Noise_Desc();

		_vector vPos = m_pCurEffectTexture->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		m_vWorldPosition_Tex[0] = vPos.m128_f32[0];
		m_vWorldPosition_Tex[1] = vPos.m128_f32[1];
		m_vWorldPosition_Tex[2] = vPos.m128_f32[2];

		m_vScale_Tex[0] = m_pCurEffectTexture->Get_Transform()->Get_Scaled().x;
		m_vScale_Tex[1] = m_pCurEffectTexture->Get_Transform()->Get_Scaled().y;
		m_vScale_Tex[2] = m_pCurEffectTexture->Get_Transform()->Get_Scaled().z;

		m_iShaderPassIndex_Tex = pDesc->iShaderPassIndex;

		m_iTextureEffectIndex = pDesc->iTextureIndex[CEffect::TEXTURE::TEXTURE_DIFFUSE];
		m_iMaskTexIndex = pDesc->iTextureIndex[CEffect::TEXTURE::TEXTURE_MASK];
		m_iNoiseTexIndex = pDesc->iTextureIndex[CEffect::TEXTURE::TEXTURE_NOISE];

		/* Sprite */
		m_iMaxVerHor[0] = pSpriteDesc->iMaxHor;
		m_iMaxVerHor[1] = pSpriteDesc->iMaxVer;

		m_fIntervalTime = pSpriteDesc->fIntervalTime;

		/* Distortion */
		m_vScrollSpeeds[0] = pDistortionDesc->vScrollSpeeds.x;
		m_vScrollSpeeds[1] = pDistortionDesc->vScrollSpeeds.y;
		m_vScrollSpeeds[2] = pDistortionDesc->vScrollSpeeds.z;

		m_vScales[0] = pDistortionDesc->vScales.x;
		m_vScales[1] = pDistortionDesc->vScales.y;
		m_vScales[2] = pDistortionDesc->vScales.z;

		m_vDistortion1[0] = pDistortionDesc->vDistortion1.x;
		m_vDistortion1[1] = pDistortionDesc->vDistortion1.y;

		m_vDistortion2[0] = pDistortionDesc->vDistortion2.x;
		m_vDistortion2[1] = pDistortionDesc->vDistortion2.y;

		m_vDistortion3[0] = pDistortionDesc->vDistortion3.x;
		m_vDistortion3[1] = pDistortionDesc->vDistortion3.y;

		m_fDistortionScale = pDistortionDesc->fDistortionScale;
		m_fDistortionBias = pDistortionDesc->fDistortionBias;

	}

}

void CWindow_EffectTool::Update_Demo_Effect_Instance()
{
	if (ImGui::Button("Create Demo Mesh"))
	{
		// Prototype_Component_Model_Mesh_Fire_Test
		//Create_New_EffectInstance(TEXT("Prototype_Component_Model_Xray_ManHeavy"));
		// Prototype_Component_Model_ShieldDome
		// Prototype_Component_Model_Mesh_Fire_Test
		Create_New_EffectInstance(TEXT("Prototype_Component_Model_ShieldDome"));
	}



	if (ImGui::InputInt("ShaderPassIndex", &m_iShaderPassIndex, 1))
	{
		if (nullptr != m_pCurEffectInstance)
		{
			if (7 < m_iShaderPassIndex)
				m_iShaderPassIndex = 7;

			m_pCurEffectInstance->Set_ShaderPassIndex(m_iShaderPassIndex);
		}
	}

	if (ImGui::Button("TRUE"))
	{
		if (nullptr != m_pCurEffectInstance)
		{
			m_pCurEffectInstance->bTest = TRUE;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("FALSE"))
	{
		if (nullptr != m_pCurEffectInstance)
		{
			m_pCurEffectInstance->bTest = FALSE;
		}
	}

}

void CWindow_EffectTool::Update_Trnasform_Effect_Instance()
{
	if (nullptr != m_pCurEffectInstance)
	{
		//if (ImGui::DragFloat3("WorldPosition_Mesh", m_vWorldPosition_EffectInstance, 1.f, 0.f))
		//{
		//	m_pCurEffectInstance->Set_Position(_float3(m_vWorldPosition_EffectInstance[0], m_vWorldPosition_EffectInstance[1], m_vWorldPosition_EffectInstance[2]));
		//}

		//if (ImGui::DragFloat("Scale_Mesh", &m_vScale_EffectInstance, 1.f, 0.f))
		//{
		//	m_pCurEffectInstance->Get_Transform()->Set_Scaling(m_vScale_EffectInstance, m_vScale_EffectInstance, m_vScale_EffectInstance);
		//}
	}

}

void CWindow_EffectTool::Update_SaveLoad_Particle()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save"))
			{
				Save_Function("../Bin/DataFiles/Data_Effect/Particle_Info/", "Particle_TestSphere_Info");
			}
			if (ImGui::MenuItem("Load"))
			{
				//m_eDialogType = DIALOG_TYPE::LOAD_DIALOG;
				//m_strDialogPath = "../Bin/DataFiles/Data_Effect/Particle_Info/";

				OpenDialog(IMGUI_EFFECTTOOL_WINDOW);

				Load_Function("../Bin/DataFiles/Data_Effect/Particle_Info/", "Particle_TestSphere_Info");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

HRESULT CWindow_EffectTool::Save_Function(string strPath, string strFileName)
{
	if (nullptr != m_pCurParticle)
	{
		char filePath[MAX_PATH] = "../Bin/DataFiles/Data_Effect/Particle_Info/Particle_TestSphere_Info";

		json Out_Json;
		m_pCurParticle->Write_Json(Out_Json);

		CJson_Utility::Save_Json(filePath, Out_Json);
	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Load_Function(string strPath, string strFileName)
{

	if (nullptr == m_pCurParticle)
	{
		Create_New_Particle();
	}

	json In_Json;
	char filePath[MAX_PATH] = "../Bin/DataFiles/Data_Effect/Particle_Info/Particle_TestSphere_Info";
	CJson_Utility::Load_Json(filePath, In_Json);

	m_pCurParticle->Load_FromJson(In_Json);

	Update_CurParameters();

	return S_OK;
}

HRESULT CWindow_EffectTool::Create_New_Particle()
{
	if (FAILED(Ready_Layer_Particle(TEXT("Layer_Particle"))))
		return E_FAIL;

	m_iCurParticleIndex = (_int)m_pParticles.size();

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
		const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
		m_szParticleNames[iCount] = new char[utf8Str.length() + 1];
		strcpy(m_szParticleNames[iCount], utf8Str.c_str());

		iCount++;
	}

	m_iCurParticleIndex -= 1;

	return S_OK;
}

HRESULT CWindow_EffectTool::Ready_Layer_Particle(const wstring& strLayerTag)
{
	CEffect_Particle::EFFECT_PARTICLE_DESC	tDesc = {};
	tDesc.fSpeedPerSec = { 5.f };
	tDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

	tDesc.eType = CEffect_Particle::SINGLE;
	tDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Particle_Base");
	//tDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
	tDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

	//tDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
	tDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("");
	tDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 /*1*/};

	tDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
	tDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

	tDesc.strShaderTag = TEXT("Prototype_Component_Shader_Particle_Point");
	tDesc.iShaderPassIndex = { 0 };
	tDesc.iRenderGroup = { 7 };

	tDesc.iNumInstance = { (_uint)m_iNumInstance };
	tDesc.iMaxNumInstance = { (_uint)m_iMaxNumInstance };

	tDesc.fRotateUvDegree = { 0.f };

	CEffect_Particle* pParticle = dynamic_cast<CEffect_Particle*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Effect_Particle"), &tDesc));

	wstring strName = Make_NameWithPin(TEXT("Particle"));
	m_pParticles.emplace(strName, pParticle);

	m_pCurParticle = pParticle;

	Update_CurParameters(strName);

	return S_OK;
}

HRESULT CWindow_EffectTool::Create_New_Texture(CEffect_Texture::TYPE eType)
{
	if (FAILED(Ready_Layer_Texture(TEXT("Layer_Texture_Effect"), eType)))
		return E_FAIL;

	m_iCurEffectTextureIndex = (_int)m_pEffectTextures.size();

	/* 문자열 초기화 */
	if (nullptr != m_szEffectTextureNames)
	{
		for (_int i = 0; i < m_iCurEffectTextureIndex; ++i)
		{
			m_szEffectTextureNames[i] = nullptr;
		}
		m_szEffectTextureNames = nullptr;
	}

	m_szEffectTextureNames = new char* [m_iCurEffectTextureIndex];

	_int iCount = 0;
	for (auto& Pair : m_pEffectTextures)
	{
		const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
		m_szEffectTextureNames[iCount] = new char[utf8Str.length() + 1];
		strcpy(m_szEffectTextureNames[iCount], utf8Str.c_str());

		iCount++;
	}

	m_iCurEffectTextureIndex -= 1;

	return S_OK;

}

HRESULT CWindow_EffectTool::Ready_Layer_Texture(const wstring& strLayerTag, CEffect_Texture::TYPE eType)
{

	CEffect_Texture::EFFECT_TEXTURE_DESC tDesc = {};

	tDesc.eType = eType;

	//tDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
	//tDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

	tDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
	tDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 };

	tDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
	tDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

	tDesc.strShaderTag = TEXT("Prototype_Component_Shader_EffectTex");

	if (CEffect_Texture::TYPE::SINGLE)
	{
		tDesc.iShaderPassIndex = { 2 };
	}
	else
	{
		tDesc.iShaderPassIndex = { 1 };
	}

	tDesc.iRenderGroup = { 7 };

	CEffect_Texture* pEffect = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &tDesc));


	wstring strName = TEXT("");
	wstring strFrontName = TEXT("Texture_");

	_int iMaxNum = -1;
	wstring strPin = TEXT("");

	for (auto& iter : m_pEffectTextures)
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

	m_pEffectTextures.emplace(strName, pEffect);

	m_pCurEffectTexture = pEffect;


	Update_CurParameters_Tex();


	return S_OK;
}

HRESULT CWindow_EffectTool::Create_New_EffectInstance(wstring strModelTag)
{
	if (FAILED(Ready_Layer_EffectInstance(TEXT("Layer_EffectInstance"), strModelTag)))
		return E_FAIL;

	m_iCurEffectInstanceIndex = (_int)m_pEffectInstances.size();

	/* 문자열 초기화 */
	if (nullptr != m_szEffectInstanceNames)
	{
		for (_int i = 0; i < m_iCurEffectInstanceIndex; ++i)
		{
			m_szEffectInstanceNames[i] = nullptr;
		}
		m_szEffectInstanceNames = nullptr;
	}

	m_szEffectInstanceNames = new char* [m_iCurParticleIndex];

	_int iCount = 0;
	for (auto& Pair : m_pParticles)
	{
		const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
		m_szEffectInstanceNames[iCount] = new char[utf8Str.length() + 1];
		strcpy(m_szEffectInstanceNames[iCount], utf8Str.c_str());

		iCount++;
	}

	m_iCurEffectInstanceIndex -= 1;

	return S_OK;
}

HRESULT CWindow_EffectTool::Ready_Layer_EffectInstance(const wstring& strLayerTag, wstring strModelTag)
{
	CEffect_Instance::EFFECT_INSTANCE_DESC tDesc = {};
	tDesc.fSpeedPerSec = { 5.f };
	tDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

	tDesc.eType = { CEffect_Instance::FLAT };

	tDesc.strModelTag = strModelTag;
	tDesc.iNumInstance = { 2 };

	
	tDesc.strTextureTag[CEffect_Instance::TYPE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
	//tDesc.strTextureTag[CEffect_Instance::TYPE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
	tDesc.iTextureIndex[CEffect_Instance::TYPE_DIFFUSE] = { 0 };

	//Shader_AnimModel
	tDesc.strShaderTag = TEXT("Prototype_Component_Shader_Effect_Model_Instance");
	//Prototype_Component_Shader_Effect_Model_Instance
	//tDesc.strShaderTag = TEXT("Prototype_Component_Shader_AnimModel");
	tDesc.iShaderPassIndex = { 0 };
	tDesc.iRenderGroup = { 6 };


	CEffect_Instance* pMesh = dynamic_cast<CEffect_Instance*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strModelTag, TEXT("Prototype_GameObject_Effect_Instance"), &tDesc));

	wstring strName = TEXT("");
	wstring strFrontName = TEXT("EffectInstance");

	_int iMaxNum = -1;
	wstring strPin = TEXT("");

	for (auto& iter : m_pEffectInstances)
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


	m_pEffectInstances.emplace(strName, pMesh);

	m_pCurEffectInstance = pMesh;

	return S_OK;
}

wstring CWindow_EffectTool::Make_NameWithPin(const wstring& strFrontName)
{
	_int iMaxNum = -1;
	wstring strPin = TEXT("");

	for (auto& iter : m_pParticles)
	{
		if (nullptr == iter.second)
			continue;

		// Particle, Particle_000
		if (strFrontName == m_pGameInstance->Remove_LastNumChar(iter.first, 4))
		{
			_int iPinNum = stoi(m_pGameInstance->Get_LastNumChar(iter.first, 3));

			if (iMaxNum < iPinNum)
				iMaxNum = iPinNum;
		}
	}

	/* 최댓값이 -1이라는 것은 해당 이름과 같은게 없으므로 고유번호를 000으로 세팅한다. */
	if (-1 == iMaxNum)
		return strFrontName + L"_000";
	else /* 아니라면 최댓값에 + 1을 하여 고유 번호로 세팅한다. */
	{
		_int iPinNum = iMaxNum + 1;

		if (0 == iPinNum / 10)
			strPin = L"_00" + to_wstring(iPinNum);
		else if (0 == iPinNum / 100)
			strPin = L"_0" + to_wstring(iPinNum);
		else
			strPin = L"_" + to_wstring(iPinNum);

		return strFrontName + strPin;
	}
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

	for (auto& Pair : m_pParticles)
		Safe_Release(Pair.second);
	m_pParticles.clear();

	if (nullptr != m_pCurParticle)
		Safe_Release(m_pCurParticle);


	for (auto& Pair : m_pEffectTextures)
		Safe_Release(Pair.second);
	m_pEffectTextures.clear();

	if (nullptr != m_pCurEffectTexture)
		Safe_Release(m_pCurEffectTexture);


	for (auto& Pair : m_pEffectInstances)
		Safe_Release(Pair.second);
	m_pEffectInstances.clear();

	if (nullptr != m_pCurEffectInstance)
		Safe_Release(m_pCurEffectInstance);


	m_cCurParticleName = nullptr;
	m_cCurEffectTextureName = nullptr;
	m_cCurEffectInstanceName = nullptr;

	if (nullptr != m_szParticleNames)
	{
		for (_int i = 0; i < m_iCurParticleIndex; ++i)
		{
			m_szParticleNames[i] = nullptr;
		}
		m_szParticleNames = nullptr;
	}

	if (nullptr != m_szEffectTextureNames)
	{
		for (_int i = 0; i < m_iCurEffectTextureIndex; ++i)
		{
			m_szEffectTextureNames[i] = nullptr;
		}
		m_szEffectTextureNames = nullptr;
	}

	if (nullptr != m_szEffectInstanceNames)
	{
		for (_int i = 0; i < m_iCurEffectInstanceIndex; ++i)
		{
			m_szEffectInstanceNames[i] = nullptr;
		}
		m_szEffectInstanceNames = nullptr;
	}

	Safe_Release(m_pGameInstance);
}
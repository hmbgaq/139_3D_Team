#include "stdafx.h"
#include "Window_EffectTool.h"

/* ImGui 관련 */
#include "imNeoSequencer/imgui_neo_sequencer.h"

/* Util */
#include "GameInstance.h"
#include "Easing_Utillity.h"
#include "Effect_Manager.h"
#include "Data_Manager.h"

/* Effect */
#include "Effect.h"
#include "Effect_Rect.h"
#include "Effect_Particle.h"
#include "Effect_Instance.h"
#include "Effect_Trail.h"
//#include "Mesh.h"

/* Level */
#include "MasterCamera.h"
#include "Sky.h"
#include "Grid.h"
#include "Model_Preview.h"
#include "Part_Preview.h"

#include "Player.h"

CWindow_EffectTool::CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{

}


HRESULT CWindow_EffectTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	Load_CustomStyle();			// 스타일 저장 정보 로드
	

	ReSet_CameraPos();			// 카메라 얻어오기, 위치 리셋
	FAILED_CHECK(Ready_Sky());	// 스카이박스 얻어오기

	return S_OK;
}

void CWindow_EffectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ShowDialog();

#pragma region 환경(레벨) 세팅 창 (스카이박스, 크기비교용 모델 등)
	SetUp_ImGuiDESC(u8"환경 세팅", ImVec2{ 400.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
	
	__super::Begin();

	Update_LevelSetting_Window();	// 레벨(환경) 세팅 창(카메라, 스카이박스, 크기비교용 모델 등...) 업데이트

	// ImGui창 사이즈
	Show_ImGui_WindowSize();

	__super::End();
#pragma endregion


#pragma region 오브젝트 리스트 창
	SetUp_ImGuiDESC(u8"오브젝트 리스트", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
	
	__super::Begin();

	Update_SaveLoad_Menu();		// 저장 불러오기 메뉴 업데이트
	Update_EffectList_Window();	// 이펙트 리스트박스 창 업데이트

	// ImGui창 사이즈
	Show_ImGui_WindowSize();


	__super::End();	
#pragma endregion


#pragma region 네오 시퀀서 창
	SetUp_ImGuiDESC(u8"네오 시퀀서", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
	
	__super::Begin();

	Update_NeoSequencer_Window();	// 시퀀서 창 업데이트

	// ImGui창 사이즈
	Show_ImGui_WindowSize();

	__super::End();
#pragma endregion



#pragma region 타임라인 창
	SetUp_ImGuiDESC(u8"타임라인", ImVec2{ 400.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
	__super::Begin();

	Update_Timeline_Window();	// 타임라인 창 (재생, 멈춤, 리셋) 업데이트

	// ImGui창 사이즈
	Show_ImGui_WindowSize();

	__super::End();
#pragma endregion



#pragma region 이미지 리스트 창
//	SetUp_ImGuiDESC(u8"이미지 리스트", ImVec2{ 400.f, 300.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
//	__super::Begin();
//
//	Update_ImageList_Window();	// 텍스처 이미지 미리보기, 리스트 업데이트
//
//	// ImGui창 사이즈
//	Show_ImGui_WindowSize();
//
//	__super::End();
#pragma endregion


#pragma region 이펙트 툴
	SetUp_ImGuiDESC(u8"이펙트 툴", ImVec2{ 300.f, 700.f }, ImGuiWindowFlags_NoDocking /*| ImGuiWindowFlags_NoCollapse */ /* | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove */ /* | ImGuiWindowFlags_NoBringToFrontOnFocus*/, ImVec4(0.f, 0.f, 0.f, 0.2f));
	
	__super::Begin();

	if (ImGui::BeginTabBar("Tab_Effect", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(" Particle "))
		{
			Update_ParticleTab();	// 파티클 탭 업데이트
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Rect "))
		{
			Update_RectTab();		// 렉트 탭 업데이트
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Mesh "))
		{
			Update_MeshTab();		// 메쉬 탭 업데이트
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Trail "))
		{
			Update_TrailTab();		// 트레일 탭 업데이트
			ImGui::EndTabItem();
		}

	}
	ImGui::EndTabBar();


	// ImGui창 사이즈
	Show_ImGui_WindowSize();

	__super::End();
#pragma endregion



}

void CWindow_EffectTool::Render()
{

}

void CWindow_EffectTool::Show_ImGui_WindowSize()
{
	ImGui::SeparatorText("");
	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	ImGui::Spacing();
}

void CWindow_EffectTool::Show_MousePos()
{
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ImGui::Text("Mouse Pos : %d, %d", pt.x, pt.y);
}

void CWindow_EffectTool::Show_CameraInfo()
{
	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	_float4 vCamDir = m_pGameInstance->Get_CamDirection();

	ImGui::Text("Cam Pos  : %.2f %.2f %.2f", vCamPos.x, vCamPos.y, vCamPos.z);
	ImGui::Text("Cam Dir : %.2f %.2f %.2f", vCamDir.x, vCamDir.y, vCamDir.z);
}


void CWindow_EffectTool::ReSet_CameraPos()
{
	if (nullptr == m_pCamera)
	{
		// 카메라 받아오기
		m_pCamera = CData_Manager::GetInstance()->Get_MasterCamera()->Get_vectorCamera()[CMasterCamera::DynamicCamera];
	}
	else
	{
		m_pCamera->Set_Position(m_Camera_ResetPos);	// 카메라 위치 리셋
	}

}

HRESULT CWindow_EffectTool::Ready_Grid()
{
	CGrid::GRID_DESC	tDesc = {};

	tDesc.vGridColor = { 0.5f, 0.5f, 0.5f, 1.f };	// 그리드 색 설정

	//tDesc.strTextureTag[CGrid::TEXTURE_DIFFUSE] = { TEXT("Prototype_Component_Texture_Effect_Diffuse") };
	tDesc.strTextureTag[CGrid::TEXTURE_DIFFUSE] = { TEXT("") };
	tDesc.iTextureIndex[CGrid::TEXTURE_DIFFUSE] = { 0 };

	tDesc.strTextureTag[CGrid::TEXTURE_MASK] = { TEXT("") };
	tDesc.iTextureIndex[CGrid::TEXTURE_MASK] = { 0 };


	CGameObject* pObj = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_Grid"), TEXT("Prototype_GameObject_Grid"), &tDesc);
	if (nullptr != pObj)
	{
		m_pGrid = dynamic_cast<CGrid*>(pObj);
	}
		
	return S_OK;
}


HRESULT CWindow_EffectTool::Ready_Sky()
{
	// 스카이박스 얻어오기
	m_pSky = CData_Manager::GetInstance()->Get_pSkyBox();

	if (m_pSky == nullptr)
		return E_FAIL;

	m_pSky->Set_SkyType(CSky::SKYTYPE::SKY_TEMP1);

	return S_OK;
}

void CWindow_EffectTool::Set_SkyTexture()
{
	if(nullptr == m_pSky)
		return;

	m_pSky->Set_SkyType((CSky::SKYTYPE)m_iSkyTextureIndex);

}

HRESULT CWindow_EffectTool::Ready_Model_Preview(wstring strModelTag)
{
	CModel_Preview::MODEL_PREVIEW_DESC	tDesc = {};
	tDesc.strProtoTag = { TEXT("Prototype_GameObject_Model_Preview") };
	tDesc.strModelTag = { strModelTag };

	if (strModelTag == TEXT("Prototype_Component_Model_Rentier"))
	{
		tDesc.iAnimIndex = { 41 };	// 플레이어 Idle애니메이션은 8번
	}

	if (strModelTag == TEXT("Prototype_Component_Model_VampireCommander"))
	{
		tDesc.iAnimIndex = { 9 };	// 보스 Idle애니메이션은 9번 VampireCommander_Idle
	}




	CGameObject* pObj = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_Model_Preview"), TEXT("Prototype_GameObject_Model_Preview"), &tDesc);
	if (nullptr != pObj)
		m_pModel_Preview = dynamic_cast<CModel_Preview*>(pObj);


	if (CModel::TYPE_ANIM == m_pModel_Preview->Get_Desc()->eType)
	{
		m_pPart_Preview = dynamic_cast<CPart_Preview*>(dynamic_cast<CModel_Preview*>(m_pModel_Preview)->Find_PartObject(TEXT("Part_Preview")));
	}

	return S_OK;
}


void CWindow_EffectTool::Update_Timeline_Window()
{
	/* 타임라인 */
	if (nullptr != m_pCurEffect)
	{
		auto& style = ImGui::GetStyle();

		_float fSequenceTimePos = m_pCurEffectDesc->fSequenceAcc;
		//_float fSequenceTimePos = m_pCurEffect->Get_EasingTimeAcc();
		ImGui::SliderFloat("SequenceTimeAcc", &fSequenceTimePos, 0.f, m_pCurEffectDesc->fSequenceTime);

		m_vTimeAccs_Effect[0] = (m_pCurEffectDesc->fWaitingTime != 0.0f) ? min(1.0f, m_pCurEffectDesc->fWaitingAcc / m_pCurEffectDesc->fWaitingTime) : 1.0f;
		m_vTimeAccs_Effect[1] = m_pCurEffectDesc->fLifeTimeRatio;
		m_vTimeAccs_Effect[2] = (m_pCurEffectDesc->fRemainTime != 0.0f) ? min(1.0f, m_pCurEffectDesc->fRemainAcc / m_pCurEffectDesc->fRemainTime) : 1.0f;
		ImGui::SliderFloat3("TimeAccs_Effect", m_vTimeAccs_Effect, 0.f, 1.f);

		if (nullptr != m_pCurPartEffect)
		{
			if (ImGui::CollapsingHeader("TimeAccs_Part"))
			{			
				ImGui::SliderFloat("SequenceTimeAcc_Part", &m_pCurVoidDesc->fSequenceAcc, 0.f, m_pCurEffectDesc->fSequenceTime);

				m_vTimeAccs_Part[0] = (m_pCurVoidDesc->fWaitingTime != 0.0f) ? min(1.0f, m_pCurVoidDesc->fWaitingAcc / m_pCurVoidDesc->fWaitingTime) : 1.0f;
				m_vTimeAccs_Part[1] = m_pCurVoidDesc->fLifeTimeRatio;
				m_vTimeAccs_Part[2] = (m_pCurVoidDesc->fRemainTime != 0.0f) ? min(1.0f, m_pCurVoidDesc->fRemainAcc / m_pCurVoidDesc->fRemainTime) : 1.0f;
				ImGui::SliderFloat3("TimeAccs_Part", m_vTimeAccs_Part, 0.f, 1.f);
			}
		}

		/* 재생, 정지, 리셋 */
		if (ImGui::Button("   Play   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffectDesc->bPlay = TRUE;
			if (nullptr != m_pModel_Preview)
			{
				dynamic_cast<CModel_Preview*>(m_pModel_Preview)->Get_Desc()->bPlay = TRUE;
			}
		}
		//ImGui::SameLine();
		if (ImGui::Button("   Stop   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffectDesc->bPlay = FALSE;
			if (nullptr != m_pModel_Preview)
			{
				dynamic_cast<CModel_Preview*>(m_pModel_Preview)->Get_Desc()->bPlay = FALSE;
			}

		}
		//ImGui::SameLine();
		if (ImGui::Button("   Reset   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffect->Init_ReSet_Effect();
		}

	}
}


void CWindow_EffectTool::Update_ParticleTab()
{
	if (nullptr != m_pCurEffect)
	{
		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurVoidDesc->eType_Effect;

			if (CEffect_Void::PARTICLE == eType_Effect)
			{
#pragma region Desc 얻어오기 업데이트_파티클
				m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();
				m_pSpriteDesc_Particle = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Sprite_Desc();
				CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
				m_pParticleBufferDesc = pVIBuffer->Get_Desc();
#pragma endregion 

				/* 이름_파티클 */
				ImGui::SeparatorText("Name");
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pCurVoidDesc->strPartTag));


#pragma region 텍스처 설정_파티클
				/* 텍스처 변경_파티클 */
				if (ImGui::CollapsingHeader(" Texture_Particle "))
				{

					if (m_pCurVoidDesc->bUseSpriteAnim)
					{
						// 디퓨즈_스프라이트_파티클 텍스처 ============================================================================================
						ImGui::SeparatorText("Diffuse_Sprte_Particle");

						if (ImGui::Button("Sprite_Base"))	// 베이스 스프라이트로 변경
						{
							dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Sprite"));
							m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 21;
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 0;
						}
						ImGui::SameLine();
						if (ImGui::Button("Sprite_Smokes"))	// 스모크 스프라이트로 변경
						{
							dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Sprite_Smokes"));
							m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 34;
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 0;
						}

						if (ImGui::InputInt("Sprite_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE], 1))
						{
							if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE])
								m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE];

							if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE])
								m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 0;

							m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE];
						}


					}
					else
					{
						// 디퓨즈_파티클 텍스처 ===============================================================================================
						ImGui::SeparatorText("Diffuse_Particle");

						if (ImGui::Button("Diffuse_Base"))	// 베이스 디퓨즈로 변경
						{
							dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Diffuse"));
							m_pCurVoidDesc->strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
							m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = 11;
							m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = 0;	// 텍스처 인덱스 초기화
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = 0;
						}


						if (ImGui::InputInt("Diffuse_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE], 1))
						{
							if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE])
								m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE];

							if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE])
								m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = 0;

							m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE];
						}
					}


					// 마스크_파티클 텍스처 =====================================================================================================
					ImGui::SeparatorText("Mask_Particle");

					if (ImGui::Button("Mask_Base"))	// 베이스 마스크로 변경
					{
						dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask"));
						m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 164;
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 0;

					}ImGui::SameLine();
					if (ImGui::Button("Mask_Waves"))	// 웨이브 마스크로 변경
					{
						dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask_Waves"));
						m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 6;
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 0;
					}

					if (ImGui::InputInt("Mask_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK], 1))
					{
						if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK] -1;

						if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK];
					}


					// 노이즈_파티클 텍스처 =====================================================================================================
					ImGui::SeparatorText("Noise_Particle");
					if (ImGui::Button("Noise_Base"))	// 베이스 노이즈로 변경
					{
						dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Noise"));
						m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = 243;
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = 0;
					}


					if (ImGui::InputInt("Noise_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE], 1))
					{
						if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] - 1;

						if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE];
					}
				}
#pragma endregion


				/* 빌보드 키고 끄기 */
				ImGui::SeparatorText("BillBoard");
				ImGui::RadioButton("BillBoard", &m_iBillBoard, 0);
				ImGui::RadioButton("Off BillBoard", &m_iBillBoard, 1);
				if (0 == m_iBillBoard)
					m_pCurVoidDesc->bBillBoard = TRUE;
				else if (1 == m_iBillBoard)
					m_pCurVoidDesc->bBillBoard = FALSE;


#pragma region 스프라이트 설정_파티클
				/* 스프라이트 키고 끄기 */
				ImGui::SeparatorText("Sprite");
				ImGui::RadioButton("Off Sprite_Particle", &m_iSprite_Particle, 0);
				ImGui::RadioButton("Sprite_Particle", &m_iSprite_Particle, 1);
				if (0 == m_iSprite_Particle)
				{
					m_pCurVoidDesc->bUseSpriteAnim = FALSE;
				}				
				else if (1 == m_iSprite_Particle)
				{
					// 1이 킨거.
					m_pCurVoidDesc->bUseSpriteAnim = TRUE;

					// 스프라이트의 루프를 키고 끄기_파티클
					ImGui::RadioButton("Loop Sprite_Particle", &m_iSpriteLoop, 0);
					ImGui::RadioButton("None Loop Sprite_Particle", &m_iSpriteLoop, 1);
					if (0 == m_iSpriteLoop)
					{
						m_pSpriteDesc_Particle->bLoop = TRUE; 
						//m_pSpriteDesc_Particle->Reset_Sprite();
					}
					else if (1 == m_iSpriteLoop)
					{
						m_pSpriteDesc_Particle->bLoop = FALSE;
						//m_pSpriteDesc_Particle->Reset_Sprite();
					}

				}
					

				ImGui::SeparatorText(" Sprite Setting_Particle ");
				if (m_pCurVoidDesc->bUseSpriteAnim)
				{
					m_pSpriteDesc_Particle = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Sprite_Desc();

					/* 스프라이트 재생 속도_파티클 */
					ImGui::SeparatorText("");
					if (ImGui::DragFloat(" Sprite Term ", &m_fSequenceTerm_Particle, 1, 1))
					{
						m_pSpriteDesc_Particle->fSequenceTerm = m_fSequenceTerm_Particle;
						//m_pSpriteDesc_Particle->Reset_Sprite();
					}

					/* 스프라이트 최대 가로 세로 지정_파티클 */
					if (ImGui::InputInt2(" Max_TileCount ", m_vUV_MaxTileCount_Particle, 1))
					{
						_uint iX, iY;
						dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_TextureCom(CEffect_Void::TEXTURE_SPRITE)->Get_TextureSize(&iX, &iY, m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE]);
						m_pSpriteDesc_Particle->vTextureSize.x = (_float)iX;
						m_pSpriteDesc_Particle->vTextureSize.y = (_float)iY;

						_float fTileX, fTileY;
						fTileX = (_float)iX / m_vUV_MaxTileCount_Particle[0];
						fTileY = (_float)iY / m_vUV_MaxTileCount_Particle[1];

						m_pSpriteDesc_Particle->vTileSize.x = fTileX;
						m_pSpriteDesc_Particle->vTileSize.y = fTileY;

						m_pSpriteDesc_Particle->vUV_MaxTileCount.x = (_float)m_vUV_MaxTileCount_Particle[0];
						m_pSpriteDesc_Particle->vUV_MaxTileCount.y = (_float)m_vUV_MaxTileCount_Particle[1];

						m_pSpriteDesc_Particle->Reset_Sprite();
						m_pCurPartEffect->ReSet_Effect();
					}

					ImGui::Text("Current Index : %d, %d", m_pSpriteDesc_Particle->vUV_CurTileIndex.x, m_pSpriteDesc_Particle->vUV_CurTileIndex.y);
				}
#pragma endregion


				if (ImGui::CollapsingHeader(" Shader & RenderGroup _Particle"))
				{
					/* 쉐이더 패스 인덱스 변경_파티클 */
					if (ImGui::InputInt("Shader Pass_Particle", &m_iShaderPassIndex_Particle, 1))
					{
						if (m_iMaxShaderPassIndex_Particle < m_iShaderPassIndex_Particle)
							m_iShaderPassIndex_Particle = m_iMaxShaderPassIndex_Particle;

						if (0 > m_iShaderPassIndex_Particle)
							m_iShaderPassIndex_Particle = 0;

						m_pCurVoidDesc->iShaderPassIndex = m_iShaderPassIndex_Particle;
					}


					/* 쉐이더에 던질 디스카드 값 변경_파티클 */
					ImGui::SeparatorText("Discard_Value");
					if (ImGui::DragFloat4("Discard_Particle", m_vColor_Clip_Part, 0.1f, 0.f, 1.f))
					{
						m_pCurVoidDesc->vColor_Clip.x = m_vColor_Clip_Part[0];
						m_pCurVoidDesc->vColor_Clip.y = m_vColor_Clip_Part[1];
						m_pCurVoidDesc->vColor_Clip.z = m_vColor_Clip_Part[2];
						m_pCurVoidDesc->vColor_Clip.w = m_vColor_Clip_Part[3];
					}


					/* 렌더그룹 변경_파티클(현재는 Effect그룹으로 고정(9번) */
					ImGui::SeparatorText(" Render Group_Particle ");
					if (ImGui::InputInt(" Render Group_Particle ", &m_iRenderGroup_Particle, 1))
					{
						if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Particle)
						{
							m_iRenderGroup_Particle = (_int)CRenderer::RENDER_END - 1;
						}
						m_pCurVoidDesc->iRenderGroup = m_iRenderGroup_Particle;
					}

					ImGui::SeparatorText("");
				}



				/* Z소트 키고 끄기 */
				ImGui::SeparatorText("SortZ");
				ImGui::RadioButton("SortZ", &m_iSortZ, 0);  ImGui::SameLine();
				ImGui::RadioButton("Off SortZ", &m_iSortZ, 1);
				if (0 == m_iSortZ)
					dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Set_SortZ(TRUE);
				else if (1 == m_iSortZ)
					dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Set_SortZ(FALSE);


				/* 인스턴스 개수 변경 */
				ImGui::SeparatorText(" Instance Num_Particle");
				ImGui::Text("MaxInstance : %d", m_iMaxNumInstance_Particle);
				if (ImGui::DragInt("Instance Count", &m_iNumInstance_Particle, 1, 1, m_iNumInstance_Particle))
				{
					// 1보다 작아질 수 없음
					if (1 > m_iNumInstance_Particle)
						m_iNumInstance_Particle = 1;

					// 맥스 개수를 넘어갈 수 없음
					if (m_iMaxNumInstance_Particle < m_iNumInstance_Particle)
						m_iNumInstance_Particle = m_iMaxNumInstance_Particle;

					m_pCurVoidDesc->iCurNumInstance = m_iNumInstance_Particle;
					m_pParticleBufferDesc->iCurNumInstance = m_iNumInstance_Particle;
				}


				/* 라이프 타임 */
				ImGui::SeparatorText(" LifeTimes ");
				if (ImGui::DragFloat2("MinMaxLifeTime_Particle", m_vMinMaxLifeTime_Particle, 1.f, 0.f, 360.f))
				{
					if (m_vMinMaxLifeTime_Particle[0] > m_vMinMaxLifeTime_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxLifeTime_Particle[1] = m_vMinMaxLifeTime_Particle[0];


					m_pParticleBufferDesc->vMinMaxLifeTime.x = m_vMinMaxLifeTime_Particle[0];
					m_pParticleBufferDesc->vMinMaxLifeTime.y = m_vMinMaxLifeTime_Particle[1];
				}


				/* 방출(Emitter) */
				ImGui::SeparatorText(" EmissionTime_Particle ");
				if (ImGui::DragFloat("EmissionTime_Particle", &m_fEmissionTime_Particle, 0.01f, 0.f, 1000.f))
				{
					if (0.f >= m_fEmissionTime_Particle)	// 0.f보다 작아지면 0으로
						m_fEmissionTime_Particle = 0.f;

					m_pParticleBufferDesc->fEmissionTime = m_fEmissionTime_Particle;
				}

				if (ImGui::DragInt("AddEmitCount_Particle", &m_iAddEmitCount_Particle, 1, 0.f, m_pParticleBufferDesc->iCurNumInstance))
				{
					// 정지 및 리셋
					m_pCurEffectDesc->bPlay = FALSE;
					m_pCurEffect->Init_ReSet_Effect();

					if (0 >= m_iAddEmitCount_Particle)	// 0보다 작아지면 0으로
						m_iAddEmitCount_Particle = 0.f;

					if (m_pParticleBufferDesc->iCurNumInstance <= m_iAddEmitCount_Particle)	// 현재 인스턴스 개수와 같거나 커지면 현재 인스턴스 개수로
						m_iAddEmitCount_Particle = m_pParticleBufferDesc->iCurNumInstance;

					m_pParticleBufferDesc->iAddEmitCount = (_uint)m_iAddEmitCount_Particle;
				}



				/* 퍼지는 범위(분포 범위) */
				ImGui::SeparatorText(" Range_Particle ");
				if (ImGui::DragFloat2("MinMaxRange_Particle", m_vMinMaxRange_Particle, 1.f, 0.1f, 360.f))
				{
					if (m_vMinMaxRange_Particle[0] > m_vMinMaxRange_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxRange_Particle[1] = m_vMinMaxRange_Particle[0];


					m_pParticleBufferDesc->vMinMaxRange.x = m_vMinMaxRange_Particle[0];
					m_pParticleBufferDesc->vMinMaxRange.y = m_vMinMaxRange_Particle[1];
				}

				/* 추가 분포 범위(레인지)*/
				ImGui::SeparatorText(" Add Range_Particle ");
				if (ImGui::DragFloat2("MinMax Add Range_Particle", m_fMinMaxAddRange_Particle, 0.1f, -500.f, 500.f))
				{
					m_pParticleBufferDesc->vMinMaxAddRange.x = m_fMinMaxAddRange_Particle[0];
					m_pParticleBufferDesc->vMinMaxAddRange.y = m_fMinMaxAddRange_Particle[1];

				}

				//if (CVIBuffer_Particle::TORNADO == m_pParticleBufferDesc->eType_Action)
				{
					ImGui::SeparatorText(" Theta ");
					HelpMarker(u8"Degree로 입력(Radian 자동변환)");
					if (ImGui::DragFloat2("vMinMaxTheta_Particle", m_vMinMaxTheta_Particle, 0.f, 1.f, 360.f))
					{
						m_pParticleBufferDesc->vMinMaxTheta.x = m_vMinMaxTheta_Particle[0];
						m_pParticleBufferDesc->vMinMaxTheta.y = m_vMinMaxTheta_Particle[1];
					}
				}


				if (CVIBuffer_Particle::FALL == m_pParticleBufferDesc->eType_Action 
					|| CVIBuffer_Particle::RISE == m_pParticleBufferDesc->eType_Action
					|| CVIBuffer_Particle::TORNADO == m_pParticleBufferDesc->eType_Action)
				{

					ImGui::SeparatorText(" Y_Max Pos ");
					if (ImGui::DragFloat2("vMinMaxPosY_Particle", m_vMinMaxPosY_Particle, 0.f, 1.f, 1000.f))
					{
						m_pParticleBufferDesc->vMinMaxPosY.x = m_vMinMaxPosY_Particle[0];
						m_pParticleBufferDesc->vMinMaxPosY.y = m_vMinMaxPosY_Particle[1];
					}
				}





				/* 스피드 조절(러프 관련) */
				ImGui::SeparatorText(" Speed ");

				if (ImGui::DragFloat2("vMinMaxSpeed_Particle", m_vMinMaxSpeed_Particle, 0.1f, -1000.f, 1000.f))
				{
					if (m_pParticleBufferDesc->bUseRigidBody)
					{
						if (0.f > m_vMinMaxSpeed_Particle[0])	// Min이 0보다 작아지면 0으로 고정
							m_vMinMaxSpeed_Particle[0] = 0.f;

						if (m_vMinMaxSpeed_Particle[0] > m_vMinMaxSpeed_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vMinMaxSpeed_Particle[1] = m_vMinMaxSpeed_Particle[0];
					}


					m_pParticleBufferDesc->vMinMaxSpeed.x = m_vMinMaxSpeed_Particle[0];
					m_pParticleBufferDesc->vMinMaxSpeed.y = m_vMinMaxSpeed_Particle[1];
				}

				if (ImGui::CollapsingHeader(" Speed_Easing Types "))
				{
					Select_EasingType(&m_pParticleBufferDesc->eType_SpeedLerp);
				}


				/* 리지드바디 키고 끄기 */
				ImGui::SeparatorText("RigidBody");
				ImGui::RadioButton(" ON  RigidBody_Particle ", &m_iUseRigidBody_Particle, 0);  
				ImGui::RadioButton(" OFF RigidBody_Particle ", &m_iUseRigidBody_Particle, 1);
				if (0 == m_iUseRigidBody_Particle)
				{
					m_pCurVoidDesc->bUseRigidBody = TRUE;
					m_pParticleBufferDesc->bUseRigidBody = TRUE;

					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::SPARK;
				}				
				else if (1 == m_iUseRigidBody_Particle)
				{
					m_pCurVoidDesc->bUseRigidBody = FALSE;
					m_pParticleBufferDesc->bUseRigidBody = FALSE;
				}

#pragma region 리지드바디 옵션 조정_파티클
				/* RigidBody =========================================================================================== */
				if (0 == m_iUseRigidBody_Particle)	// 리지드바디 사용이면 띄우기
				{
					if (ImGui::CollapsingHeader(" RigidBody_Particle "))
					{
						/* 키네틱 키고 끄기 */
						ImGui::RadioButton(" Kinetic_Particle ", &m_iKinetic_Particle, 0);  ImGui::SameLine();
						ImGui::RadioButton(" Kinematic_Particle ", &m_iKinetic_Particle, 1);
						if (0 == m_iKinetic_Particle)
						{

							m_pParticleBufferDesc->bKinetic = TRUE;
						}
						else if (1 == m_iKinetic_Particle)
						{
							m_pParticleBufferDesc->bKinetic = FALSE;
						}

						/* 중력 키고 끄기 */
						ImGui::RadioButton(" ON   Gravity_Particle ", &m_iUseGravity_Particle, 0);  ImGui::SameLine();
						ImGui::RadioButton(" OFF  Gravity_Particle ", &m_iUseGravity_Particle, 1);
						if (0 == m_iUseGravity_Particle)
						{
							m_pParticleBufferDesc->bUseGravity = TRUE;
						}
						else if (1 == m_iUseGravity_Particle)
						{
							m_pParticleBufferDesc->bUseGravity = FALSE;
						}

						/* 중력 가속도 조절 */
						if (0 == m_iUseGravity_Particle)
						{
							if (ImGui::DragFloat("Gravity_Particle", &m_fGravity_Particle, 0.5f, -100.f, 1000.f))
							{
								m_pParticleBufferDesc->fGravity = m_fGravity_Particle;
							}
						}

						/* 리지드바디 : Force Mode */
						ImGui::SeparatorText("Force Mode");
						if (ImGui::Button(" FORCE "))
						{
							m_pParticleBufferDesc->eForce_Mode = FORCE_MODE::FORCE;
						}
						ImGui::SameLine();
						if (ImGui::Button(" IMPULSE "))
						{
							m_pParticleBufferDesc->eForce_Mode = FORCE_MODE::IMPULSE;
						}
						ImGui::SameLine();
						if (ImGui::Button(" ACCELERATION "))
						{
							m_pParticleBufferDesc->eForce_Mode = FORCE_MODE::ACCELERATION;
						}
						ImGui::SameLine();
						if (ImGui::Button(" VELOCITY_CHANGE "))
						{
							m_pParticleBufferDesc->eForce_Mode = FORCE_MODE::VELOCITY_CHANGE;
						}

						/* 파워 조절 */
						ImGui::SeparatorText("Power");
						if (ImGui::DragFloat2("MinMaxPower_Particle", m_vMinMaxPower_Particle, 10.f, 0.1f, 5000.f))
						{
							if (m_vMinMaxPower_Particle[0] > m_vMinMaxPower_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vMinMaxPower_Particle[1] = m_vMinMaxPower_Particle[0];

							m_pParticleBufferDesc->vMinMaxPower.x = m_vMinMaxPower_Particle[0];
							m_pParticleBufferDesc->vMinMaxPower.y = m_vMinMaxPower_Particle[1];
						}

						/* Mass(질량) 조절 */
						ImGui::SeparatorText("Mass");
						if (ImGui::DragFloat2("vMinMaxMass_Particle", m_vMinMaxMass_Particle, 1.f, 0.f, 1000.f))
						{
							if (m_vMinMaxMass_Particle[0] > m_vMinMaxMass_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vMinMaxMass_Particle[1] = m_vMinMaxMass_Particle[0];


							m_pParticleBufferDesc->vMinMaxMass.x = m_vMinMaxMass_Particle[0];
							m_pParticleBufferDesc->vMinMaxMass.y = m_vMinMaxMass_Particle[1];
						}


						/* 마찰계수 조절 */
						//ImGui::SeparatorText("Friction");
						//if (ImGui::DragFloat("Friction_Particle", &m_fFriction_Particle, 0.1f, 0.1f, 100.f))
						//{
						//	m_pParticleBufferDesc->fFriction = m_fFriction_Particle;
						//}

						ImGui::SeparatorText("Friction Lerp_Particle");
						ImGui::SeparatorText("Start End Friction_Particle");
						if (ImGui::DragFloat2("Start End Friction_Particle", m_vStartEnd_Friction_Particle, 0.1f, 0.f, 1.f))
						{
							m_pParticleBufferDesc->vStartEnd_Friction.x = m_vStartEnd_Friction_Particle[0];
							m_pParticleBufferDesc->vStartEnd_Friction.y = m_vStartEnd_Friction_Particle[1];
						}


						ImGui::SeparatorText("FrictionLerp_Pos_Particle");
						if (ImGui::DragFloat2("FrictionLerp_Pos_Particle", m_vFrictionLerp_Pos_Particle, 0.1f, 0.f, 1.f))
						{
							if (m_vFrictionLerp_Pos_Particle[0] > m_vFrictionLerp_Pos_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vFrictionLerp_Pos_Particle[1] = m_vFrictionLerp_Pos_Particle[0];

							m_pParticleBufferDesc->vFrictionLerp_Pos.x = m_vFrictionLerp_Pos_Particle[0];
							m_pParticleBufferDesc->vFrictionLerp_Pos.y = m_vFrictionLerp_Pos_Particle[1];
						}

						if (ImGui::CollapsingHeader(" Friction_Easing Types "))
						{
							Select_EasingType(&m_pParticleBufferDesc->eType_FrictionLerp);
						}


						/* 슬립 한계점 조절(1이하여야 함) */
						ImGui::SeparatorText("SleepThreshold");
						if (ImGui::DragFloat("SleepThreshold_Particle", &m_fSleepThreshold_Particle, 0.01f, 0.f, 1.f))
						{
							m_pParticleBufferDesc->fSleepThreshold = m_fSleepThreshold_Particle;
						}




					} // 콜랩스 끝
				}
#pragma endregion 리지드바디 옵션 조정_파티클 끝


				/* 파티클 리사이클 */
				ImGui::SeparatorText(" Recycle_Particle ");
				ImGui::RadioButton("Recycle_Particle", &m_iRecycle_Particle, 0);  ImGui::SameLine();
				ImGui::RadioButton("None Recycle_Particle", &m_iRecycle_Particle, 1);
				if (0 == m_iRecycle_Particle)
					m_pParticleBufferDesc->bRecycle = TRUE;
				else if (1 == m_iRecycle_Particle)
					m_pParticleBufferDesc->bRecycle = FALSE;


				/* 파티클 진행방향 리버스 Reverse */
				ImGui::SeparatorText(" Reverse_Particle ");
				ImGui::RadioButton("Reverse_Particle", &m_iReverse_Particle, 0);  ImGui::SameLine();
				ImGui::RadioButton("None Reverse_Particle", &m_iReverse_Particle, 1);
				if (0 == m_iReverse_Particle)
					m_pParticleBufferDesc->bReverse = TRUE;
				else if (1 == m_iReverse_Particle)
					m_pParticleBufferDesc->bReverse = FALSE;


				/* 파티클 액션 타입 */
				ImGui::SeparatorText(" Action Type_Particle ");
				if (ImGui::RadioButton("SPARK_Particle ", &m_iType_Action_Particle, 0))
					pVIBuffer->ReSet();
				if(ImGui::RadioButton("BLINK_Particle", &m_iType_Action_Particle, 1))
					pVIBuffer->ReSet();
				if(ImGui::RadioButton("FALL_Particle", &m_iType_Action_Particle, 2))
					pVIBuffer->ReSet();
				if(ImGui::RadioButton("RISE_Particle", &m_iType_Action_Particle, 3))
					pVIBuffer->ReSet();
				if(ImGui::RadioButton("TORNADO_Particle", &m_iType_Action_Particle, 4))
					pVIBuffer->ReSet();
				ImGui::RadioButton("TYPE_ACTION_END_Particle", &m_iType_Action_Particle, 5);


				if (0 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::SPARK;
				else if (1 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::BLINK;
				else if (2 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::FALL;
				else if (3 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::RISE;
				else if (4 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::TORNADO;
				else if (5 == m_iType_Action_Particle)
					m_pParticleBufferDesc->eType_Action = CVIBuffer_Particle::TYPE_ACTION_END;


				/* 센터위치 오프셋(000 에서 어느정도 떨어진 지점에서 스폰 될건지) */
				ImGui::SeparatorText("Center Offset_Particle");
				/* MinCenterOffsetPos */
				if (ImGui::DragFloat3("MinCenterOffsetPos_Mesh", m_vMinCenterOffsetPos_Particle, 0.1f, -500.f, 500.f))
				{
					m_pParticleBufferDesc->vMinCenterOffsetPos.x = m_vMinCenterOffsetPos_Particle[0];
					m_pParticleBufferDesc->vMinCenterOffsetPos.y = m_vMinCenterOffsetPos_Particle[1];
					m_pParticleBufferDesc->vMinCenterOffsetPos.z = m_vMinCenterOffsetPos_Particle[2];
				}
				/* MaxCenterOffsetPos */
				if (ImGui::DragFloat3("MaxCenterOffsetPos_Mesh", m_vMaxCenterOffsetPos_Particle, 0.1f, -500.f, 500.f))
				{
					m_pParticleBufferDesc->vMaxCenterOffsetPos.x = m_vMaxCenterOffsetPos_Particle[0];
					m_pParticleBufferDesc->vMaxCenterOffsetPos.y = m_vMaxCenterOffsetPos_Particle[1];
					m_pParticleBufferDesc->vMaxCenterOffsetPos.z = m_vMaxCenterOffsetPos_Particle[2];
				}


				if (ImGui::CollapsingHeader(" Dir Rot Offset _Particle "))
				{
					/* 회전 범위(오프셋) */
					/* RotX */
					ImGui::SeparatorText("");
					if (ImGui::DragFloat2("RotationX", m_vRotationOffsetX_Particle, 1.f, 0.f, 360.f))
					{
						if (m_vRotationOffsetX_Particle[0] > m_vRotationOffsetX_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vRotationOffsetX_Particle[1] = m_vRotationOffsetX_Particle[0];


						m_pParticleBufferDesc->vMinMaxRotationOffsetX.x = m_vRotationOffsetX_Particle[0];
						m_pParticleBufferDesc->vMinMaxRotationOffsetX.y = m_vRotationOffsetX_Particle[1];

					}

					/* RotY */
					if (ImGui::DragFloat2("RotationY", m_vRotationOffsetY_Particle, 1.f, 0.f, 360.f))
					{
						if (m_vRotationOffsetY_Particle[0] > m_vRotationOffsetY_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vRotationOffsetY_Particle[1] = m_vRotationOffsetY_Particle[0];


						m_pParticleBufferDesc->vMinMaxRotationOffsetY.x = m_vRotationOffsetY_Particle[0];
						m_pParticleBufferDesc->vMinMaxRotationOffsetY.y = m_vRotationOffsetY_Particle[1];
					}

					/* RotZ */
					if (ImGui::DragFloat2("RotationZ", m_vRotationOffsetZ_Particle, 1.f, 0.f, 360.f))
					{
						if (m_vRotationOffsetZ_Particle[0] > m_vRotationOffsetZ_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vRotationOffsetZ_Particle[1] = m_vRotationOffsetZ_Particle[0];

						m_pParticleBufferDesc->vMinMaxRotationOffsetZ.x = m_vRotationOffsetZ_Particle[0];
						m_pParticleBufferDesc->vMinMaxRotationOffsetZ.y = m_vRotationOffsetZ_Particle[1];
					}

					ImGui::SeparatorText("");
				}


#pragma region 크기 변경 러프_파티클 시작
				if (ImGui::CollapsingHeader(" Scale_Particle "))
				{
					ImGui::SeparatorText("MinMax Width");
					if (ImGui::DragFloat2("MinMax Width_Particle", m_vMinMaxWidth_Particle, 0.1f, 0.f, 5000.f))
					{
						if (m_vMinMaxWidth_Particle[0] > m_vMinMaxWidth_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vMinMaxWidth_Particle[1] = m_vMinMaxWidth_Particle[0];

						m_pParticleBufferDesc->vMinMaxWidth.x = m_vMinMaxWidth_Particle[0];
						m_pParticleBufferDesc->vMinMaxWidth.y = m_vMinMaxWidth_Particle[1];
					}

					ImGui::SeparatorText("MinMax Height");
					if (ImGui::DragFloat2("MinMax Height_Particle", m_vMinMaxHeight_Particle, 0.1f, 0.f, 5000.f))
					{
						if (m_vMinMaxHeight_Particle[0] > m_vMinMaxHeight_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vMinMaxHeight_Particle[1] = m_vMinMaxHeight_Particle[0];

						m_pParticleBufferDesc->vMinMaxHeight.x = m_vMinMaxHeight_Particle[0];
						m_pParticleBufferDesc->vMinMaxHeight.y = m_vMinMaxHeight_Particle[1];
					}


					/* 파티클 크기 변경 러프 사용 */
					ImGui::SeparatorText(" Use_ScaleLerp _Particle ");
					ImGui::RadioButton("Use_ScaleLerp_Particle", &m_iUseScaleLerp, 0);  ImGui::SameLine();
					ImGui::RadioButton("None_ScaleLerp_Particle", &m_iUseScaleLerp, 1);
					if (0 == m_iUseScaleLerp)
						m_pParticleBufferDesc->bUseScaleLerp = TRUE;
					else if (1 == m_iUseScaleLerp)
						m_pParticleBufferDesc->bUseScaleLerp = FALSE;

					if (TRUE == m_pParticleBufferDesc->bUseScaleLerp)
					{
						ImGui::SeparatorText("ScaleLerp Pos");
						if (ImGui::DragFloat2("Up ScaleLerp Pos_Particle", m_vScaleLerp_Up_Pos, 0.1f, 0.f, 1.f))
						{
							if (m_vScaleLerp_Up_Pos[0] > m_vScaleLerp_Up_Pos[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vScaleLerp_Up_Pos[1] = m_vScaleLerp_Up_Pos[0];

							m_pParticleBufferDesc->vScaleLerp_Up_Pos.x = m_vScaleLerp_Up_Pos[0];
							m_pParticleBufferDesc->vScaleLerp_Up_Pos.y = m_vScaleLerp_Up_Pos[1];
						}

						if (ImGui::DragFloat2("Down ScaleLerp Pos_Particle", m_vScaleLerp_Down_Pos, 0.1f, 0.f, 1.f))
						{
							if (m_vScaleLerp_Down_Pos[0] > m_vScaleLerp_Down_Pos[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vScaleLerp_Down_Pos[1] = m_vScaleLerp_Down_Pos[0];

							m_pParticleBufferDesc->vScaleLerp_Down_Pos.x = m_vScaleLerp_Down_Pos[0];
							m_pParticleBufferDesc->vScaleLerp_Down_Pos.y = 1.f; // 1로 고정
						}

						if (ImGui::CollapsingHeader(" Scale_Easing Types "))
						{
							Select_EasingType(&m_pParticleBufferDesc->eType_ScaleLerp);
						}
					}

				}
#pragma endregion 크기 변경 러프_파티클 끝



#pragma region 자체 회전_파티클 시작
				if (ImGui::CollapsingHeader(" Rotate_Particle "))
				{
					if (ImGui::DragFloat(" Rotation_Radian_X ", &m_vRadian_Particle[0], 1.f, 0.f, 360.f))
						m_pParticleBufferDesc->vRadian.x = m_vRadian_Particle[0];

					if (ImGui::DragFloat(" Rotation_Radian_Y ", &m_vRadian_Particle[1], 1.f, 0.f, 360.f))
						m_pParticleBufferDesc->vRadian.y = m_vRadian_Particle[1];

					if (ImGui::DragFloat(" Rotation_Radian_Z ", &m_vRadian_Particle[2], 1.f, 0.f, 360.f))
						m_pParticleBufferDesc->vRadian.z = m_vRadian_Particle[2];
				}
#pragma endregion 자체 회전_파티클 끝


#pragma region 색 변경_파티클

				/* 파티클 페이드 인아웃 사용 */
				ImGui::SeparatorText(" Fade Type ");
				ImGui::RadioButton("FADE_NONE_Particle", &m_iType_Fade_Particle, 0);  ImGui::SameLine();
				ImGui::RadioButton("FADE_OUT_Particle", &m_iType_Fade_Particle, 1);
				ImGui::RadioButton("FADE_IN_Particle", &m_iType_Fade_Particle, 2);
				if (0 == m_iType_Fade_Particle)
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_NONE;
				else if (1 == m_iType_Fade_Particle)
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_OUT;
				else if (2 == m_iType_Fade_Particle)
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_IN;


				/* 색 변경 */
				if (ImGui::CollapsingHeader(" Color_Particle "))
				{

					/* 디퓨즈 색상혼합 모드_Particle */
					ImGui::RadioButton(u8"곱하기_Particle", &m_iColor_Mode_Particle, 0);
					ImGui::RadioButton(u8"스크린_Particle", &m_iColor_Mode_Particle, 1);
					ImGui::RadioButton(u8"오버레이_Particle", &m_iColor_Mode_Particle, 2);
					ImGui::RadioButton(u8"더하기_Particle", &m_iColor_Mode_Particle, 3);
					ImGui::RadioButton(u8"번_Particle", &m_iColor_Mode_Particle, 4);
					ImGui::RadioButton(u8"혼합안함_Particle", &m_iColor_Mode_Particle, 5);
					if (0 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::MUL;
					else if (1 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::SCREEN;
					else if (2 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::OVERLAY;
					else if (3 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::ADD;
					else if (4 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::BURN;
					else if (5 == m_iColor_Mode_Particle)
						m_pCurVoidDesc->eMode_Color = MODE_COLOR::MODE_COLOR_END;


					/* 입자마다 다른 주기로 색 변경 키고 끄기 */
					ImGui::RadioButton(" ON   Dynamic_Color_Particle ", &m_iDynamic_Color_Particle, 0);  ImGui::SameLine();
					ImGui::RadioButton(" OFF  Dynamic_Color_Particle ", &m_iDynamic_Color_Particle, 1);
					if (0 == m_iDynamic_Color_Particle)
					{
						m_pParticleBufferDesc->bDynamic_Color = TRUE;
					}
					else if (1 == m_iDynamic_Color_Particle)
					{

						m_pParticleBufferDesc->bDynamic_Color = FALSE;
					}

					/* Min 색 설정_파티클 */
					if (ImGui::ColorEdit4("Min_Color_Particle", m_fColor_Min_Particle, ImGuiColorEditFlags_None))
					{
						m_pParticleBufferDesc->vMinMaxRed.x = m_fColor_Min_Particle[0];
						m_pParticleBufferDesc->vMinMaxGreen.x = m_fColor_Min_Particle[1];
						m_pParticleBufferDesc->vMinMaxBlue.x = m_fColor_Min_Particle[2];
						m_pParticleBufferDesc->vMinMaxAlpha.x = m_fColor_Min_Particle[3];
					}

					/* Max 색 설정_파티클 */
					if (ImGui::ColorEdit4("Max_Color_Particle", m_fColor_Max_Particle, ImGuiColorEditFlags_None))
					{
						m_pParticleBufferDesc->vMinMaxRed.y = m_fColor_Max_Particle[0];
						m_pParticleBufferDesc->vMinMaxGreen.y = m_fColor_Max_Particle[1];
						m_pParticleBufferDesc->vMinMaxBlue.y = m_fColor_Max_Particle[2];
						m_pParticleBufferDesc->vMinMaxAlpha.y = m_fColor_Max_Particle[3];
					}

					if (1 == m_iDynamic_Color_Particle) // 입자 색 일괄변경이면 현재 색 변화 보여주기
					{
						ImGui::ColorEdit4("Cur_Color_Particle", m_fColor_Cur_Particle, ImGuiColorEditFlags_None);
						m_fColor_Cur_Particle[0] = m_pParticleBufferDesc->vCurrentColor.x;
						m_fColor_Cur_Particle[1] = m_pParticleBufferDesc->vCurrentColor.y;
						m_fColor_Cur_Particle[2] = m_pParticleBufferDesc->vCurrentColor.z;
						m_fColor_Cur_Particle[3] = m_pParticleBufferDesc->vCurrentColor.w;
					}


					if (ImGui::CollapsingHeader(" Color_Easing Types "))
					{
						Select_EasingType(&m_pParticleBufferDesc->eType_ColorLerp);
					}


					ImGui::SeparatorText("");
				}
#pragma endregion 색 변경_파티클 끝



				/* 텍스처 UV회전_파티클 */
				if (ImGui::CollapsingHeader("UV Option_Particle"))
				{

					if (ImGui::DragFloat2(" UV_Offset_Particle ", m_vUV_Offset_Particle, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Offset.x = m_vUV_Offset_Particle[0];
						m_pCurVoidDesc->vUV_Offset.y = m_vUV_Offset_Particle[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 이동");

					if (ImGui::DragFloat2(" UV_Scale_Particle ", m_vUV_Scale_Particle, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Scale.x = m_vUV_Scale_Particle[0];
						m_pCurVoidDesc->vUV_Scale.y = m_vUV_Scale_Particle[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 크기(타일링)");


					if (ImGui::DragFloat(" UV Degree_Particle ", &m_fUV_RotDegree_Particle, 1.f, 0.f, 360.f))
						m_pCurVoidDesc->fUV_RotDegree = m_fUV_RotDegree_Particle;


					ImGui::SeparatorText("");
				}

				

				/* 림라이트 색 변경_파티클 */
				ImGui::SeparatorText("");
				if (ImGui::ColorEdit4("RimColor_Particle", m_fRimColor_Particle, ImGuiColorEditFlags_None))
				{
					m_pCurVoidDesc->vRimColor.x = m_fRimColor_Particle[0];
					m_pCurVoidDesc->vRimColor.y = m_fRimColor_Particle[1];
					m_pCurVoidDesc->vRimColor.z = m_fRimColor_Particle[2];
					m_pCurVoidDesc->vRimColor.w = m_fRimColor_Particle[3];
				}

				if (ImGui::ColorEdit3("BloomPower_Particle", m_vBloomPower_Particle, ImGuiColorEditFlags_None))
				{
					m_pCurVoidDesc->vBloomPower.x = m_vBloomPower_Particle[0];
					m_pCurVoidDesc->vBloomPower.y = m_vBloomPower_Particle[1];
					m_pCurVoidDesc->vBloomPower.z = m_vBloomPower_Particle[2];
				}



#pragma region 디스토션_파티클
				// 디스토션 값 변경_파티클
				if (ImGui::CollapsingHeader("Distortion _Particle"))
				{
					CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Distortion_Desc();
					
					if (ImGui::CollapsingHeader(" Distortion Preset "))
					{
						if (ImGui::Button("FIRE"))
						{
							pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_COL };

							pDistortionDesc->vScrollSpeeds = { 1.f, 1.f, 1.f };
							pDistortionDesc->vScales = { 1.f, 1.f, 1.f };

							pDistortionDesc->vDistortion1 = { 0.1f, 0.1f };
							pDistortionDesc->vDistortion2 = { 0.f, 0.f };
							pDistortionDesc->vDistortion3 = { 0.f, 0.1f };

							pDistortionDesc->fDistortionScale = { 1.f };
							pDistortionDesc->fDistortionBias = { 1.f };

							Update_CurParameters_Parts();
						}

						ImGui::SeparatorText("");
					}
		
					/* 디스토션 스크롤 방향 변경 */
					ImGui::RadioButton(" Row_Scroll_Particle ", &m_iType_Scroll_Particle, 0);  ImGui::SameLine();
					ImGui::RadioButton("Col_Scroll_Particle", &m_iType_Scroll_Particle, 1);
					ImGui::RadioButton("Both_Scroll_Particle", &m_iType_Scroll_Particle, 2);
					ImGui::RadioButton("Rotat_Scroll_Particle", &m_iType_Scroll_Particle, 3);
					ImGui::RadioButton("End_Scroll_Particle", &m_iType_Scroll_Particle, 4);
					if (0 == m_iType_Scroll_Particle)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROW };
					else if (1 == m_iType_Scroll_Particle)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_COL };
					else if (2 == m_iType_Scroll_Particle)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_BOTH };
					else if (3 == m_iType_Scroll_Particle)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROTAT };
					else if (4 == m_iType_Scroll_Particle)
						pDistortionDesc->eType_Scroll = { CEffect_Void::TYPE_SCROLL_END };


					if (ImGui::DragFloat3("ScrollSpeeds", m_vScrollSpeeds_Particle, 0.1f, -100.f))
					{
						pDistortionDesc->vScrollSpeeds.x = m_vScrollSpeeds_Particle[0];
						pDistortionDesc->vScrollSpeeds.y = m_vScrollSpeeds_Particle[1];
						pDistortionDesc->vScrollSpeeds.z = m_vScrollSpeeds_Particle[2];
					}
					if (ImGui::DragFloat3("Distortion_Scales", m_vScales_Distortion_Particle, 0.1f, 0.f))
					{
						pDistortionDesc->vScales.x = m_vScales_Distortion_Particle[0];
						pDistortionDesc->vScales.y = m_vScales_Distortion_Particle[1];
						pDistortionDesc->vScales.z = m_vScales_Distortion_Particle[2];
					}
					if (ImGui::DragFloat2("Distortion1", m_vDistortion1_Particle, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion1.x = m_vDistortion1_Particle[0];
						pDistortionDesc->vDistortion1.y = m_vDistortion1_Particle[1];
					}
					if (ImGui::DragFloat2("Distortion2", m_vDistortion2_Particle, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion2.x = m_vDistortion2_Particle[0];
						pDistortionDesc->vDistortion2.y = m_vDistortion2_Particle[1];
					}
					if (ImGui::DragFloat2("Distortion3", m_vDistortion3_Particle, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion3.x = m_vDistortion3_Particle[0];
						pDistortionDesc->vDistortion3.y = m_vDistortion3_Particle[1];
					}
					if (ImGui::DragFloat("Distortion_Scale", &m_fDistortionScale_Particle, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionScale = m_fDistortionScale_Particle;
					}
					if (ImGui::DragFloat("DistortionBias", &m_fDistortionBias_Particle, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionBias = m_fDistortionBias_Particle;
					}

					ImGui::SeparatorText("");
				}
#pragma endregion 디스토션_파티클 끝


			}
		}
	}
}

void CWindow_EffectTool::Update_RectTab()
{
	if (nullptr != m_pCurEffect)
	{
		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurVoidDesc->eType_Effect;

			if (CEffect_Void::RECT == eType_Effect)
			{
				m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();
				m_pRectDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Desc();
				CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Distortion_Desc();

				/* 이름 */
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pCurVoidDesc->strPartTag));

				/* 텍스처 변경 */

#pragma region 텍스처 설정_렉트
				/* 텍스처 변경_렉트 */
				ImGui::CollapsingHeader(" Texture_Rect ");
				if (m_pCurVoidDesc->bUseSpriteAnim)
				{
					// 디퓨즈_스프라이트_렉트 텍스처 ============================================================================================
					ImGui::SeparatorText("Diffuse_Sprte_Rect");

					if (ImGui::Button("Sprite_Base_Rect"))	// 베이스 스프라이트로 변경
					{
						dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Sprite"));
						m_pCurVoidDesc->strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
						m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 21;
						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = 0;	// 텍스처 인덱스 초기화
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 0;
					}
					ImGui::SameLine();
					if (ImGui::Button("Sprite_Smokes_Rect"))	// 스모크 스프라이트로 변경
					{
						dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Sprite_Smokes"));
						m_pCurVoidDesc->strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite_Smokes");
						m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 34;
						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = 0;	// 텍스처 인덱스 초기화
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 0;
					}

					if (ImGui::InputInt("Sprite_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE], 1))
					{
						if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] - 1;

						if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE];
					}


				}
				else
				{
					// 디퓨즈_렉트 텍스처 ===============================================================================================
					ImGui::SeparatorText("Diffuse_Rect");

					if (ImGui::Button("Diffuse_Base"))	// 베이스 디퓨즈로 변경
					{
						dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Diffuse"));
						m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = 13;
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = 0;
					}


					if (ImGui::InputInt("Diffuse_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE], 1))
					{
						if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] - 1;

						if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE];
					}
				}


				// 마스크_렉트 텍스처 =====================================================================================================
				ImGui::SeparatorText("Mask_Rect");

				if (ImGui::Button("Mask_Base"))	// 베이스 마스크로 변경
				{
					dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask"));
					m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 44;
					m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 0;

				}ImGui::SameLine();
				if (ImGui::Button("Mask_Waves"))	// 웨이브 마스크로 변경
				{
					dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask_Waves"));
					m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 6;
					m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 0;
				}

				if (ImGui::Button(" Remove Mask_Rect "))
				{
					dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Remove_TextureCom(CEffect_Void::TEXTURE_MASK);
				}


				if (ImGui::InputInt("Mask_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK], 1))
				{
					if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK] - 1;

					if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 0;

					m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK];
				}


				// 노이즈_렉트 텍스처 =====================================================================================================
				ImGui::SeparatorText("Noise_Rect");
				if (ImGui::Button("Noise_Base"))	// 베이스 노이즈로 변경
				{
					dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Noise"));
					m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 243;
					m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Noise_Lens"))	// 렌즈 노이즈로 변경
				{
					dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Noise_Lens"));
					m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 1;
					m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 0;
				}

				if (ImGui::InputInt("Noise_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE], 1))
				{
					if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] - 1;

					if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 0;

					m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE];
				}
#pragma endregion

				/* 쉐이더에 던질 곱하기 컬러 값_파티클 */
				ImGui::SeparatorText("Color_Mul");
				if (ImGui::ColorEdit4("Color_Mul_Rect", m_fColor_Mul_Rect, ImGuiColorEditFlags_None))
				{
					m_pCurVoidDesc->vColor_Mul.x = m_fColor_Mul_Rect[0];
					m_pCurVoidDesc->vColor_Mul.y = m_fColor_Mul_Rect[1];
					m_pCurVoidDesc->vColor_Mul.z = m_fColor_Mul_Rect[2];
					m_pCurVoidDesc->vColor_Mul.w = m_fColor_Mul_Rect[3];
				}


				/* 빌보드 키고 끄기 */
				ImGui::SeparatorText("Shader_Rect");

				ImGui::RadioButton("BillBoard_Rect", &m_iBillBoard_Rect, 0);  ImGui::SameLine();
				ImGui::RadioButton("Off BillBoard_Rect", &m_iBillBoard_Rect, 1);
				if (0 == m_iBillBoard_Rect)
					m_pCurVoidDesc->bBillBoard = TRUE;
				else if (1 == m_iBillBoard_Rect)
					m_pCurVoidDesc->bBillBoard = FALSE;


				/* 쉐이더 패스 인덱스 변경 */
				if (ImGui::InputInt("Shader Pass_Rect", &m_iShaderPassIndex_Rect, 1))
				{
					if (m_iMaxShaderPassIndex_Rect < m_iShaderPassIndex_Rect)
						m_iShaderPassIndex_Rect = m_iMaxShaderPassIndex_Rect;

					if (0 > m_iShaderPassIndex_Rect)
						m_iShaderPassIndex_Rect = 0;

					m_pCurVoidDesc->iShaderPassIndex = m_iShaderPassIndex_Rect;
				}
				/* 쉐이더에 던질 디스카드 값 변경 */
				//vColor_Clip
				if (ImGui::DragFloat4("Discard_Rect", m_vColor_Clip_Rect, 0.1f, 0.f, 1.f))
				{
					m_pCurVoidDesc->vColor_Clip.x = m_vColor_Clip_Rect[0];
					m_pCurVoidDesc->vColor_Clip.y = m_vColor_Clip_Rect[1];
					m_pCurVoidDesc->vColor_Clip.z = m_vColor_Clip_Rect[2];
					m_pCurVoidDesc->vColor_Clip.w = m_vColor_Clip_Rect[3];
				}

				/* 렌더그룹 변경 */
				ImGui::SeparatorText("Render Group_Rect");
				if (ImGui::InputInt("Render Group_Rect", &m_iRenderGroup_Rect, 1))
				{
					if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Rect)
					{
						m_iRenderGroup_Rect = (_int)CRenderer::RENDER_END - 1;
					}
					m_pCurVoidDesc->iRenderGroup = m_iRenderGroup_Rect;
				}



				ImGui::SeparatorText("");
				if (ImGui::CollapsingHeader(" Distortion Preset "))
				{
					if (ImGui::Button("FIRE"))
					{
						m_pCurVoidDesc->iShaderPassIndex = { 2 };
						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 1 };
						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 17 };
						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 1 };

						pDistortionDesc->vScrollSpeeds = { 1.f, 1.f, 1.f };
						pDistortionDesc->vScales = { 1.f, 1.f, 1.f };

						pDistortionDesc->vDistortion1 = { 0.1f, 0.1f };
						pDistortionDesc->vDistortion2 = { 0.f, 0.f };
						pDistortionDesc->vDistortion3 = { 0.f, 0.1f };

						pDistortionDesc->fDistortionScale = { 1.f };
						pDistortionDesc->fDistortionBias = { 1.f };

						Update_CurParameters_Parts();
					}

					ImGui::SeparatorText(" Distortion Values_Rect ");


					/* 디스토션 스크롤 방향 변경 */
					ImGui::RadioButton("Row_Scroll_Rect", &m_iType_Scroll_Rect, 0);  ImGui::SameLine();
					ImGui::RadioButton("Col_Scroll_Rect", &m_iType_Scroll_Rect, 1);
					ImGui::RadioButton("Both_Scroll_Rect", &m_iType_Scroll_Rect, 2);
					ImGui::RadioButton("Rotat_Scroll_Rect", &m_iType_Scroll_Rect, 3);
					ImGui::RadioButton("End_Scroll_Rect", &m_iType_Scroll_Rect, 4);
					if (0 == m_iType_Scroll_Rect)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROW };
					else if (1 == m_iType_Scroll_Rect)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_COL };
					else if (2 == m_iType_Scroll_Rect)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_BOTH };
					else if (3 == m_iType_Scroll_Rect)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROTAT };
					else if (4 == m_iType_Scroll_Rect)
						pDistortionDesc->eType_Scroll = { CEffect_Void::TYPE_SCROLL_END };


					if (ImGui::DragFloat3("ScrollSpeeds", m_vScrollSpeeds, 1.f, 0.f))
					{
						pDistortionDesc->vScrollSpeeds.x = m_vScrollSpeeds[0];
						pDistortionDesc->vScrollSpeeds.y = m_vScrollSpeeds[1];
						pDistortionDesc->vScrollSpeeds.z = m_vScrollSpeeds[2];
					}
					if (ImGui::DragFloat3("Distortion_Scales", m_vScales_Distortion, 1.f, 0.f))
					{
						pDistortionDesc->vScales.x = m_vScales_Distortion[0];
						pDistortionDesc->vScales.y = m_vScales_Distortion[1];
						pDistortionDesc->vScales.z = m_vScales_Distortion[2];
					}
					if (ImGui::DragFloat2("Distortion1", m_vDistortion1, 1.f, 0.f))
					{
						pDistortionDesc->vDistortion1.x = m_vDistortion1[0];
						pDistortionDesc->vDistortion1.y = m_vDistortion1[1];
					}
					if (ImGui::DragFloat2("Distortion2", m_vDistortion2, 1.f, 0.f))
					{
						pDistortionDesc->vDistortion2.x = m_vDistortion2[0];
						pDistortionDesc->vDistortion2.y = m_vDistortion2[1];
					}
					if (ImGui::DragFloat2("Distortion3", m_vDistortion3, 1.f, 0.f))
					{
						pDistortionDesc->vDistortion3.x = m_vDistortion3[0];
						pDistortionDesc->vDistortion3.y = m_vDistortion3[1];
					}
					if (ImGui::DragFloat("Distortion_Scale", &m_fDistortionScale, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionScale = m_fDistortionScale;
					}
					if (ImGui::DragFloat("DistortionBias", &m_fDistortionBias, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionBias = m_fDistortionBias;
					}
				}



				/* UV 값 조절_Rect */
				if (ImGui::CollapsingHeader("UV Option_Rect"))
				{
					if (ImGui::DragFloat2(" UV_Offset_Rect ", m_fUV_Offset_Rect, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Offset.x = m_fUV_Offset_Rect[0];
						m_pCurVoidDesc->vUV_Offset.y = m_fUV_Offset_Rect[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 이동");

					if (ImGui::DragFloat2(" UV_Scale ", m_vUV_Scale_Rect, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Scale.x = m_vUV_Scale_Rect[0];
						m_pCurVoidDesc->vUV_Scale.y = m_vUV_Scale_Rect[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 크기(타일링)");

					if (ImGui::DragFloat(" RotDegree_Rect ", &m_fUV_RotDegree_Rect, 1.f, 0.f, 360.f))
					{
						m_pCurVoidDesc->fUV_RotDegree = m_fUV_RotDegree_Rect;
					}ImGui::SameLine();
					HelpMarker(u8"회전");

					ImGui::SeparatorText("");
				}




#pragma region 스프라이트 설정_렉트
				/* 스프라이트 키고 끄기 */
				ImGui::SeparatorText(" Sprite ");
				ImGui::RadioButton("Off Sprite_Rect", &m_iSprite_Rect, 0); ImGui::SameLine();
				ImGui::RadioButton("Sprite_Rect", &m_iSprite_Rect, 1);
				if (0 == m_iSprite_Rect)
				{
					m_pCurVoidDesc->bUseSpriteAnim = FALSE;
				}
				else if (1 == m_iSprite_Rect)
				{
					CEffect_Void::UVSPRITE_DESC* pSpriteDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Sprite_Desc();

					// 1이 킨거.
					m_pCurVoidDesc->bUseSpriteAnim = TRUE;

					// 스프라이트의 루프를 키고 끄기_렉트
					ImGui::RadioButton("Loop Sprite_Rect", &m_iSpriteLoop, 0); ImGui::SameLine();
					ImGui::RadioButton("None Loop Sprite_Rect", &m_iSpriteLoop, 1);
					if (0 == m_iSpriteLoop)
					{
						pSpriteDesc->bLoop = TRUE;
					}
					else if (1 == m_iSpriteLoop)
					{
						pSpriteDesc->bLoop = FALSE;
					}

				}

				ImGui::SeparatorText(" Sprite Setting_Rect ");
				if (m_pCurVoidDesc->bUseSpriteAnim)
				{
					CEffect_Void::UVSPRITE_DESC* pSpriteDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Sprite_Desc();

					/* 스프라이트 재생 속도_렉트 */
					ImGui::SeparatorText("");
					if (ImGui::DragFloat(" Sprite Term ", &m_fSequenceTerm_Rect, 1, 1))
					{
						pSpriteDesc->fSequenceTerm = m_fSequenceTerm_Rect;
						//m_pSpriteDesc_Rect->Reset_Sprite();
					}

					/* 스프라이트 최대 가로 세로 지정_렉트 */
					if (ImGui::InputInt2(" Max_TileCount ", m_vUV_MaxTileCount_Rect, 1))
					{
						_uint iX, iY;
						dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_TextureCom(CEffect_Void::TEXTURE_SPRITE)->Get_TextureSize(&iX, &iY, m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE]);
						pSpriteDesc->vTextureSize.x = (_float)iX;
						pSpriteDesc->vTextureSize.y = (_float)iY;

						_float fTileX, fTileY;
						fTileX = (_float)iX / m_vUV_MaxTileCount_Rect[0];
						fTileY = (_float)iY / m_vUV_MaxTileCount_Rect[1];

						pSpriteDesc->vTileSize.x = fTileX;
						pSpriteDesc->vTileSize.y = fTileY;

						pSpriteDesc->vUV_MaxTileCount.x = (_float)m_vUV_MaxTileCount_Rect[0];
						pSpriteDesc->vUV_MaxTileCount.y = (_float)m_vUV_MaxTileCount_Rect[1];

						pSpriteDesc->Reset_Sprite();
						m_pCurPartEffect->ReSet_Effect();
					}

					ImGui::Text("Current Index : %d, %d", pSpriteDesc->vUV_CurTileIndex.x, pSpriteDesc->vUV_CurTileIndex.y);
				}
#pragma endregion bUseSpriteAnim 스프라이트 사용



				/* Rim & Bloom */
				/* 림라이트 & 블룸 */
				ImGui::SeparatorText("Rim Bloom_Rect");
				if (ImGui::ColorEdit4("RimColor_Rect", m_fRimColor_Rect, ImGuiColorEditFlags_None))
				{
					m_pCurVoidDesc->vRimColor.x = m_fRimColor_Rect[0];
					m_pCurVoidDesc->vRimColor.y = m_fRimColor_Rect[1];
					m_pCurVoidDesc->vRimColor.z = m_fRimColor_Rect[2];
					m_pCurVoidDesc->vRimColor.w = m_fRimColor_Rect[3];
				}

				if (ImGui::ColorEdit3("BloomPower_Rect", m_vBloomPower_Rect, ImGuiColorEditFlags_None))
				{
					m_pCurVoidDesc->vBloomPower.x = m_vBloomPower_Rect[0];
					m_pCurVoidDesc->vBloomPower.y = m_vBloomPower_Rect[1];
					m_pCurVoidDesc->vBloomPower.z = m_vBloomPower_Rect[2];
				}





			} // Rect 타입 업데이트 엔드
			
		}
	}
}

void CWindow_EffectTool::Update_MeshTab()
{
	if (nullptr != m_pCurEffect)
	{
		// 테스트용 데모 이펙트 메쉬 생성
		if (ImGui::CollapsingHeader(" Demo Meshs "))
		{
			if (ImGui::Button("Demo_Xray"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Xray_ManHeavy"));
			}

			if (ImGui::Button("Demo_Tornado_spline"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Tornado_splineMesh"));
			}

			if (ImGui::Button("Demo_BloodPoolsRaid"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_BloodPoolsRaid"));
			}

			ImGui::SeparatorText("");
		}


		// 메쉬 파티클용 이펙트 메쉬 생성
		if (ImGui::CollapsingHeader(" Mesh_Particle "))
		{
			if (ImGui::Button("Particle_Test"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Particle_Test"));
			}

			ImGui::SeparatorText("");
		}


		// 이펙트용 스태틱 메쉬 생성
		if (ImGui::CollapsingHeader(" Mesh_Static "))
		{
			if (ImGui::Button("Ring"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Ring"));
			}ImGui::SameLine();
			if (ImGui::Button("Sphere"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Sphere"));
			}

			if (ImGui::Button("Projectile"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Projectile"));
			}

			if (ImGui::Button("Corn"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Corn"));
			}ImGui::SameLine();
			if (ImGui::Button("ShieldDome"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_ShieldDome"));
			}

			if (ImGui::Button("Tornado"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Tornado"));
			}ImGui::SameLine();
			if (ImGui::Button("Tornado_Cream"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Tornado_cream"));
			}

			if (ImGui::Button("WinchesterElectric"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_WinchesterElectric"));
			}

			if (ImGui::Button("LightningFast"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_LightningFast"));
			}
			ImGui::SameLine();
			if (ImGui::Button("LightningsPack"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_LightningsPack"));
			}

			ImGui::SeparatorText("");
		}


		// 인트로 보스용 이펙트 메쉬 생성
		if (ImGui::CollapsingHeader(" Mesh_VampireCommander "))
		{
			//ImGui::SeparatorText("VampireCommander");
			if (ImGui::Button("Demo_BeastSkull"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_BeastSkull"));
			}ImGui::SameLine();
			if (ImGui::Button("Demo_Impact"))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Impact"));
			}
			if (ImGui::Button(" Add Bat Test "))
			{
				Add_Part_Mesh_Morph(TEXT("Prototype_Component_Model_Effect_BatStorm_01"), TEXT("Prototype_Component_Model_Effect_BatStorm_02"));
			}
			ImGui::SameLine();
			if (ImGui::Button(" Add Torch Test "))
			{
				Add_Part_Mesh(TEXT("Prototype_Component_Model_Effect_Torch"));
			}

			//// 두두두두 불 이펙트 테스트
			//if (ImGui::Button(" Tick Effect Test "))
			//{
			//	//EFFECT_MANAGER->Tick_Create_Effect();
			//}

			ImGui::SeparatorText("");
		}


		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurVoidDesc->eType_Effect;

			if (CEffect_Void::MESH == eType_Effect)
			{
#pragma region Desc 얻어오기 업데이트_메쉬
				m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();												// 이펙트_보이드 구조체
				m_pInstanceDesc = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_InstanceDesc();	// 이펙트_인스턴스 구조체
				CVIBuffer_Effect_Model_Instance* pVIBuffer = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_VIBufferCom();	// 이펙트 모델 인스턴스 VIBuffer객체 얻어오기
				m_pMeshBufferDesc = pVIBuffer->Get_Desc();	// 버퍼의 구조체 얻어오기
#pragma endregion

				/* 이름 */
				ImGui::SeparatorText(" NAME ");
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pCurVoidDesc->strPartTag));


				/* 인스턴스 개수 변경 */
				ImGui::SeparatorText(" Instance Count ");
				ImGui::Text("MaxInstance_Mesh : %d", m_iMaxNumInstance_Mesh);
				if (ImGui::DragInt("Instance Count", &m_iNumInstance_Mesh, 1, 1, m_iMaxNumInstance_Mesh))
				{
					m_pCurVoidDesc->iCurNumInstance = m_iNumInstance_Mesh;
					m_pMeshBufferDesc->iCurNumInstance = m_iNumInstance_Mesh;
				}


				/* 텍스처 변경 */
				// 디퓨즈_메쉬 텍스처 =====================================================================================================
				if (ImGui::CollapsingHeader(" Diffuse_Tex_MESH "))
				{
					/* 모델 텍스처를 쓸건지, 내가 텍스처를 정해줄건지 */
					if (ImGui::Button("Use CustomTex"))
					{
						m_pInstanceDesc->bUseCustomTex = TRUE;
					}ImGui::SameLine();
					if (ImGui::Button("Use ModelTex "))
					{
						m_pInstanceDesc->bUseCustomTex = FALSE;
					}
					ImGui::SeparatorText("");

					if (ImGui::Button("Diffuse_Base"))	// 베이스 디퓨즈로 변경
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Diffuse"));
						m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 13;
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 0;
						

					}ImGui::SameLine();
					if (ImGui::Button("Diffuse_Waves"))	// 웨이브 디퓨즈로 변경
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Diffuse_Waves"));
						m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 6;
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 0;
					}


					if (ImGui::Button(" Remove Diffuse_Mesh "))
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Remove_TextureCom(CEffect_Void::TEXTURE_DIFFUSE);
					}

					if (ImGui::InputInt("Diffuse_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE], 1))
					{
						if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] - 1;

						if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE];
					}


					/* 디퓨즈 색상혼합 모드_Mesh */
					if (ImGui::CollapsingHeader(" Color Blend_Mesh "))
					{
						ImGui::RadioButton(u8"곱하기_Mesh", &m_iColor_Mode_Mesh, 0);
						ImGui::RadioButton(u8"스크린_Mesh", &m_iColor_Mode_Mesh, 1);
						ImGui::RadioButton(u8"오버레이_Mesh", &m_iColor_Mode_Mesh, 2);
						ImGui::RadioButton(u8"더하기_Mesh", &m_iColor_Mode_Mesh, 3);
						ImGui::RadioButton(u8"번_Mesh", &m_iColor_Mode_Mesh, 4);
						ImGui::RadioButton(u8"혼합안함_Mesh", &m_iColor_Mode_Mesh, 5);
						if (0 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::MUL;
						else if (1 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::SCREEN;
						else if (2 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::OVERLAY;
						else if (3 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::ADD;
						else if (4 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::BURN;
						else if (5 == m_iColor_Mode_Mesh)
							m_pCurVoidDesc->eMode_Color = MODE_COLOR::MODE_COLOR_END;


						// 디퓨즈에 곱할 색 (색 변경)
						if (ImGui::ColorEdit4("Color_Mul_Mesh", m_fColor_Mul_Mesh, ImGuiColorEditFlags_None))
						{
							m_pCurVoidDesc->vColor_Mul.x = m_fColor_Mul_Mesh[0];
							m_pCurVoidDesc->vColor_Mul.y = m_fColor_Mul_Mesh[1];
							m_pCurVoidDesc->vColor_Mul.z = m_fColor_Mul_Mesh[2];
							m_pCurVoidDesc->vColor_Mul.w = m_fColor_Mul_Mesh[3];
						}

						ImGui::SeparatorText("");
					}
				
					ImGui::SeparatorText("");
				}
				// 디퓨즈_메쉬 텍스처 =====================================================================================================


				// 마스크_메쉬 텍스처 =====================================================================================================
				if (ImGui::CollapsingHeader(" Mask_Tex_MESH "))
				{
					if (ImGui::Button("Mask_Base_Mesh"))	// 베이스 마스크로 변경
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask"));
						m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 164;
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 0;

					}ImGui::SameLine();
					if (ImGui::Button("Mask_Waves_Mesh"))	// 웨이브 마스크로 변경
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Mask_Waves"));
						m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 6;
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 0;
					}

					if (ImGui::Button("Remove_Mask_Mesh"))	// 마스크 텍스처 컴포넌트 삭제
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Remove_TextureCom(CEffect_Void::TEXTURE_MASK);
					}

					if (ImGui::InputInt("Mask_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK], 1))
					{
						if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] - 1;

						if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK];
					}

					ImGui::SeparatorText("");
				}


				// 노이즈_메쉬 텍스처 =====================================================================================================			
				if (ImGui::CollapsingHeader(" Noise_Tex_MESH "))
				{
					if (ImGui::Button("Noise_Base_Mesh"))	// 베이스 노이즈로 변경
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Change_TextureCom(TEXT("Prototype_Component_Texture_Effect_Noise"));
						m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = 243;
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = 0;

					}

					if (ImGui::Button("Remove_Noise_Mesh"))	// 노이즈 텍스처 컴포넌트 삭제
					{
						dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Remove_TextureCom(CEffect_Void::TEXTURE_NOISE);
					}

					ImGui::SeparatorText(" Noise ");
					if (ImGui::InputInt("Noise_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE], 1))
					{
						if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] - 1;

						if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE])
							m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = 0;

						m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE];
					}

					ImGui::SeparatorText("");
				}


				/* UV 값 조절 */
				if (ImGui::CollapsingHeader(" UV Option_Mesh "))
				{
					if (ImGui::DragFloat2(" UV_Offset ", m_fUV_Offset_Mesh, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Offset.x = m_fUV_Offset_Mesh[0];
						m_pCurVoidDesc->vUV_Offset.y = m_fUV_Offset_Mesh[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 이동");

					if (ImGui::DragFloat2(" UV_Scale ", m_vUV_Scale_Mesh, 1.f, 0.f, 100.f))
					{
						m_pCurVoidDesc->vUV_Scale.x = m_vUV_Scale_Mesh[0];
						m_pCurVoidDesc->vUV_Scale.y = m_vUV_Scale_Mesh[1];
					}ImGui::SameLine();
					HelpMarker(u8"UV 크기(타일링)");

					if (ImGui::DragFloat(" RotDegree_Mesh ", &m_fUV_RotDegree_Mesh, 1.f, 0.f, 360.f))
					{
						m_pCurVoidDesc->fUV_RotDegree = m_fUV_RotDegree_Mesh;
					}ImGui::SameLine();
					HelpMarker(u8"회전");

					ImGui::SeparatorText("");
				}


				/* 쉐이더 패스 인덱스 변경 */
				if (ImGui::CollapsingHeader(" Shader & Render _MESH "))
				{
					if (ImGui::InputInt("Shader Pass_Mesh", &m_iShaderPassIndex_Mesh, 1))
					{
						if (m_iMaxShaderPassIndex_Mesh < m_iShaderPassIndex_Mesh)
							m_iShaderPassIndex_Mesh = m_iMaxShaderPassIndex_Mesh;

						if (0 > m_iShaderPassIndex_Mesh)
							m_iShaderPassIndex_Mesh = 0;

						m_pCurVoidDesc->iShaderPassIndex = m_iShaderPassIndex_Mesh;
					}

					/* 쉐이더에 던질 디스카드 값 변경 */
					if (ImGui::DragFloat4("Discard_Mesh", m_vColor_Clip_Part, 0.1f, 0.f, 1.f))
					{
						m_pCurVoidDesc->vColor_Clip.x = m_vColor_Clip_Part[0];
						m_pCurVoidDesc->vColor_Clip.y = m_vColor_Clip_Part[1];
						m_pCurVoidDesc->vColor_Clip.z = m_vColor_Clip_Part[2];
						m_pCurVoidDesc->vColor_Clip.w = m_vColor_Clip_Part[3];
					}


					/* 렌더그룹 변경 */
					ImGui::SeparatorText("Render Group_MESH");
					if (ImGui::InputInt("Render Group_Mesh", &m_iRenderGroup_Mesh, 1))
					{
						if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Mesh)
						{
							m_iRenderGroup_Mesh = (_int)CRenderer::RENDER_END - 1;
						}
						m_pCurVoidDesc->iRenderGroup = m_iRenderGroup_Mesh;
					}

					ImGui::SeparatorText("");
				}


				/* 디졸브 값 확인 */
				if (ImGui::CollapsingHeader(" Dissolve_Mesh "))
				{
					ImGui::SeparatorText(" Dissolve_Mesh ");
					ImGui::SliderFloat(" DissolveAmount ", &m_pCurVoidDesc->fDissolveAmount, 0.f, 1.f);
					ImGui::SameLine();
					HelpMarker(u8"마스크:1/노이즈:5/쉐이더패스:2");

					if (ImGui::CollapsingHeader(" Dissolve_Easing Types "))
					{
						Select_EasingType(&m_pCurVoidDesc->eType_Easing);
					}

					ImGui::SeparatorText("");
				}


				if (ImGui::CollapsingHeader(" Morp_Mesh (Bat Test) "))
				{
					/* 모델 바꿔끼기 */
					if (ImGui::Button("ON Morph"))
					{
						m_pMeshBufferDesc->bMorph = TRUE;

					}ImGui::SameLine();
					if (ImGui::Button("OFF Morph"))
					{
						m_pMeshBufferDesc->bMorph = FALSE;
					}

					/* 모델 바꿔끼는 시간 텀 조정*/
					if (ImGui::DragFloat("MorphTimeTerm", &m_fMorphTimeTerm, 0.01f, 0.f, 360.f))
					{
						if (0.f > m_fMorphTimeTerm)
							m_fMorphTimeTerm = 0.f;

						m_pMeshBufferDesc->fMorphTimeTerm = m_fMorphTimeTerm;

					}

					ImGui::SeparatorText("");
				}


				// 메쉬 모드 선택 : MODE_STATIC, MODE_PARTICLE, MODE_END
				ImGui::SeparatorText(" Mode_Mesh ");
				ImGui::RadioButton("STATIC_Mode_Mesh ", &m_iType_Mode_Mesh, 0);
				ImGui::RadioButton("PARTICLE_Mesh", &m_iType_Mode_Mesh, 1);

				if (0 == m_iType_Mode_Mesh)
					m_pMeshBufferDesc->eType_Mode = CVIBuffer_Effect_Model_Instance::MODE_STATIC;
				else if (1 == m_iType_Mode_Mesh)
					m_pMeshBufferDesc->eType_Mode = CVIBuffer_Effect_Model_Instance::MODE_PARTICLE;



				if (ImGui::CollapsingHeader(" Particle Option_Mesh "))
				{
					/* 라이프 타임 */
					ImGui::SeparatorText(" LifeTimes ");
					if (ImGui::DragFloat2("MinMaxLifeTime_Mesh", m_vMinMaxLifeTime_Mesh, 1.f, 0.f, 360.f))
					{
						if (m_vMinMaxLifeTime_Mesh[0] > m_vMinMaxLifeTime_Mesh[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vMinMaxLifeTime_Mesh[1] = m_vMinMaxLifeTime_Mesh[0];


						m_pMeshBufferDesc->vMinMaxLifeTime.x = m_vMinMaxLifeTime_Mesh[0];
						m_pMeshBufferDesc->vMinMaxLifeTime.y = m_vMinMaxLifeTime_Mesh[1];
					}


					/* 퍼지는 범위 */
					ImGui::SeparatorText(" Range ");
					if (ImGui::DragFloat2("MinMaxRange_Mesh", m_vMinMaxRange_Mesh, 1.f, 0.1f, 360.f))
					{
						if (m_vMinMaxRange_Mesh[0] > m_vMinMaxRange_Mesh[1])
							m_vMinMaxRange_Mesh[0] = m_vMinMaxRange_Mesh[1];

						m_pMeshBufferDesc->vMinMaxRange.x = m_vMinMaxRange_Mesh[0];
						m_pMeshBufferDesc->vMinMaxRange.y = m_vMinMaxRange_Mesh[1];
					}


					/* 스피드 조절(러프 관련) */
					ImGui::SeparatorText(" Speed ");
					if (ImGui::DragFloat2("vMinMaxSpeed_Mesh", m_vMinMaxSpeed_Mesh, 0.f, 1.f, 1000.f))
					{
						if (0.f > m_vMinMaxSpeed_Mesh[0])	// Min이 0보다 작아지면 0으로 고정
							m_vMinMaxSpeed_Mesh[0] = 0.f;

						if (m_vMinMaxSpeed_Mesh[0] > m_vMinMaxSpeed_Mesh[1])	// Min이 Max보다 크면 Max를 Min으로
							m_vMinMaxSpeed_Mesh[1] = m_vMinMaxSpeed_Mesh[0];


						m_pMeshBufferDesc->vMinMaxSpeed.x = m_vMinMaxSpeed_Mesh[0];
						m_pMeshBufferDesc->vMinMaxSpeed.y = m_vMinMaxSpeed_Mesh[1];
					}

					if (ImGui::CollapsingHeader(" Speed_Easing Types_Mesh "))
					{
						Select_EasingType(&m_pMeshBufferDesc->eType_SpeedLerp);
					}


					/* 메쉬 파티클 리사이클 */
					ImGui::SeparatorText(" Recycle_Mesh ");
					ImGui::RadioButton("Recycle_Mesh", &m_iRecycle_Mesh, 0);  ImGui::SameLine();
					ImGui::RadioButton("None Recycle_Mesh", &m_iRecycle_Mesh, 1);
					if (0 == m_iRecycle_Mesh)
						m_pMeshBufferDesc->bRecycle = TRUE;
					else if (1 == m_iRecycle_Mesh)
						m_pMeshBufferDesc->bRecycle = FALSE;



					/* 파티클 액션 타입 */
					ImGui::SeparatorText(" Action Type_Mesh ");
					ImGui::RadioButton("SPARK_Mesh ", &m_iType_Action_Mesh, 0);  
					ImGui::RadioButton("BLINK_Mesh", &m_iType_Action_Mesh, 1);
					ImGui::RadioButton("FALL_Mesh", &m_iType_Action_Mesh, 2);	
					ImGui::RadioButton("RISE_Mesh", &m_iType_Action_Mesh, 3);
					ImGui::RadioButton("TORNADO_Mesh", &m_iType_Action_Mesh, 4);
					if (0 == m_iType_Action_Mesh)
						m_pMeshBufferDesc->eType_Action = CVIBuffer_Effect_Model_Instance::SPARK;
					else if (1 == m_iType_Action_Mesh)
						m_pMeshBufferDesc->eType_Action = CVIBuffer_Effect_Model_Instance::BLINK;
					else if (2 == m_iType_Action_Mesh)
						m_pMeshBufferDesc->eType_Action = CVIBuffer_Effect_Model_Instance::FALL;
					else if (3 == m_iType_Action_Mesh)
						m_pMeshBufferDesc->eType_Action = CVIBuffer_Effect_Model_Instance::RISE;
					else if (4 == m_iType_Action_Mesh)
						m_pMeshBufferDesc->eType_Action = CVIBuffer_Effect_Model_Instance::TORNADO;



					/* 센터 포지션 오프셋 */
					/* MinCenterOffsetPos */
					if (ImGui::DragFloat3("MinCenterOffsetPos_Mesh", m_vMinCenterOffsetPos_Mesh, 0.1f, -500.f, 500.f))
					{
						m_pMeshBufferDesc->vMinCenterOffsetPos.x = m_vMinCenterOffsetPos_Mesh[0];
						m_pMeshBufferDesc->vMinCenterOffsetPos.y = m_vMinCenterOffsetPos_Mesh[1];
						m_pMeshBufferDesc->vMinCenterOffsetPos.z = m_vMinCenterOffsetPos_Mesh[2];
					}
					/* MaxCenterOffsetPos */
					if (ImGui::DragFloat3("MaxCenterOffsetPos_Mesh", m_vMaxCenterOffsetPos_Mesh, 0.1f, -500.f, 500.f))
					{
						m_pMeshBufferDesc->vMaxCenterOffsetPos.x = m_vMaxCenterOffsetPos_Mesh[0];
						m_pMeshBufferDesc->vMaxCenterOffsetPos.y = m_vMaxCenterOffsetPos_Mesh[1];
						m_pMeshBufferDesc->vMaxCenterOffsetPos.z = m_vMaxCenterOffsetPos_Mesh[2];
					}




#pragma region 크기 변경 러프_메쉬 파티클 시작
					if (ImGui::CollapsingHeader(" Scale_Mesh "))
					{
						if (1 == m_iUseScaleLerp_Mesh)
						{
							// 크기러프 안쓸 때만 정비율 랜덤인지 선택_메쉬 파티클
							ImGui::SeparatorText(" ScaleRatio_Mesh ");
							ImGui::RadioButton("ScaleRatio_Mesh", &m_iScaleRatio_Mesh, 0); 
							ImGui::RadioButton("None_ScaleRatio_Mesh", &m_iScaleRatio_Mesh, 1);
							if (0 == m_iScaleRatio_Mesh)
								m_pMeshBufferDesc->bScaleRatio = TRUE;
							else if (1 == m_iScaleRatio_Mesh)
								m_pMeshBufferDesc->bScaleRatio = FALSE;
						}

						ImGui::SeparatorText("Start Scale_Mesh");
						if (ImGui::DragFloat3("Start Scale_Mesh", m_vStartScale_Mesh, 0.1f, 0.f, 5000.f))
						{
							m_pMeshBufferDesc->vStartScale.x = m_vStartScale_Mesh[0];
							m_pMeshBufferDesc->vStartScale.y = m_vStartScale_Mesh[1];
							m_pMeshBufferDesc->vStartScale.z = m_vStartScale_Mesh[2];
						}

						ImGui::SeparatorText("End Scale_Mesh");
						if (ImGui::DragFloat3("End Scale_Mesh", m_vEndScale_Mesh, 0.1f, 0.f, 5000.f))
						{
							m_pMeshBufferDesc->vEndScale.x = m_vEndScale_Mesh[0];
							m_pMeshBufferDesc->vEndScale.y = m_vEndScale_Mesh[1];
							m_pMeshBufferDesc->vEndScale.z = m_vEndScale_Mesh[2];
						}


						/* 메쉬 파티클 크기 변경 러프 사용 */
						ImGui::SeparatorText(" Use_ScaleLerp _Mesh ");
						ImGui::RadioButton("Use_ScaleLerp_Mesh", &m_iUseScaleLerp_Mesh, 0);  ImGui::SameLine();
						ImGui::RadioButton("None_ScaleLerp_Mesh", &m_iUseScaleLerp_Mesh, 1);
						if (0 == m_iUseScaleLerp_Mesh)
							m_pMeshBufferDesc->bUseScaleLerp = TRUE;
						else if (1 == m_iUseScaleLerp_Mesh)
							m_pMeshBufferDesc->bUseScaleLerp = FALSE;

						if (TRUE == m_pMeshBufferDesc->bUseScaleLerp)
						{
							ImGui::SeparatorText("ScaleLerp Pos_Mesh");
							if (ImGui::DragFloat2("Up ScaleLerp Pos_Mesh", m_vScaleLerp_Up_Pos_Mesh, 0.1f, 0.f, 1.f))
							{
								if (m_vScaleLerp_Up_Pos_Mesh[0] > m_vScaleLerp_Up_Pos_Mesh[1])	// Min이 Max보다 크면 Max를 Min으로
									m_vScaleLerp_Up_Pos_Mesh[1] = m_vScaleLerp_Up_Pos_Mesh[0];

								m_pMeshBufferDesc->vScaleLerp_Up_Pos.x = m_vScaleLerp_Up_Pos_Mesh[0];
								m_pMeshBufferDesc->vScaleLerp_Up_Pos.y = m_vScaleLerp_Up_Pos_Mesh[1];
							}

							if (ImGui::DragFloat2("Down ScaleLerp Pos_Mesh", m_vScaleLerp_Down_Pos_Mesh, 0.1f, 0.f, 1.f))
							{
								if (m_vScaleLerp_Down_Pos_Mesh[0] > m_vScaleLerp_Down_Pos_Mesh[1])	// Min이 Max보다 크면 Max를 Min으로
									m_vScaleLerp_Down_Pos_Mesh[1] = m_vScaleLerp_Down_Pos_Mesh[0];

								m_pMeshBufferDesc->vScaleLerp_Down_Pos.x = m_vScaleLerp_Down_Pos_Mesh[0];
								m_pMeshBufferDesc->vScaleLerp_Down_Pos.y = 1.f; // 1로 고정
							}

							if (ImGui::CollapsingHeader(" Scale_Easing Types "))
							{
								Select_EasingType(&m_pMeshBufferDesc->eType_ScaleLerp);
							}
						}

					}
#pragma endregion 크기 변경 러프_메쉬 파티클 끝


#pragma region 자체 회전_메쉬 파티클 시작
					if (ImGui::CollapsingHeader(" Rotation_Mesh "))
					{
						if (ImGui::DragFloat(" Rotation_Radian_X_Mesh ", &m_vRadian_Mesh[0], 1.f, 0.f, 360.f))
							m_pMeshBufferDesc->vRadian.x = m_vRadian_Mesh[0];

						if (ImGui::DragFloat(" Rotation_Radian_Y_Mesh ", &m_vRadian_Mesh[1], 1.f, 0.f, 360.f))
							m_pMeshBufferDesc->vRadian.y = m_vRadian_Mesh[1];

						if (ImGui::DragFloat(" Rotation_Radian_Z_Mesh ", &m_vRadian_Mesh[2], 1.f, 0.f, 360.f))
							m_pMeshBufferDesc->vRadian.z = m_vRadian_Mesh[2];

						ImGui::SeparatorText("");
					}
#pragma endregion 자체 회전_메쉬 파티클 끝


#pragma region Dir 회전 범위 오프셋_메쉬 파티클 시작
					if (ImGui::CollapsingHeader(" Dir Rotation Offset_Mesh "))
					{
						/* 회전 범위(오프셋) */
						/* RotX */
						if (ImGui::DragFloat2("RotationX_Mesh", m_vRotationOffsetX_Mesh, 1.f, 0.f, 360.f))
						{
							if (0 > m_vRotationOffsetX_Mesh[0])
								m_vRotationOffsetX_Mesh[0] = 0.f;

							if (m_vRotationOffsetX_Mesh[0] > m_vRotationOffsetX_Mesh[1])
								m_vRotationOffsetX_Mesh[1] = m_vRotationOffsetX_Mesh[0];

							m_pMeshBufferDesc->vMinMaxRotationOffsetX.x = m_vRotationOffsetX_Mesh[0];
							m_pMeshBufferDesc->vMinMaxRotationOffsetX.y = m_vRotationOffsetX_Mesh[1];
						}

						/* RotY */
						if (ImGui::DragFloat2("RotationY_Mesh", m_vRotationOffsetY_Mesh, 1.f, 0.f, 360.f))
						{
							if (0 > m_vRotationOffsetY_Mesh[0])
								m_vRotationOffsetY_Mesh[0] = 0.f;

							if (m_vRotationOffsetY_Mesh[0] > m_vRotationOffsetY_Mesh[1])
								m_vRotationOffsetY_Mesh[1] = m_vRotationOffsetY_Mesh[0];

							m_pMeshBufferDesc->vMinMaxRotationOffsetY.x = m_vRotationOffsetY_Mesh[0];
							m_pMeshBufferDesc->vMinMaxRotationOffsetY.y = m_vRotationOffsetY_Mesh[1];
						}

						/* RotZ */
						if (ImGui::DragFloat2("RotationZ_Mesh", m_vRotationOffsetZ_Mesh, 1.f, 0.f, 360.f))
						{
							if (0 > m_vRotationOffsetZ_Mesh[0])
								m_vRotationOffsetZ_Mesh[0] = 0.f;

							if (m_vRotationOffsetZ_Mesh[0] > m_vRotationOffsetZ_Mesh[1])
								m_vRotationOffsetZ_Mesh[1] = m_vRotationOffsetZ_Mesh[0];

							m_pMeshBufferDesc->vMinMaxRotationOffsetZ.x = m_vRotationOffsetZ_Mesh[0];
							m_pMeshBufferDesc->vMinMaxRotationOffsetZ.y = m_vRotationOffsetZ_Mesh[1];
						}

						ImGui::SeparatorText("");
					}
				}
#pragma endregion Dir 회전 범위 오프셋_메쉬 파티클 끝




				/* 리지드바디 키고 끄기_Mesh */
				ImGui::SeparatorText(" RigidBody_Mesh ");
				ImGui::RadioButton(" ON  RigidBody_Mesh ", &m_iUseRigidBody_Mesh, 0);
				ImGui::RadioButton(" OFF RigidBody_Mesh ", &m_iUseRigidBody_Mesh, 1);
				if (0 == m_iUseRigidBody_Mesh)
				{
					m_pCurVoidDesc->bUseRigidBody = TRUE;
					m_pMeshBufferDesc->bUseRigidBody = TRUE;
				}
				else if (1 == m_iUseRigidBody_Mesh)
				{
					m_pCurVoidDesc->bUseRigidBody = FALSE;
					m_pMeshBufferDesc->bUseRigidBody = FALSE;
				}

#pragma region 리지드바디 옵션 조정_메쉬 파티클

				if (ImGui::CollapsingHeader(" RigidBody Option_Mesh "))
				{
					/* 키네틱 키고 끄기 */
					ImGui::RadioButton(" Kinetic_Mesh ", &m_iKinetic_Mesh, 0);  ImGui::SameLine();
					ImGui::RadioButton(" Kinematic_Mesh ", &m_iKinetic_Mesh, 1);
					if (0 == m_iKinetic_Mesh)
					{
						m_pMeshBufferDesc->bKinetic = TRUE;
					}
					else if (1 == m_iKinetic_Mesh)
					{
						m_pMeshBufferDesc->bKinetic = FALSE;
					}

					/* 중력 키고 끄기 */
					ImGui::SeparatorText(" Gravity_Mesh ");
					ImGui::RadioButton(" ON   Gravity_Mesh ", &m_iUseGravity_Mesh, 0);  ImGui::SameLine();
					ImGui::RadioButton(" OFF  Gravity_Mesh ", &m_iUseGravity_Mesh, 1);
					if (0 == m_iUseGravity_Mesh)
					{
						m_pMeshBufferDesc->bUseGravity = TRUE;
					}
					else if (1 == m_iUseGravity_Mesh)
					{
						m_pMeshBufferDesc->bUseGravity = FALSE;
					}

					/* 중력 가속도 조절 */
					if (0 == m_iUseGravity_Mesh)
					{
						if (ImGui::DragFloat("Gravity_Mesh", &m_fGravity_Mesh, 0.5f, -100.f, 1000.f))
						{
							m_pMeshBufferDesc->fGravity = m_fGravity_Mesh;
						}
					}


					if (ImGui::Button(" FORCE "))
					{
						m_pMeshBufferDesc->eForce_Mode = FORCE_MODE::FORCE;
					}
					ImGui::SameLine();
					if (ImGui::Button(" IMPULSE "))
					{
						m_pMeshBufferDesc->eForce_Mode = FORCE_MODE::IMPULSE;
					}
					ImGui::SameLine();
					if (ImGui::Button(" ACCELERATION "))
					{
						m_pMeshBufferDesc->eForce_Mode = FORCE_MODE::ACCELERATION;
					}
					ImGui::SameLine();
					if (ImGui::Button(" VELOCITY_CHANGE "))
					{
						m_pMeshBufferDesc->eForce_Mode = FORCE_MODE::VELOCITY_CHANGE;
					}

					ImGui::SeparatorText("Power_Mesh");
					if (ImGui::DragFloat2("Power_Mesh", m_vMinMaxPower_Mesh, 10.f, 0.1f))
					{
						m_pMeshBufferDesc->vMinMaxPower.x = m_vMinMaxPower_Mesh[0];
						m_pMeshBufferDesc->vMinMaxPower.y = m_vMinMaxPower_Mesh[1];
					}


					ImGui::SeparatorText("");
				}
#pragma endregion 리지드바디 옵션 조정_메쉬 파티클 끝

		
				/* Rim & Bloom */
				if (ImGui::CollapsingHeader(" Rim Bloom_Mesh "))
				{
					ImGui::SeparatorText("RimLight_Mesh");
					if (ImGui::ColorEdit4("RimColor", m_fRimColor_Mesh, ImGuiColorEditFlags_None))
					{
						m_pCurVoidDesc->vRimColor.x = m_fRimColor_Mesh[0];
						m_pCurVoidDesc->vRimColor.y = m_fRimColor_Mesh[1];
						m_pCurVoidDesc->vRimColor.z = m_fRimColor_Mesh[2];
						m_pCurVoidDesc->vRimColor.w = m_fRimColor_Mesh[3];
					}
					if (ImGui::DragFloat("RimPower", &m_fRimPower_Mesh, 1.f, 0.f, 5000.f))
					{
						m_pCurVoidDesc->fRimPower = m_fRimPower_Mesh;
					}

					ImGui::SeparatorText("Bloom_Mesh");
					if (ImGui::ColorEdit3("BloomPower", m_vBloomPower_Mesh, ImGuiColorEditFlags_None))
					{
						m_pCurVoidDesc->vBloomPower.x = m_vBloomPower_Mesh[0];
						m_pCurVoidDesc->vBloomPower.y = m_vBloomPower_Mesh[1];
						m_pCurVoidDesc->vBloomPower.z = m_vBloomPower_Mesh[2];
					}

					ImGui::SeparatorText("");
				}


				// 디스토션 값 변경
				if (ImGui::CollapsingHeader(" Distortion_Mesh "))
				{
					CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_Distortion_Desc();
					if (ImGui::CollapsingHeader(" Distortion Preset "))
					{
						if (ImGui::Button("ZERO"))
						{
							pDistortionDesc->eType_Scroll = { CEffect_Void::TYPE_SCROLL_END };

							pDistortionDesc->vScrollSpeeds = { 0.f, 0.f, 0.f };
							pDistortionDesc->vScales = { 0.f, 0.f, 0.f };

							pDistortionDesc->vDistortion1 = { 0.f, 0.f };
							pDistortionDesc->vDistortion2 = { 0.f, 0.f };
							pDistortionDesc->vDistortion3 = { 0.f, 0.f };

							pDistortionDesc->fDistortionScale = { 0.f };
							pDistortionDesc->fDistortionBias = { 0.f };

							Update_CurParameters_Parts();
						}

						if (ImGui::Button("FIRE"))
						{
							pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_COL };

							pDistortionDesc->vScrollSpeeds = { 1.f, 1.f, 1.f };
							pDistortionDesc->vScales = { 1.f, 1.f, 1.f };

							pDistortionDesc->vDistortion1 = { 0.1f, 0.1f };
							pDistortionDesc->vDistortion2 = { 0.f, 0.f };
							pDistortionDesc->vDistortion3 = { 0.f, 0.1f };

							pDistortionDesc->fDistortionScale = { 1.f };
							pDistortionDesc->fDistortionBias = { 1.f };

							Update_CurParameters_Parts();
						}

						ImGui::SeparatorText("");
					}
					
					/* 디스토션 스크롤 방향 변경 */
					ImGui::RadioButton(" Row_Scroll_Mesh ", &m_iType_Scroll_Mesh, 0);  ImGui::SameLine();
					ImGui::RadioButton("Col_Scroll_Mesh", &m_iType_Scroll_Mesh, 1);
					ImGui::RadioButton("Both_Scroll_Mesh", &m_iType_Scroll_Mesh, 2);
					ImGui::RadioButton("Rotat_Scroll_Mesh", &m_iType_Scroll_Mesh, 3);
					ImGui::RadioButton("End_Scroll_Mesh", &m_iType_Scroll_Mesh, 4);
					if (0 == m_iType_Scroll_Mesh)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROW };
					else if (1 == m_iType_Scroll_Mesh)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_COL };
					else if (2 == m_iType_Scroll_Mesh)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_BOTH };
					else if (3 == m_iType_Scroll_Mesh)
						pDistortionDesc->eType_Scroll = { CEffect_Void::SCROLL_ROTAT };
					else if (4 == m_iType_Scroll_Mesh)
						pDistortionDesc->eType_Scroll = { CEffect_Void::TYPE_SCROLL_END };


					if (ImGui::DragFloat3("ScrollSpeeds", m_vScrollSpeeds_Mesh, 0.1f, -100.f))
					{
						pDistortionDesc->vScrollSpeeds.x = m_vScrollSpeeds_Mesh[0];
						pDistortionDesc->vScrollSpeeds.y = m_vScrollSpeeds_Mesh[1];
						pDistortionDesc->vScrollSpeeds.z = m_vScrollSpeeds_Mesh[2];
					}
					if (ImGui::DragFloat3("Distortion_Scales", m_vScales_Distortion_Mesh, 0.1f, 0.f))
					{
						pDistortionDesc->vScales.x = m_vScales_Distortion_Mesh[0];
						pDistortionDesc->vScales.y = m_vScales_Distortion_Mesh[1];
						pDistortionDesc->vScales.z = m_vScales_Distortion_Mesh[2];
					}
					if (ImGui::DragFloat2("Distortion1", m_vDistortion1_Mesh, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion1.x = m_vDistortion1_Mesh[0];
						pDistortionDesc->vDistortion1.y = m_vDistortion1_Mesh[1];
					}
					if (ImGui::DragFloat2("Distortion2", m_vDistortion2_Mesh, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion2.x = m_vDistortion2_Mesh[0];
						pDistortionDesc->vDistortion2.y = m_vDistortion2_Mesh[1];
					}
					if (ImGui::DragFloat2("Distortion3", m_vDistortion3_Mesh, 0.1f, 0.f))
					{
						pDistortionDesc->vDistortion3.x = m_vDistortion3_Mesh[0];
						pDistortionDesc->vDistortion3.y = m_vDistortion3_Mesh[1];
					}
					if (ImGui::DragFloat("Distortion_Scale", &m_fDistortionScale_Mesh, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionScale = m_fDistortionScale_Mesh;
					}
					if (ImGui::DragFloat("DistortionBias", &m_fDistortionBias_Mesh, 1.f, 0.f))
					{
						pDistortionDesc->fDistortionBias = m_fDistortionBias_Mesh;
					}

					ImGui::SeparatorText("");
				}

			

			}
		}
	}
}

void CWindow_EffectTool::Update_TrailTab()
{
	if (nullptr != m_pModel_Preview)
	{
		if (ImGui::CollapsingHeader(u8"로드 테스트 주의!!"))
		{
			if (ImGui::Button("Test Ready Trail"))
			{
				m_pPart_Preview->Ready_Trail(LEVEL_TOOL, "Test_Trail.json");
			}
			ImGui::SeparatorText("");
		}
	}



	// 트레일 추가
	if (nullptr == m_pTrail)
	{
		// 트레일 객체가 nullptr이면 트레일 생성 가능
		if (ImGui::Button(" Create Trail "))
		{
			Create_Trail();
		}
	}
	else
	{	
		// 트레일 객체가 nullptr이 아니면

		ImGui::SeparatorText("Save & Load Trail");
		if (ImGui::Button(" Save Trail "))
		{
			m_eFile = FILE_TRAIL;
			m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;
			m_strDialogPath = "../Bin/DataFiles/Data_Effect/Data_Trail/";

			OpenDialog(IMGUI_EFFECTTOOL_WINDOW);
		}
		ImGui::SameLine();
		if (ImGui::Button(" Load Trail "))
		{
			m_eFile = FILE_TRAIL;
			m_eDialogType = DIALOG_TYPE::LOAD_DIALOG;
			m_strDialogPath = "../Bin/DataFiles/Data_Effect/Data_Trail/";

			OpenDialog(IMGUI_EFFECTTOOL_WINDOW);
		}

		//m_pTrail->Tick_Trail()

		ImGui::SeparatorText(" Delete ");
		if (ImGui::Button(" Delete Trail "))
		{
			Delete_Trail();
	
			return;
		}

		// =============================================================================================================================================================
		
		if (ImGui::CollapsingHeader("Trail For Model"))	// 모델에 트레일 달아주기
		{
			if (nullptr != m_pModel_Preview)
			{
				// Model_Preview 디스크립션 얻어오기
				CModel_Preview::MODEL_PREVIEW_DESC* pDesc = dynamic_cast<CModel_Preview*>(m_pModel_Preview)->Get_Desc();

				// 애니메이션 변경 테스트 
				ImGui::SeparatorText("Model Animation");
				if (ImGui::Button("Idle"))
				{
					if (TEXT("Prototype_Component_Model_Rentier") == pDesc->strModelTag)
					{
						// 플레이어 아이들 
						m_pModel_Preview->Set_AnimIndex(CPlayer::Player_IdleLoop);
					}

					if (TEXT("Prototype_Component_Model_VampireCommander") == pDesc->strModelTag)
					{
						// 보스 아이들 // Index 9
						m_pModel_Preview->Set_AnimIndex(9);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Attack"))
				{
					if (TEXT("Prototype_Component_Model_Rentier") == pDesc->strModelTag)
					{
						// 플레이어 공격
						m_pModel_Preview->Set_AnimIndex(CPlayer::Player_Finisher_BanditHeavy_ZapperLeap);
					}

					if (TEXT("Prototype_Component_Model_VampireCommander") == pDesc->strModelTag)
					{
						// 보스 VampireCommander_AttackMelee_02 // Index 55
						m_pModel_Preview->Set_AnimIndex(55);
					}
				}

				/* 재생, 정지 */
				if (ImGui::Button("   Play   "))
				{
					m_pModel_Preview->Get_Desc()->bPlay = TRUE;
					m_pTrail->Set_Pause(FALSE);
				}
				ImGui::SameLine();
				if (ImGui::Button("   Stop   "))
				{
					m_pModel_Preview->Get_Desc()->bPlay = FALSE;
					m_pTrail->Set_Pause(TRUE);
				}


				// 현재 모델의 파트에 트레일 달아주기
				if (ImGui::Button("Set Trail_Part"))
				{
					m_pPart_Preview->Set_Trail(m_pTrail);
				}


			} // nullptr != m_pModel_Preview
		}
		

		if (ImGui::CollapsingHeader("Trail For Effect"))	// 이펙트에 트레일 달아주기
		{
			if (nullptr != m_pCurEffect)
			{
				// 이펙트에 트레일 달아주기
				if (ImGui::Button("Set Trail_Effect"))
				{
					m_pCurEffect->Set_Trail(m_pTrail);
				}


				/* 재생, 정지 */
				if (ImGui::Button("   Play   "))
				{
					m_pTrail->Set_Pause(FALSE);
				}
				ImGui::SameLine();
				if (ImGui::Button("   Stop   "))
				{
					m_pTrail->Set_Pause(TRUE);
				}



			}

		}


#pragma region Desc 얻어오기 업데이트_트레일
		// 트레일이 nullptr이 아닐 때 트레일 전용 Desc들 얻어오기
		m_pVoidTrailDesc = m_pTrail->Get_Desc();
		m_pTrailDesc = m_pTrail->Get_TrailDesc();
		CVIBuffer_Trail* pVIBuffer = m_pTrail->Get_VIBufferCom();
		m_pTrailBufferDesc = pVIBuffer->Get_Desc();
#pragma endregion 

		/* 이름 */
		ImGui::SeparatorText("");
		ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pVoidTrailDesc->strPartTag));


#pragma region 텍스처 번경_트레일
		/* 텍스처 변경 */
		ImGui::SeparatorText("TEXTURE_TRAIL");
		if (m_pVoidTrailDesc->bUseSpriteAnim)
		{
			if (ImGui::InputInt("Diffuse_Trail", &m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE], 1))
			{
				if (m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE] <= m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE])
					m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE] = m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE] - 1;

				if (0 > m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE])
					m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE] = 0;

				m_pVoidTrailDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = m_iTexIndex_Trail[CEffect_Void::TEXTURE_SPRITE];
			}
		}
		else
		{
			if (ImGui::InputInt("Diffuse_Trail", &m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE], 1))
			{
				if (m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE])
					m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE] - 1;

				if (0 > m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE])
					m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE] = 0;

				m_pVoidTrailDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Trail[CEffect_Void::TEXTURE_DIFFUSE];
			}
		}

		if (ImGui::InputInt("Mask_Trail", &m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK], 1))
		{
			if (m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK])
				m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_MASK] - 1;

			if (0 > m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK])
				m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK] = 0;

			m_pVoidTrailDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Trail[CEffect_Void::TEXTURE_MASK];
		}

		if (ImGui::InputInt("Noise_Trail", &m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE], 1))
		{
			if (m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE])
				m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Trail[CEffect_Void::TEXTURE_NOISE] - 1;

			if (0 > m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE])
				m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE] = 0;

			m_pVoidTrailDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Trail[CEffect_Void::TEXTURE_NOISE];
		}

#pragma endregion 텍스처 번경_트레일 끝


		/* 쉐이더 패스 인덱스 변경 */
		if (ImGui::InputInt("Shader Pass_Trail", &m_iShaderPassIndex_Trail, 1))
		{
			if (m_iMaxShaderPassIndex_Trail < m_iShaderPassIndex_Trail)
				m_iShaderPassIndex_Trail = m_iMaxShaderPassIndex_Trail;

			if (0 > m_iShaderPassIndex_Trail)
				m_iShaderPassIndex_Trail = 0;

			m_pVoidTrailDesc->iShaderPassIndex = m_iShaderPassIndex_Trail;
		}

		/* 쉐이더에 던질 디스카드 값 변경 */
		if (ImGui::DragFloat4("Discard_Trail", m_vColor_Clip_Trail, 0.1f, 0.f, 1.f))
		{
			m_pVoidTrailDesc->vColor_Clip.x = m_vColor_Clip_Trail[0];
			m_pVoidTrailDesc->vColor_Clip.y = m_vColor_Clip_Trail[1];
			m_pVoidTrailDesc->vColor_Clip.z = m_vColor_Clip_Trail[2];
			m_pVoidTrailDesc->vColor_Clip.w = m_vColor_Clip_Trail[3];
		}

		/* 렌더그룹 변경 */
		ImGui::SeparatorText("");
		if (ImGui::InputInt("Render Group_Trail", &m_iRenderGroup_Trail, 1))
		{
			if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Trail)
			{
				m_iRenderGroup_Trail = (_int)CRenderer::RENDER_END - 1;
			}
			m_pVoidTrailDesc->iRenderGroup = m_iRenderGroup_Trail;
		}

		/* 쉐이더에 던질 곱하기 컬러 값_트레일 */
		if (ImGui::ColorEdit4("Color_Mul_Trail", m_fColor_Mul_Trail, ImGuiColorEditFlags_None))
		{
			m_pVoidTrailDesc->vColor_Mul.x = m_fColor_Mul_Trail[0];
			m_pVoidTrailDesc->vColor_Mul.y = m_fColor_Mul_Trail[1];
			m_pVoidTrailDesc->vColor_Mul.z = m_fColor_Mul_Trail[2];
			m_pVoidTrailDesc->vColor_Mul.w = m_fColor_Mul_Trail[3];
		}


#pragma region 버퍼 Desc 값 조절_트레일
		ImGui::SeparatorText("Trail_Buffer");

		if (ImGui::DragFloat3("Pos_0", m_vPos_0, 0.1f, -100.f, 100.f))
		{
			m_pTrailBufferDesc->vPos_0.x = m_vPos_0[0];
			m_pTrailBufferDesc->vPos_0.y = m_vPos_0[1];
			m_pTrailBufferDesc->vPos_0.z = m_vPos_0[2];
		}

		if (ImGui::DragFloat3("Pos_1", m_vPos_1, 0.1f, -100.f, 100.f))
		{
			m_pTrailBufferDesc->vPos_1.x = m_vPos_1[0];
			m_pTrailBufferDesc->vPos_1.y = m_vPos_1[1];
			m_pTrailBufferDesc->vPos_1.z = m_vPos_1[2];
		}

		//if (ImGui::DragInt("iMaxCnt_Trail", &m_iMaxCnt_Trail, 1, 2.f, 100.f))
		//{
		//	m_pTrailBufferDesc->iMaxCnt = m_iMaxCnt_Trail;
		//}

		if (ImGui::DragInt("iLerpPointNum_Trail", &m_iLerpPointNum, 1, 2, 100))
		{
			m_pTrailBufferDesc->iLerpPointNum = m_iLerpPointNum;
		}

		ImGui::SeparatorText("Trail_Buffer_Preset");
		if (ImGui::Button("Boss_Trail_Preset"))
		{
			m_pTrailBufferDesc->vPos_0 = { 0.f, 0.f, -0.5f };
			m_pTrailBufferDesc->vPos_0 = { 0.f, 0.f, 0.5f };
			m_pTrailBufferDesc->iLerpPointNum = { 12 };
		}

#pragma endregion 버퍼 Desc 값 조절_트레일 끝


#pragma region Rim & Bloom 값 조절_트레일

		/* 림라이트 & 블룸 */
		ImGui::SeparatorText(" Rim Bloom_Trail");
		if (ImGui::ColorEdit4(" RimColor_Trail ", m_fRimColor_Trail, ImGuiColorEditFlags_None))
		{

			m_pVoidTrailDesc->vRimColor.x = m_fRimColor_Trail[0];
			m_pVoidTrailDesc->vRimColor.y = m_fRimColor_Trail[1];
			m_pVoidTrailDesc->vRimColor.z = m_fRimColor_Trail[2];
			m_pVoidTrailDesc->vRimColor.w = m_fRimColor_Trail[3];
		}
		if (ImGui::DragFloat(" RimPower_Trail ", &m_fRimPower_Trail, 1.f, 0.f, 5000.f))
		{
			m_pVoidTrailDesc->fRimPower = m_fRimPower_Trail;
		}

		if (ImGui::ColorEdit3("BloomPower_TrailPower", m_vBloomPower_Trail, ImGuiColorEditFlags_None))
		{
			m_pVoidTrailDesc->vBloomPower.x = m_vBloomPower_Trail[0];
			m_pVoidTrailDesc->vBloomPower.y = m_vBloomPower_Trail[1];
			m_pVoidTrailDesc->vBloomPower.z = m_vBloomPower_Trail[2];
		}



	} // nullptr != m_pTrail





}

void CWindow_EffectTool::Update_CurParameters_Parts()
{
	//! 객체의 실제 정보를 툴에서 확인할수 있도록
	//! 즉 툴의 멤버 = 객체의 멤버정보 대입

	if (nullptr != m_pCurPartEffect)
	{
		m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();	// 이펙트_보이드 Desc

		CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurVoidDesc->eType_Effect;
		CTransform* pPartTransform = m_pCurPartEffect->Get_Transform();

		m_vTimes_Part[0] = m_pCurVoidDesc->fWaitingTime;
		m_vTimes_Part[1] = m_pCurVoidDesc->fLifeTime;
		m_vTimes_Part[2] = m_pCurVoidDesc->fRemainTime;

		_float4 vPos = pPartTransform->Get_State(CTransform::STATE_POSITION);
		m_vWorldPosition_Part[0] = vPos.x;
		m_vWorldPosition_Part[1] = vPos.y;
		m_vWorldPosition_Part[2] = vPos.z;

		_float3 vScaled = pPartTransform->Get_Scaled();
		m_vScale_Part[0] = vScaled.x;
		m_vScale_Part[1] = vScaled.y;
		m_vScale_Part[2] = vScaled.z;

		_float3 vRotated = pPartTransform->Get_Rotated();
		m_vRotate_Part[0] = vRotated.x;
		m_vRotate_Part[1] = vRotated.y;
		m_vRotate_Part[2] = vRotated.z;


		if (CEffect_Void::PARTICLE == eType_Effect)
		{
#pragma region 파티클(+버퍼) 디스크립션 얻어오기 시작
			m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();	// 이펙트_보이드 Desc
			CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Distortion_Desc(); // 파티클 디스토션 구조체
			CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();	// 파티클 버퍼 얻어오기
			m_pParticleBufferDesc = pVIBuffer->Get_Desc(); // 버퍼의 Desc 얻어오기
#pragma endregion

			m_vTimes_Part[0] = m_pCurVoidDesc->fWaitingTime;
			m_vTimes_Part[1] = m_pCurVoidDesc->fLifeTime;
			m_vTimes_Part[2] = m_pCurVoidDesc->fRemainTime; // 리메인 타임??

			// 텍스처 업데이트 =============================================================================================================
			if (m_pCurVoidDesc->bUseSpriteAnim)
			{
				m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE];
			}
			else
			{
				m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE];
			}

			m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK];
			m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE];
			// 텍스처 업데이트 =============================================================================================================

			m_iShaderPassIndex_Particle = m_pCurVoidDesc->iShaderPassIndex;		// 쉐이더 패스 인덱스 업데이트

			m_iRenderGroup_Particle = m_pCurVoidDesc->iRenderGroup;				// 렌더그룹 업데이트

			m_iNumInstance_Particle = m_pParticleBufferDesc->iCurNumInstance;	// 인스턴스 개수 업데이트


			/* 빌보드 여부 */
			if (m_pCurVoidDesc->bBillBoard)
				m_iBillBoard = 0;
			else
				m_iBillBoard = 1;

			/* Z소팅 여부 */
			if (dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_SortZ())
				m_iSortZ = 0;
			else
				m_iSortZ = 1;

			/* 스프라이트 애니메이션 사용여부 */
			if (m_pCurVoidDesc->bUseSpriteAnim)
				m_iSprite_Particle = 1;
			else
				m_iSprite_Particle = 0;

			if (0 == m_iSprite_Particle)	// 스프라이트 애니메이션 사용이면
			{
				m_pSpriteDesc_Particle = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Sprite_Desc();

				m_vUV_MaxTileCount_Particle[0] = (_int)m_pSpriteDesc_Particle->vUV_MaxTileCount.x;
				m_vUV_MaxTileCount_Particle[1] = (_int)m_pSpriteDesc_Particle->vUV_MaxTileCount.y;

				m_fSequenceTerm_Particle = m_pSpriteDesc_Particle->fSequenceTerm;
			}


			/* 컬러 블렌딩 모드 업데이트 */
			if (MODE_COLOR::MUL == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 0;
			else if (MODE_COLOR::SCREEN == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 1;
			else if (MODE_COLOR::OVERLAY == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 2;
			else if (MODE_COLOR::ADD == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 3;
			else if (MODE_COLOR::BURN == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 4;
			else if (MODE_COLOR::MODE_COLOR_END == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Particle = 5;


			/* 쉐이더에 던져져서 곱해질 색상 값_파티클  */
			m_fColor_Mul_Particle[0] = m_pCurVoidDesc->vColor_Mul.x;
			m_fColor_Mul_Particle[1] = m_pCurVoidDesc->vColor_Mul.y;
			m_fColor_Mul_Particle[2] = m_pCurVoidDesc->vColor_Mul.z;
			m_fColor_Mul_Particle[3] = m_pCurVoidDesc->vColor_Mul.w;


			/* 쉐이더에 던져서 자를 값 */
			m_vColor_Clip_Part[0] = m_pCurVoidDesc->vColor_Clip.x;
			m_vColor_Clip_Part[1] = m_pCurVoidDesc->vColor_Clip.y;
			m_vColor_Clip_Part[2] = m_pCurVoidDesc->vColor_Clip.z;
			m_vColor_Clip_Part[3] = m_pCurVoidDesc->vColor_Clip.w;

			
			/* UV 옵션 */
			m_vUV_Offset_Particle[0] = m_pCurVoidDesc->vUV_Offset.x;
			m_vUV_Offset_Particle[1] = m_pCurVoidDesc->vUV_Offset.y;

			m_vUV_Scale_Particle[0] = m_pCurVoidDesc->vUV_Scale.x;
			m_vUV_Scale_Particle[1] = m_pCurVoidDesc->vUV_Scale.y;

			m_fUV_RotDegree_Particle = m_pCurVoidDesc->fUV_RotDegree;



			// 리지드바디 업데이트 =============================================================================================================
			
			/* 리지드바디 사용 여부 */
			if (m_pCurVoidDesc->bUseRigidBody)
				m_iUseRigidBody_Particle = 0;
			else
				m_iUseRigidBody_Particle = 1;
			
		
			if (0 == m_iUseRigidBody_Particle)	// 리지드바디 사용이면
			{
				/* 키네틱 여부 */
				if (m_pParticleBufferDesc->bKinetic)
					m_iKinetic_Particle = 0;
				else
					m_iKinetic_Particle = 1;

				/* 중력 사용 여부 */
				if (m_pParticleBufferDesc->bUseGravity)
					m_iUseGravity_Particle = 0;
				else
					m_iUseGravity_Particle = 1;


				m_fGravity_Particle = m_pParticleBufferDesc->fGravity;	// 중력 가속도

				//m_fFriction_Particle = m_pParticleBufferDesc->fFriction;	// 마찰 계수
				m_vFrictionLerp_Pos_Particle[0] = m_pParticleBufferDesc->vFrictionLerp_Pos.x;
				m_vFrictionLerp_Pos_Particle[1] = m_pParticleBufferDesc->vFrictionLerp_Pos.y;

				m_vStartEnd_Friction_Particle[0] = m_pParticleBufferDesc->vStartEnd_Friction.x;
				m_vStartEnd_Friction_Particle[1] = m_pParticleBufferDesc->vStartEnd_Friction.y;


				m_fSleepThreshold_Particle = m_pParticleBufferDesc->fSleepThreshold;	// 슬립 한계점


				/* 파워 */
				m_vMinMaxPower_Particle[0] = m_pParticleBufferDesc->vMinMaxPower.x;
				m_vMinMaxPower_Particle[1] = m_pParticleBufferDesc->vMinMaxPower.y;


				/* 질량(Mass) */
				m_vMinMaxMass_Particle[0] = m_pParticleBufferDesc->vMinMaxMass.x;
				m_vMinMaxMass_Particle[1] = m_pParticleBufferDesc->vMinMaxMass.y;

	

			}
			// 리지드바디 업데이트 =============================================================================================================


			/* 라이프타임_파티클 */
			m_vMinMaxLifeTime_Particle[0] = m_pParticleBufferDesc->vMinMaxLifeTime.x;
			m_vMinMaxLifeTime_Particle[1] = m_pParticleBufferDesc->vMinMaxLifeTime.y;


			/* 방출(Emitter)_파티클 */
			m_fEmissionTime_Particle = m_pParticleBufferDesc->fEmissionTime;
			m_iAddEmitCount_Particle = (_int)m_pParticleBufferDesc->iAddEmitCount;


			/* 퍼지는 범위(분포 범위) */
			m_vMinMaxRange_Particle[0] = m_pParticleBufferDesc->vMinMaxRange.x;
			m_vMinMaxRange_Particle[1] = m_pParticleBufferDesc->vMinMaxRange.y;

			m_fMinMaxAddRange_Particle[0] = m_pParticleBufferDesc->vMinMaxAddRange.x;
			m_fMinMaxAddRange_Particle[1] = m_pParticleBufferDesc->vMinMaxAddRange.y;

			/* 파티클이 올라갈 최고 높이 */
			m_vMinMaxPosY_Particle[0] = m_pParticleBufferDesc->vMinMaxPosY.x;
			m_vMinMaxPosY_Particle[1] = m_pParticleBufferDesc->vMinMaxPosY.y;


			/* 스피드 */
			m_vMinMaxSpeed_Particle[0] = m_pParticleBufferDesc->vMinMaxSpeed.x;
			m_vMinMaxSpeed_Particle[1] = m_pParticleBufferDesc->vMinMaxSpeed.y;


			/* 파티클 이동 회전 각도 */
			m_vMinMaxTheta_Particle[0] = m_pParticleBufferDesc->vMinMaxTheta.x;
			m_vMinMaxTheta_Particle[1] = m_pParticleBufferDesc->vMinMaxTheta.y;


			/* 리사이클 여부 */
			if (m_pParticleBufferDesc->bRecycle)
				m_iRecycle_Particle = 0;
			else
				m_iRecycle_Particle = 1;


			/* 진행방향 리버스 여부 */
			if (m_pParticleBufferDesc->bReverse)
				m_iReverse_Particle = 0;
			else
				m_iReverse_Particle = 1;


			/* 움직이는 모양 업데이트 */
			if (CVIBuffer_Particle::SPARK == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 0;
			else if(CVIBuffer_Particle::BLINK == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 1;
			else if (CVIBuffer_Particle::FALL == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 2;
			else if (CVIBuffer_Particle::RISE == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 3;
			else if (CVIBuffer_Particle::TORNADO == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 4;
			else if (CVIBuffer_Particle::TYPE_ACTION_END == m_pParticleBufferDesc->eType_Action)
				m_iType_Action_Particle = 5;


			/* 센터 오프셋 위치_파티클 */
			m_vMinCenterOffsetPos_Particle[0] = m_pParticleBufferDesc->vMinCenterOffsetPos.x;
			m_vMinCenterOffsetPos_Particle[1] = m_pParticleBufferDesc->vMinCenterOffsetPos.y;
			m_vMinCenterOffsetPos_Particle[2] = m_pParticleBufferDesc->vMinCenterOffsetPos.z;

			m_vMaxCenterOffsetPos_Particle[0] = m_pParticleBufferDesc->vMaxCenterOffsetPos.x;
			m_vMaxCenterOffsetPos_Particle[1] = m_pParticleBufferDesc->vMaxCenterOffsetPos.y;
			m_vMaxCenterOffsetPos_Particle[2] = m_pParticleBufferDesc->vMaxCenterOffsetPos.z;


			/* 자체 회전 */
			m_vRadian_Particle[0] = m_pParticleBufferDesc->vRadian.x;
			m_vRadian_Particle[1] = m_pParticleBufferDesc->vRadian.y;
			m_vRadian_Particle[2] = m_pParticleBufferDesc->vRadian.z;


			/* 회전 범위 */
			m_vRotationOffsetX_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetX.x;
			m_vRotationOffsetX_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetX.y;

			m_vRotationOffsetY_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetY.x;
			m_vRotationOffsetY_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetY.y;

			m_vRotationOffsetZ_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetZ.x;
			m_vRotationOffsetZ_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetZ.y;


			/* 스케일 러프 */
			if (TRUE == m_pParticleBufferDesc->bUseScaleLerp)
				m_iUseScaleLerp = 0;
			else if (FALSE == m_pParticleBufferDesc->bUseScaleLerp)
				m_iUseScaleLerp = 1;


			m_vScaleLerp_Up_Pos[0] = m_pParticleBufferDesc->vScaleLerp_Up_Pos.x;
			m_vScaleLerp_Up_Pos[1] = m_pParticleBufferDesc->vScaleLerp_Up_Pos.y;

			m_vScaleLerp_Down_Pos[0] = m_pParticleBufferDesc->vScaleLerp_Down_Pos.x;
			m_vScaleLerp_Down_Pos[1] = m_pParticleBufferDesc->vScaleLerp_Down_Pos.y;

			m_vMinMaxWidth_Particle[0] = m_pParticleBufferDesc->vMinMaxWidth.x;
			m_vMinMaxWidth_Particle[1] = m_pParticleBufferDesc->vMinMaxWidth.y;

			m_vMinMaxHeight_Particle[0] = m_pParticleBufferDesc->vMinMaxHeight.x;
			m_vMinMaxHeight_Particle[1] = m_pParticleBufferDesc->vMinMaxHeight.y;




			/* 색상 */
			/* 입자별 색상 변경 여부 */
			if (m_pParticleBufferDesc->bDynamic_Color)
				m_iDynamic_Color_Particle = 0;
			else
				m_iDynamic_Color_Particle = 1;

			m_fColor_Min_Particle[0] = m_pParticleBufferDesc->vMinMaxRed.x;
			m_fColor_Min_Particle[1] = m_pParticleBufferDesc->vMinMaxBlue.x;
			m_fColor_Min_Particle[2] = m_pParticleBufferDesc->vMinMaxGreen.x;
			m_fColor_Min_Particle[3] = m_pParticleBufferDesc->vMinMaxAlpha.x;

			m_fColor_Max_Particle[0] = m_pParticleBufferDesc->vMinMaxRed.y;
			m_fColor_Max_Particle[1] = m_pParticleBufferDesc->vMinMaxBlue.y;
			m_fColor_Max_Particle[2] = m_pParticleBufferDesc->vMinMaxGreen.y;
			m_fColor_Max_Particle[3] = m_pParticleBufferDesc->vMinMaxAlpha.y;

			if (0 == m_iDynamic_Color_Particle)
			{
				m_fColor_Cur_Particle[0] = m_pParticleBufferDesc->vCurrentColor.y;
				m_fColor_Cur_Particle[1] = m_pParticleBufferDesc->vCurrentColor.y;
				m_fColor_Cur_Particle[2] = m_pParticleBufferDesc->vCurrentColor.y;
				m_fColor_Cur_Particle[3] = m_pParticleBufferDesc->vCurrentColor.y;

			}


			/* 페이드 타입 업데이트 */
			if (CVIBuffer_Particle::FADE_NONE == m_pParticleBufferDesc->eType_Fade)
				m_iType_Fade_Particle = 0;
			else if (CVIBuffer_Particle::FADE_OUT == m_pParticleBufferDesc->eType_Fade)
				m_iType_Fade_Particle = 1;
			else if (CVIBuffer_Particle::FADE_IN == m_pParticleBufferDesc->eType_Fade)
				m_iType_Fade_Particle = 2;


			/* 림 블룸 값 업데이트 */
			m_vBloomPower_Particle[0] = m_pCurVoidDesc->vBloomPower.x;
			m_vBloomPower_Particle[1] = m_pCurVoidDesc->vBloomPower.y;
			m_vBloomPower_Particle[2] = m_pCurVoidDesc->vBloomPower.z;


			m_fRimColor_Particle[0] = m_pCurVoidDesc->vRimColor.x;
			m_fRimColor_Particle[1] = m_pCurVoidDesc->vRimColor.y;
			m_fRimColor_Particle[2] = m_pCurVoidDesc->vRimColor.z;
			m_fRimColor_Particle[3] = m_pCurVoidDesc->vRimColor.w;



			/* 디스토션_파티클 값 업데이트 */

			// 디스토션 스크롤 방향 업데이트
			if (CEffect_Void::SCROLL_ROW == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Particle = 0;
			else if (CEffect_Void::SCROLL_COL == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Particle = 1;
			else if (CEffect_Void::SCROLL_BOTH == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Particle = 2;
			else if (CEffect_Void::SCROLL_ROTAT == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Particle = 3;
			else if (CEffect_Void::TYPE_SCROLL_END == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Particle = 4;


			m_vScrollSpeeds_Particle[0] = pDistortionDesc->vScrollSpeeds.x;
			m_vScrollSpeeds_Particle[1] = pDistortionDesc->vScrollSpeeds.y;
			m_vScrollSpeeds_Particle[2] = pDistortionDesc->vScrollSpeeds.z;


			m_vScales_Distortion_Particle[0] = pDistortionDesc->vScales.x;
			m_vScales_Distortion_Particle[1] = pDistortionDesc->vScales.y;
			m_vScales_Distortion_Particle[2] = pDistortionDesc->vScales.z;


			m_vDistortion1_Particle[0] = pDistortionDesc->vDistortion1.x;
			m_vDistortion1_Particle[1] = pDistortionDesc->vDistortion1.y;

			m_vDistortion2_Particle[0] = pDistortionDesc->vDistortion2.x;
			m_vDistortion2_Particle[1] = pDistortionDesc->vDistortion2.y;

			m_vDistortion3_Particle[0] = pDistortionDesc->vDistortion3.x;
			m_vDistortion3_Particle[1] = pDistortionDesc->vDistortion3.y;

			m_fDistortionScale_Particle = pDistortionDesc->fDistortionScale;
			m_fDistortionBias_Particle = pDistortionDesc->fDistortionBias;


		}

		if (CEffect_Void::RECT == eType_Effect)
		{
			m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();								// 이펙트_보이드 Desc (부모)
			m_pRectDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Desc();	// 렉트의 Desc (자식)
			CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Distortion_Desc();
			CEffect_Void::UVSPRITE_DESC* pSpriteDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Sprite_Desc();


			m_iShaderPassIndex_Rect = m_pCurVoidDesc->iShaderPassIndex;		// 쉐이더 패스 인덱스 업데이트

			m_iRenderGroup_Rect = m_pCurVoidDesc->iRenderGroup;				// 렌더그룹 업데이트


			/* 빌보드 여부_Rect */
			if (m_pCurVoidDesc->bBillBoard)
				m_iBillBoard_Rect = 0;
			else
				m_iBillBoard_Rect = 1;


			/* 스프라이트 애니메이션 사용여부_Rect */
			if (m_pCurVoidDesc->bUseSpriteAnim)
				m_iSprite_Rect = 1;
			else
				m_iSprite_Rect = 0;

			if (0 == m_iSprite_Rect)	// 스프라이트 애니메이션 사용이면(렉트)
			{
				m_vUV_MaxTileCount_Particle[0] = (_int)pSpriteDesc->vUV_MaxTileCount.x;
				m_vUV_MaxTileCount_Particle[1] = (_int)pSpriteDesc->vUV_MaxTileCount.y;

				m_fSequenceTerm_Particle = pSpriteDesc->fSequenceTerm;
			}


			/* 쉐이더에서 곱해줄 색_Rect*/
			m_fColor_Mul_Rect[0] = m_pCurVoidDesc->vColor_Mul.x;
			m_fColor_Mul_Rect[1] = m_pCurVoidDesc->vColor_Mul.y;
			m_fColor_Mul_Rect[2] = m_pCurVoidDesc->vColor_Mul.z;
			m_fColor_Mul_Rect[3] = m_pCurVoidDesc->vColor_Mul.w;


			/* 쉐이더에서 discard할 값 */
			m_vColor_Clip_Rect[0] = m_pCurVoidDesc->vColor_Clip.x;
			m_vColor_Clip_Rect[1] = m_pCurVoidDesc->vColor_Clip.y;
			m_vColor_Clip_Rect[2] = m_pCurVoidDesc->vColor_Clip.z;
			m_vColor_Clip_Rect[3] = m_pCurVoidDesc->vColor_Clip.w;


			/* UV 회전 값 */
			m_fUV_RotDegree_Rect = m_pCurVoidDesc->fUV_RotDegree;

			m_fUV_Offset_Rect[0] = m_pCurVoidDesc->vUV_Offset.x;
			m_fUV_Offset_Rect[1] = m_pCurVoidDesc->vUV_Offset.y;

			m_vUV_Scale_Rect[0] = m_pCurVoidDesc->vUV_Scale.x;
			m_vUV_Scale_Rect[1] = m_pCurVoidDesc->vUV_Scale.y;



			/* 디스토션 */
			// 디스토션 스크롤 방향 업데이트
			if (CEffect_Void::SCROLL_ROW == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Rect = 0;
			else if (CEffect_Void::SCROLL_COL == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Rect = 1;
			else if (CEffect_Void::SCROLL_BOTH == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Rect = 2;
			else if (CEffect_Void::SCROLL_ROTAT == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Rect = 3;
			else if (CEffect_Void::TYPE_SCROLL_END == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Rect = 4;


			m_vScrollSpeeds[0] = pDistortionDesc->vScrollSpeeds.x;
			m_vScrollSpeeds[1] = pDistortionDesc->vScrollSpeeds.y;
			m_vScrollSpeeds[2] = pDistortionDesc->vScrollSpeeds.z;

			m_vScales_Distortion[0] = pDistortionDesc->vScales.x;
			m_vScales_Distortion[1] = pDistortionDesc->vScales.y;


			m_vDistortion1[0] = pDistortionDesc->vDistortion1.x;
			m_vDistortion1[1] = pDistortionDesc->vDistortion1.y;

			m_vDistortion2[0] = pDistortionDesc->vDistortion2.x;
			m_vDistortion2[1] = pDistortionDesc->vDistortion2.y;

			m_vDistortion3[0] = pDistortionDesc->vDistortion3.x;
			m_vDistortion3[1] = pDistortionDesc->vDistortion3.y;

			m_fDistortionBias = pDistortionDesc->fDistortionBias;


			/* 스프라이트 */
			m_fSequenceTerm_Rect = pSpriteDesc->fSequenceTerm;

			m_vUV_MaxTileCount_Rect[0] = (_int)pSpriteDesc->vUV_MaxTileCount.x;
			m_vUV_MaxTileCount_Rect[1] = (_int)pSpriteDesc->vUV_MaxTileCount.y;
		}


		if (CEffect_Void::MESH == eType_Effect)
		{
#pragma region 메쉬(+버퍼) 디스크립션 얻어오기 시작
			m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();	// 이펙트_보이드 Desc
			CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_Distortion_Desc();	// 메쉬(인스턴스)의 디스토션 Desc 얻어오기
			CVIBuffer_Effect_Model_Instance* pVIBuffer = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_VIBufferCom();	// 메쉬(인스턴스)버퍼 얻어오기
			m_pMeshBufferDesc = pVIBuffer->Get_Desc(); // 버퍼의 Desc 얻어오기
#pragma endregion

			// 텍스처 업데이트 =============================================================================================================
			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE];
			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK];
			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = m_pCurVoidDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE];
			// 텍스처 업데이트 =============================================================================================================

			/* 인스턴스 개수 */
			m_iNumInstance_Mesh = m_pMeshBufferDesc->iCurNumInstance;

			/* 모프 */
			m_fMorphTimeTerm = m_pMeshBufferDesc->fMorphTimeTerm;

			m_iShaderPassIndex_Mesh = m_pCurVoidDesc->iShaderPassIndex;

			m_iRenderGroup_Mesh = m_pCurVoidDesc->iRenderGroup;

			/* 디스카드 컬러값 업데이트 */
			m_vColor_Clip_Part[0] = m_pCurVoidDesc->vColor_Clip.x;
			m_vColor_Clip_Part[1] = m_pCurVoidDesc->vColor_Clip.y;
			m_vColor_Clip_Part[2] = m_pCurVoidDesc->vColor_Clip.z;
			m_vColor_Clip_Part[3] = m_pCurVoidDesc->vColor_Clip.w;


			/* 컬러 블렌딩 모드 업데이트 */
			if (MODE_COLOR::MUL == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 0;
			else if (MODE_COLOR::SCREEN == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 1;
			else if (MODE_COLOR::OVERLAY == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 2;
			else if (MODE_COLOR::ADD == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 3;
			else if (MODE_COLOR::BURN == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 4;
			else if (MODE_COLOR::MODE_COLOR_END == m_pCurVoidDesc->eMode_Color)
				m_iColor_Mode_Mesh = 5;


			/* 현재 곱하기 컬러 업데이트 */
			m_fColor_Mul_Mesh[0] = m_pCurVoidDesc->vColor_Mul.x;
			m_fColor_Mul_Mesh[1] = m_pCurVoidDesc->vColor_Mul.y;
			m_fColor_Mul_Mesh[2] = m_pCurVoidDesc->vColor_Mul.z;
			m_fColor_Mul_Mesh[3] = m_pCurVoidDesc->vColor_Mul.w;


			/* UV 조정 값 업데이트 */
			// Offset 
			m_fUV_Offset_Mesh[0] = m_pCurVoidDesc->vUV_Offset.x;
			m_fUV_Offset_Mesh[1] = m_pCurVoidDesc->vUV_Offset.y;

			// Scale
			m_vUV_Scale_Mesh[0] = m_pCurVoidDesc->vUV_Scale.x;
			m_vUV_Scale_Mesh[1] = m_pCurVoidDesc->vUV_Scale.y;

			// UV 회전
			m_fUV_RotDegree_Mesh = m_pCurVoidDesc->fUV_RotDegree;



			// 리지드바디 업데이트 =============================================================================================================		
			/* 리지드바디 사용 여부 */
			if (m_pCurVoidDesc->bUseRigidBody)
				m_iUseRigidBody_Mesh = 0;
			else
				m_iUseRigidBody_Mesh = 1;	
			
			if (0 == m_iUseRigidBody_Mesh)	// 리지드바디 사용이면
			{
				/* 키네틱 여부 */
				if (m_pMeshBufferDesc->bKinetic)
					m_iKinetic_Mesh = 0;
				else
					m_iKinetic_Mesh = 1;

				/* 중력 사용 여부 */
				if (m_pMeshBufferDesc->bUseGravity)
					m_iUseGravity_Mesh = 0;
				else
					m_iUseGravity_Mesh = 1;


				m_fGravity_Mesh			= m_pMeshBufferDesc->fGravity;			// 중력 가속도
				m_fFriction_Mesh		= m_pMeshBufferDesc->fFriction;			// 마찰 계수
				m_fSleepThreshold_Mesh	= m_pMeshBufferDesc->fSleepThreshold;	// 슬립 한계점


				/* 파워 */
				m_vMinMaxPower_Mesh[0] = m_pMeshBufferDesc->vMinMaxPower.x;
				m_vMinMaxPower_Mesh[1] = m_pMeshBufferDesc->vMinMaxPower.y;


				///* 질량(Mass) */
				//m_vMinMaxMass_Mesh[0] = m_pMeshBufferDesc->vMinMaxMass.x;
				//m_vMinMaxMass_Mesh[1] = m_pMeshBufferDesc->vMinMaxMass.y;

			}
			// 리지드바디 업데이트 =============================================================================================================
			
			/* 스피드 */
			m_vMinMaxSpeed_Mesh[0] = m_pMeshBufferDesc->vMinMaxSpeed.x;
			m_vMinMaxSpeed_Mesh[1] = m_pMeshBufferDesc->vMinMaxSpeed.y;


			/* 리사이클 여부_메쉬 파티클 */
			if (m_pMeshBufferDesc->bRecycle)
				m_iRecycle_Mesh = 0;
			else
				m_iRecycle_Mesh = 1;



			/* 움직이는 모양 업데이트 */
			if (CVIBuffer_Effect_Model_Instance::SPARK == m_pMeshBufferDesc->eType_Action)
				m_iType_Action_Mesh = 0;
			else if (CVIBuffer_Effect_Model_Instance::BLINK == m_pMeshBufferDesc->eType_Action)
				m_iType_Action_Mesh = 1;
			else if (CVIBuffer_Effect_Model_Instance::FALL == m_pMeshBufferDesc->eType_Action)
				m_iType_Action_Mesh = 2;
			else if (CVIBuffer_Effect_Model_Instance::RISE == m_pMeshBufferDesc->eType_Action)
				m_iType_Action_Mesh = 3;
			else if (CVIBuffer_Effect_Model_Instance::TORNADO == m_pMeshBufferDesc->eType_Action)
				m_iType_Action_Mesh = 4;



			/* 라이프타임_메쉬 파티클 */
			m_vMinMaxLifeTime_Mesh[0] = m_pMeshBufferDesc->vMinMaxLifeTime.x;
			m_vMinMaxLifeTime_Mesh[1] = m_pMeshBufferDesc->vMinMaxLifeTime.y;


			/* 분포 범위(Range) */
			m_vMinMaxRange_Mesh[0] = m_pMeshBufferDesc->vMinMaxRange.x;
			m_vMinMaxRange_Mesh[1] = m_pMeshBufferDesc->vMinMaxRange.y;


			/* 센터 오프셋 위치_메쉬 */
			m_vMinCenterOffsetPos_Mesh[0] = m_pMeshBufferDesc->vMinCenterOffsetPos.x;
			m_vMinCenterOffsetPos_Mesh[1] = m_pMeshBufferDesc->vMinCenterOffsetPos.y;
			m_vMinCenterOffsetPos_Mesh[2] = m_pMeshBufferDesc->vMinCenterOffsetPos.z;

			m_vMaxCenterOffsetPos_Mesh[0] = m_pMeshBufferDesc->vMaxCenterOffsetPos.x;
			m_vMaxCenterOffsetPos_Mesh[1] = m_pMeshBufferDesc->vMaxCenterOffsetPos.y;
			m_vMaxCenterOffsetPos_Mesh[2] = m_pMeshBufferDesc->vMaxCenterOffsetPos.z;


			/* 자체 회전 */
			m_vRadian_Mesh[0] = m_pMeshBufferDesc->vRadian.x;
			m_vRadian_Mesh[1] = m_pMeshBufferDesc->vRadian.y;
			m_vRadian_Mesh[2] = m_pMeshBufferDesc->vRadian.z;


			/* 회전 범위 */
			m_vRotationOffsetX_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetX.x;
			m_vRotationOffsetX_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetX.y;

			m_vRotationOffsetY_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetY.x;
			m_vRotationOffsetY_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetY.y;

			m_vRotationOffsetZ_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetZ.x;
			m_vRotationOffsetZ_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetZ.y;


			/* Rim Bloom 림블룸 관련 */
			m_vBloomPower_Mesh[0] = m_pCurVoidDesc->vBloomPower.x;
			m_vBloomPower_Mesh[1] = m_pCurVoidDesc->vBloomPower.y;
			m_vBloomPower_Mesh[2] = m_pCurVoidDesc->vBloomPower.z;

			m_fRimColor_Mesh[0] = m_pCurVoidDesc->vRimColor.x;
			m_fRimColor_Mesh[1] = m_pCurVoidDesc->vRimColor.y;
			m_fRimColor_Mesh[2] = m_pCurVoidDesc->vRimColor.z;
			m_fRimColor_Mesh[3] = m_pCurVoidDesc->vRimColor.w;

			m_fRimPower_Mesh = m_pCurVoidDesc->fRimPower;



			/* 스케일 러프 */
			if (TRUE == m_pMeshBufferDesc->bUseScaleLerp)
				m_iUseScaleLerp_Mesh = 0;
			else if (FALSE == m_pMeshBufferDesc->bUseScaleLerp)
				m_iUseScaleLerp_Mesh = 1;


			if (TRUE == m_pMeshBufferDesc->bScaleRatio)
				m_iScaleRatio_Mesh = 0;
			else if (FALSE == m_pMeshBufferDesc->bScaleRatio)
				m_iScaleRatio_Mesh = 1;


			m_vScaleLerp_Up_Pos_Mesh[0] = m_pMeshBufferDesc->vScaleLerp_Up_Pos.x;
			m_vScaleLerp_Up_Pos_Mesh[1] = m_pMeshBufferDesc->vScaleLerp_Up_Pos.y;

			m_vScaleLerp_Down_Pos_Mesh[0] = m_pMeshBufferDesc->vScaleLerp_Down_Pos.x;
			m_vScaleLerp_Down_Pos_Mesh[1] = m_pMeshBufferDesc->vScaleLerp_Down_Pos.y;

			m_vStartScale_Mesh[0] = m_pMeshBufferDesc->vStartScale.x;
			m_vStartScale_Mesh[1] = m_pMeshBufferDesc->vStartScale.y;
			m_vStartScale_Mesh[2] = m_pMeshBufferDesc->vStartScale.z;

			m_vEndScale_Mesh[0] = m_pMeshBufferDesc->vEndScale.x;
			m_vEndScale_Mesh[1] = m_pMeshBufferDesc->vEndScale.y;
			m_vEndScale_Mesh[2] = m_pMeshBufferDesc->vEndScale.z;




			/* 디스토션_메쉬 값 업데이트 */

			// 디스토션 스크롤 방향 업데이트
			if (CEffect_Void::SCROLL_ROW == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Mesh = 0;
			else if (CEffect_Void::SCROLL_COL == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Mesh = 1;
			else if (CEffect_Void::SCROLL_BOTH == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Mesh = 2;
			else if (CEffect_Void::SCROLL_ROTAT == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Mesh = 3;
			else if (CEffect_Void::TYPE_SCROLL_END == pDistortionDesc->eType_Scroll)
				m_iType_Scroll_Mesh = 4;



			m_vScrollSpeeds_Mesh[0] = pDistortionDesc->vScrollSpeeds.x;
			m_vScrollSpeeds_Mesh[1] = pDistortionDesc->vScrollSpeeds.y;
			m_vScrollSpeeds_Mesh[2] = pDistortionDesc->vScrollSpeeds.z;


			m_vScales_Distortion_Mesh[0] = pDistortionDesc->vScales.x;
			m_vScales_Distortion_Mesh[1] = pDistortionDesc->vScales.y;
			m_vScales_Distortion_Mesh[2] = pDistortionDesc->vScales.z;


			m_vDistortion1_Mesh[0] = pDistortionDesc->vDistortion1.x;
			m_vDistortion1_Mesh[1] = pDistortionDesc->vDistortion1.y;

			m_vDistortion2_Mesh[0] = pDistortionDesc->vDistortion2.x;
			m_vDistortion2_Mesh[1] = pDistortionDesc->vDistortion2.y;

			m_vDistortion3_Mesh[0] = pDistortionDesc->vDistortion3.x;
			m_vDistortion3_Mesh[1] = pDistortionDesc->vDistortion3.y;

			m_fDistortionScale_Mesh = pDistortionDesc->fDistortionScale;
			m_fDistortionBias_Mesh = pDistortionDesc->fDistortionBias;
		}

	}

}

void CWindow_EffectTool::Select_EasingType(EASING_TYPE* eType)
{
	/* Easing Type : 이징 타입 */
	if (ImGui::Button("LINEAR"))
	{
		*eType = EASING_TYPE::LINEAR;
	}
	if (ImGui::Button("QUAD_IN"))
	{
		*eType = EASING_TYPE::QUAD_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUAD_OUT"))
	{
		*eType = EASING_TYPE::QUAD_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUAD_INOUT"))
	{
		*eType = EASING_TYPE::QUAD_INOUT;
	}
	if (ImGui::Button("CUBIC_IN"))
	{
		*eType = EASING_TYPE::CUBIC_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("CUBIC_OUT"))
	{
		*eType = EASING_TYPE::CUBIC_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("CUBIC_INOUT"))
	{
		*eType = EASING_TYPE::CUBIC_INOUT;
	}
	if (ImGui::Button("QUART_IN"))
	{
		*eType = EASING_TYPE::QUART_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUART_OUT"))
	{
		*eType = EASING_TYPE::QUART_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUART_INOUT"))
	{
		*eType = EASING_TYPE::QUART_INOUT;
	}
	if (ImGui::Button("QUINT_IN"))
	{
		*eType = EASING_TYPE::QUINT_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUINT_OUT"))
	{
		*eType = EASING_TYPE::QUINT_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("QUINT_INOUT"))
	{
		*eType = EASING_TYPE::QUINT_INOUT;
	}
	if (ImGui::Button("SINE_IN"))
	{
		*eType = EASING_TYPE::SINE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("SINE_OUT"))
	{
		*eType = EASING_TYPE::SINE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("SINE_INOUT"))
	{
		*eType = EASING_TYPE::SINE_INOUT;
	}
	if (ImGui::Button("EXPO_IN"))
	{
		*eType = EASING_TYPE::EXPO_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("EXPO_OUT"))
	{
		*eType = EASING_TYPE::EXPO_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("EXPO_INOUT"))
	{
		*eType = EASING_TYPE::EXPO_INOUT;
	}
	if (ImGui::Button("CIRC_IN"))
	{
		*eType = EASING_TYPE::CIRC_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("CIRC_OUT"))
	{
		*eType = EASING_TYPE::CIRC_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("CIRC_INOUT"))
	{
		*eType = EASING_TYPE::CIRC_INOUT;
	}
	if (ImGui::Button("ELASTIC_IN"))
	{
		*eType = EASING_TYPE::ELASTIC_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("ELASTIC_OUT"))
	{
		*eType = EASING_TYPE::ELASTIC_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("ELASTIC_INOUT"))
	{
		*eType = EASING_TYPE::ELASTIC_INOUT;
	}
	if (ImGui::Button("BOUNCE_IN"))
	{
		*eType = EASING_TYPE::BOUNCE_IN;
	}
	ImGui::SameLine();
	if (ImGui::Button("BOUNCE_OUT"))
	{
		*eType = EASING_TYPE::BOUNCE_OUT;
	}
	ImGui::SameLine();
	if (ImGui::Button("ELASTIC_INOUT"))
	{
		*eType = EASING_TYPE::ELASTIC_INOUT;
	}

}


void CWindow_EffectTool::Update_ImageList_Window()
{
	//const int ObjCount = m_CubeFile.size();

	//char** cObj = new char* [ObjCount];
	//int ObjIndex = 0;

	//for (auto& iter : m_CubeFile)
	//{
	//	vector<wstring>& stateKetyvec = iter.second;
	//	sort(stateKetyvec.begin(), stateKetyvec.end());
	//}
	//for (const auto& iter : m_CubeFile)
	//{
	//	const wstring& folder = iter.first;
	//	const string utf8Str = WstringToUTF8(folder);

	//	cObj[ObjIndex] = new char[utf8Str.length() + 1];
	//	strcpy(cObj[ObjIndex], utf8Str.c_str());

	//	ObjIndex++;
	//}

	////ImGui::Combo(u8"폴더명", &m_iObjChoice, cObj, ObjCount);

	//char* str = cObj[m_iObjChoice];
	//wstring selectedObj = ConverCtoWC2(str);

	//if (m_CubeFile.find(selectedObj) != m_CubeFile.end())
	//{
	//	const vector<wstring>& fileNames = m_CubeFile[selectedObj];

	//	const int StateCount = fileNames.size();
	//	char** cState = new char* [StateCount];
	//	int stateIndex = 0;

	//	for (const wstring& fileName : fileNames)
	//	{
	//		const string utf8Str = WstringToUTF8(fileName);
	//		cState[stateIndex] = new char[utf8Str.length() + 1];
	//		strcpy(cState[stateIndex], utf8Str.c_str());

	//		stateIndex++;
	//	}

	//	//ImGui::Combo(u8"파일명", &m_iStateChoice, cState, StateCount);


	//	if (m_pSelectedImage != nullptr)
	//	{
	//		ImGui::Text(u8"선택된 이미지");
	//		ImGui::SameLine();
	//		ImGui::Image(m_pSelectedImage, ImVec2(80.f, 80.f));
	//	}

	//	char* StrObj = cObj[m_iObjChoice];
	//	char* StrState = cState[m_iStateChoice];
	//	wstring selectObj = ConverCtoWC2(StrObj);
	//	wstring selectState = ConverCtoWC2(StrState);

	//	int imagesPerRow = 7;
	//	int currentImageCount = 0;

	//	for (const wstring& fileName : fileNames)
	//	{
	//		LPDIRECT3DTEXTURE9 selecteTexture = FindTextureKey(selectedObj, fileName);

	//		if (selecteTexture != nullptr)
	//		{
	//			if (currentImageCount > 0 && currentImageCount % imagesPerRow == 0)
	//			{
	//				ImGui::NewLine();
	//			}

	//			ImVec4 backupButtonHoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
	//			ImVec4 backupButtonActiveColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

	//			ImVec4 clickedColor(0.8f, 0.5f, 0.3f, 1.0f);

	//			if (ImGui::ImageButton(selecteTexture, ImVec2(60.f, 60.f)))
	//			{
	//				m_pSelectedImage = selecteTexture;
	//				m_iTextureIndex = 0;
	//				for (wchar_t c : fileName)
	//				{
	//					if (iswdigit(c))
	//					{
	//						m_iTextureIndex = m_iTextureIndex * 10 + (c - L'0');
	//					}
	//				}

	//				ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = clickedColor;
	//				ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = clickedColor;
	//			}

	//			ImGui::SameLine();

	//			currentImageCount++;
	//		}
	//	}
	//	for (int i = 0; i < ObjIndex; ++i)
	//	{
	//		Safe_Delete_Array(cObj[i]);
	//	}
	//	for (int i = 0; i < stateIndex; ++i)
	//	{
	//		Safe_Delete_Array(cState[i]);
	//	}
	//	Safe_Delete_Array(cObj);
	//	Safe_Delete_Array(cState);
	//}




}

void CWindow_EffectTool::Update_LevelSetting_Window()
{

	// 카메라 위치 표시
	ImGui::SeparatorText("Camera");
	Show_CameraInfo();

	// 카메라 위치 리셋
	if (ImGui::Button("Reset Camera"))
	{
		ReSet_CameraPos();
		m_pCamera->Get_Transform()->Look_At(m_Camera_ResetLookAt);
	}

	if (ImGui::Button("Look Effect"))
	{
		ReSet_CameraPos();

		if (nullptr != m_pCurEffect)	// 현재 이펙트가 존재하면
		{
			// 카메라가 이펙트를 바라보도록
			m_pCamera->Get_Transform()->Look_At(m_pCurEffect->Get_Position_Vector());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Look Model"))
	{
		ReSet_CameraPos();

		if (nullptr != m_pModel_Preview) // 현재 프리뷰 모델이 존재하면
		{
			// 카메라가 모델을 바라보도록	
			m_pCamera->Get_Transform()->Look_At(m_pModel_Preview->Get_Position_Vector());
		}
	}



	// 그리드
	ImGui::SeparatorText("Grid");
	if (nullptr == m_pGrid)
	{
		if (ImGui::Button("Create Grid"))	// 그리드 생성
		{
			Ready_Grid();
		}
	}
	else
	{
		// 그리드가 존재하면
		CGrid::GRID_DESC* pGridDesc = dynamic_cast<CGrid*>(m_pGrid)->Get_Desc();	// 그리드 Desc 얻어오기

		if (ImGui::Button("ON Grid"))	// 그리드 켜기
		{
			pGridDesc->bRender = TRUE;
		}
		ImGui::SameLine();
		if (ImGui::Button("OFF Grid"))	// 그리드 끄기
		{
			pGridDesc->bRender = FALSE;
		}


		// 그리드 색 변경
		if (ImGui::ColorEdit4("Grid Color", m_fColor_Grid, ImGuiColorEditFlags_None))
		{
			pGridDesc->vGridColor.x = m_fColor_Grid[0];
			pGridDesc->vGridColor.y = m_fColor_Grid[1];
			pGridDesc->vGridColor.z = m_fColor_Grid[2];
			pGridDesc->vGridColor.w = m_fColor_Grid[3];
		}

		////m_iShaderPass_Grid
		//if (ImGui::DragInt("ShaderPass_Grid", &m_iShaderPass_Grid, 1, 2))
		//{
		//	m_pGrid->Get_Desc()->iShaderPassIndex = m_iShaderPass_Grid;
		//}


		if (ImGui::Button("Delete Grid"))	// 그리드 삭제
		{
			if (nullptr != m_pGrid)
			{
				m_pGrid->Set_Dead(TRUE);
				m_pGrid = nullptr;
			}
		}
	}


	// 바닥 메쉬
	ImGui::SeparatorText("Floor");
	if (nullptr == m_pFloor_Preview)
	{
		if (ImGui::Button("Create Floor"))	// 땅바닥 생성
		{
			//Ready_Model_Preview(TEXT("Prototype_Component_Model_EffectTool_IntroBoss_Floor"));

			CModel_Preview::MODEL_PREVIEW_DESC	tDesc = {};
			tDesc.strProtoTag = { TEXT("Prototype_GameObject_Model_Preview") };
			tDesc.strModelTag = { TEXT("Prototype_Component_Model_EffectTool_IntroBoss_Floor") };


			CGameObject* pObj = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_Model_Preview"), TEXT("Prototype_GameObject_Model_Preview"), &tDesc);
			if (nullptr != pObj)
				m_pFloor_Preview = dynamic_cast<CModel_Preview*>(pObj);
		}
	}
	else
	{

		// 바닥 모델 프리뷰가 존재하면	
		CModel_Preview::MODEL_PREVIEW_DESC* pDesc = dynamic_cast<CModel_Preview*>(m_pFloor_Preview)->Get_Desc();

		// 바닥 모델 월드 이동
		CTransform* pTransform = m_pFloor_Preview->Get_Transform();
		_float4 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Floor Pos  : %.2f %.2f %.2f", vPos.x, vPos.y, vPos.z);
		if (ImGui::DragFloat3("Floor_Pos", m_vWorldPosition_Floor, 0.2f))
		{
			m_pFloor_Preview->Set_Position(_float3(m_vWorldPosition_Floor[0], m_vWorldPosition_Floor[1], m_vWorldPosition_Floor[2]));
		}


		// 바닥 모델 크기 변경
		_float3 vScaled = pTransform->Get_Scaled();
		ImGui::Text("Floor Scaled  : %.2f %.2f %.2f", vScaled.x, vScaled.y, vScaled.z);
		if (ImGui::DragFloat3("Floor_Scale", m_vScale_Floor, 0.5f))
		{
			if (0.f == m_vScale_Floor[0])
				m_vScale_Floor[0] = 1.f;

			if (0.f == m_vScale_Floor[1])
				m_vScale_Floor[1] = 1.f;

			if (0.f == m_vScale_Floor[2])
				m_vScale_Floor[2] = 1.f;

			pTransform->Set_Scaling(m_vScale_Floor[0], m_vScale_Floor[1], m_vScale_Floor[2]);
		}


		if (ImGui::Button("Delete Floor"))	// 바닥 모델 삭제 버튼
		{
			if (nullptr != m_pFloor_Preview)
			{
				m_pFloor_Preview->Set_Dead(TRUE);
				m_pFloor_Preview = nullptr;
			}
		}

	}




	// 스카이박스
	ImGui::SeparatorText("SkyBox");
	if (nullptr != m_pSky)
	{
		// 스카이박스가 존재하면

		if (ImGui::Button("ON Sky"))	// 스카이박스 렌더 킴
		{
			m_pSky->Set_Render_Tool(TRUE);
		}
		ImGui::SameLine();
		if (ImGui::Button("OFF Sky"))	// 스카이박스 렌더 끔
		{
			m_pSky->Set_Render_Tool(FALSE);
		}

		// 스카이박스 텍스처 변경
		if (ImGui::InputInt(" Sky box Texture ", &m_iSkyTextureIndex, 1))
		{
			_uint iSkyTextureCount = m_pSky->Get_SkyTextureCount();

			if ((_int)iSkyTextureCount - 1 < m_iSkyTextureIndex)
				m_iSkyTextureIndex = iSkyTextureCount-1;

			if (0 > m_iSkyTextureIndex)
				m_iSkyTextureIndex = 0;

			Set_SkyTexture();
		}
	}


	// 크기비교용 모델
	ImGui::SeparatorText("Model_Preview");
	if (nullptr == m_pModel_Preview)
	{
		// 플레이어 모델 로드
		if (ImGui::Button("Load_Player"))	
		{
			_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
			m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Rentier"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player", PivotMatrix));
		}

		if (ImGui::Button("Create Model_Player"))	// 모델 생성
		{
			Ready_Model_Preview(TEXT("Prototype_Component_Model_Rentier")); 
		}
		//ImGui::SameLine();
		if (ImGui::Button("Create Model_Boss"))	// 모델 생성
		{
			Ready_Model_Preview(TEXT("Prototype_Component_Model_VampireCommander"));
		}

		if (ImGui::Button("Create Model_Torch"))	// 모델 생성
		{
			Ready_Model_Preview(TEXT("Prototype_Component_Model_Effect_Torch"));
		}

	}
	else
	{
		// 모델_프리뷰가 존재하면	
		CModel_Preview::MODEL_PREVIEW_DESC* pDesc = dynamic_cast<CModel_Preview*>(m_pModel_Preview)->Get_Desc();
		
		// 모델 월드 이동
		CTransform* pTransform = m_pModel_Preview->Get_Transform();
		_float4 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Model Pos  : %.2f %.2f %.2f", vPos.x, vPos.y, vPos.z);
		if (ImGui::DragFloat3("Model_Pos", m_vWorldPosition_Model, 0.2f))
		{
			m_pModel_Preview->Set_Position(_float3(m_vWorldPosition_Model[0], m_vWorldPosition_Model[1], m_vWorldPosition_Model[2]));
		}

		// 모델 월드 회전
		_float3 vRotated = pTransform->Get_Rotated();
		ImGui::Text("Model Rotated  : %.2f %.2f %.2f", vRotated.x, vRotated.y, vRotated.z);
		if (ImGui::DragFloat("Model_Rotate_X", &m_vWorldRotate_Model[0], 0.5f))
		{
			pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vWorldRotate_Model[0]));
		}
		if (ImGui::DragFloat("Model_Rotate_Y", &m_vWorldRotate_Model[1], 0.5f))
		{
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vWorldRotate_Model[1]));
		}
		if (ImGui::DragFloat("Model_Rotate_Z", &m_vWorldRotate_Model[2], 0.5f))
		{
			pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vWorldRotate_Model[2]));
		}


		// 모델 크기 변경
		_float3 vScaled = pTransform->Get_Scaled();
		ImGui::Text("Model Scaled  : %.2f %.2f %.2f", vScaled.x, vScaled.y, vScaled.z);
		if (ImGui::DragFloat3("Model_Scale", m_vScale_Model, 0.5f))
		{
			if (0.f == m_vScale_Model[0])
				m_vScale_Model[0] = 1.f;

			if (0.f == m_vScale_Model[1])
				m_vScale_Model[1] = 1.f;

			if (0.f == m_vScale_Model[2])
				m_vScale_Model[2] = 1.f;

			pTransform->Set_Scaling(m_vScale_Model[0], m_vScale_Model[1], m_vScale_Model[2]);
		}


		if (nullptr != m_pCurEffect)
		{
			// 이펙트의주인 정해주기 테스트
			if (ImGui::Button("ON  Pivot"))
			{
				m_pCurEffect->Set_Object_Owner(m_pModel_Preview);
				m_pCurEffect->Get_Desc()->bParentPivot = TRUE;
			}
			ImGui::SameLine();
			if (ImGui::Button("OFF Pivot"))
			{
				m_pCurEffect->Get_Desc()->bParentPivot = FALSE;
				m_pCurEffect->Delete_Object_Owner();
			}
		}

		// 애님 모델일 경우에만 애니메이션 변경
		if (CModel::TYPE_ANIM == m_pModel_Preview->Get_Desc()->eType)
		{
			ImGui::SeparatorText("Model Animation");
			if (ImGui::Button("Idle_"))
			{
				if (TEXT("Prototype_Component_Model_Rentier") == pDesc->strModelTag)
				{
					// 플레이어 아이들 // Index 8
					m_pModel_Preview->Set_AnimIndex(CPlayer::Player_IdleLoop);
				}

				if (TEXT("Prototype_Component_Model_VampireCommander") == pDesc->strModelTag)
				{
					// 보스 아이들 // Index 9
					m_pModel_Preview->Set_AnimIndex(9);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Attack_"))
			{
				if (TEXT("Prototype_Component_Model_Rentier") == pDesc->strModelTag)
				{
					// 플레이어 공격
					m_pModel_Preview->Set_AnimIndex(CPlayer::Player_Finisher_BanditHeavy_ZapperLeap);
				}

				if (TEXT("Prototype_Component_Model_VampireCommander") == pDesc->strModelTag)
				{
					// 보스 VampireCommander_AttackMelee_02 // Index 55
					m_pModel_Preview->Set_AnimIndex(55);
				}
			}
		}



		if (ImGui::Button("Delete Model"))	// 모델 삭제 버튼
		{
			if (nullptr != m_pModel_Preview)
			{
				//if(nullptr != m_pCurEffect->Get_Object_Owner())
				//{
				//	// 현재 이펙트의 오너가 nullptr이 아니면 오너부터 해제 해주고 모델 삭제
				//	m_pCurEffect->Get_Desc()->bParentPivot = FALSE;
				//	m_pCurEffect->Delete_Object_Owner();
				//}

				m_pModel_Preview->Set_Dead(TRUE);
				m_pModel_Preview = nullptr;
				m_pPart_Preview = nullptr;
			}
		}
	}


	// 마우스 위치 표시
	ImGui::SeparatorText("Mouse");
	Show_MousePos();


}

void CWindow_EffectTool::Update_EffectList_Window()
{
	auto& style = ImGui::GetStyle();

	/* 비어있는 이펙트 객체 생성 */
	if (ImGui::Button("         Create         ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
	{
		Create_EffectObject(TEXT("Layer_Effect"));
	}

	ImGui::SeparatorText("");
	if (ImGui::Button("         Create Test        "))
	{
		CEffect* pEffect = EFFECT_MANAGER->Create_Effect(LEVEL_TOOL, LAYER_EFFECT, "Test_Effect.json");
	}

	ImGui::SameLine();
	if (ImGui::Button("   Create Test Effect Trail   "))
	{
		CEffect* pEffect = EFFECT_MANAGER->Create_Effect_With_Trail("Test_Impact.json", "Test_Effect_Trail.json");
	}

	if (nullptr != m_pModel_Preview)
	{
		if (nullptr == m_pTestEffect)
		{
			if (ImGui::Button("         Skull Test        "))
			{
				m_pTestEffect = EFFECT_MANAGER->Create_Effect(LEVEL_TOOL, LAYER_EFFECT, "Test_Skull.json", m_pModel_Preview);
			}
		}
		else
		{
			if (ImGui::Button("         Skull Delete        "))
			{
				m_pTestEffect->Set_Dead(TRUE);
				m_pTestEffect = nullptr;
			}
		}

		if (ImGui::Button("         Zapper Dash Test        "))
		{
			CEffect* pEffect = EFFECT_MANAGER->Create_Effect_Pos("Zapper_Dash", "Zapper_Dash_12.json", m_pModel_Preview->Get_Position());
		}

	}





	/* 이펙트 리스트 & 현재 이펙트 선택 */
	ImGui::SeparatorText(" EFFECT LIST ");
	if (ImGui::ListBox(" Effect List ", &m_iCurEffectIndex, m_szEffectNames, (_int)m_pEffects.size(), (_int)6))
	{
		wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]);
		m_pCurEffect = m_pEffects.find(strCurName)->second;

		/* 문자열 초기화 */
		m_iCurPartIndex = (_int)m_CurPartObjects.size();
		if (nullptr != m_szPartNames)
		{
			for (_int i = 0; i < m_iCurPartIndex; ++i)
			{
				m_szPartNames[i] = nullptr;
			}
			m_szPartNames = nullptr;
		}

		Update_CurMembers(strCurName);

		m_iCurPartIndex = (_int)m_CurPartObjects.size();
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
			Update_CurParameters_Parts();
		}		
		else
			m_pCurPartEffect = nullptr;

	} // 이펙트 리스트 END



	if (nullptr != m_pCurEffect)
	{
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

		if (ImGui::Button(" Delete_Effect "))
		{
			Delete_CurEffectObject();
			return;
		}

		// =========================================

		ImGui::SeparatorText(" ADD PART EFFECT ");
		if (ImGui::Button(" Add Particle "))
		{
			Add_Part_Particle();
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Rect "))
		{
			Add_Part_Rect();
		}
		//ImGui::SameLine();
		//if (ImGui::Button(" Add Mesh "))
		//{
		//	//Add_Part_Mesh(TEXT("Prototype_Component_Model_Xray_ManHeavy"));
		//	//Add_Part_Mesh(TEXT("Prototype_Component_Model_splineMesh_tornado"));
		//	//Add_Part_Mesh(TEXT("Prototype_Component_Model_ShieldDome"));
		//	//Add_Part_Mesh(TEXT("Prototype_Component_Model_Particle_Test"));
		//}

		if (ImGui::Button(" Copy Part "))
		{
			Copy_CurPart();
		}


		// =========================================

		/* 이펙트 파트오브젝트 리스트 & 현재 파트오브젝트 선택 */
		ImGui::SeparatorText(" PART LIST ");
		if (ImGui::ListBox(" Part List ", &m_iCurPartIndex, m_szPartNames, (_int)m_CurPartObjects.size(), (_int)6))
		{
			wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szPartNames[m_iCurPartIndex]);
			m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.find(strCurName)->second);

			Update_CurParameters_Parts();
		}

		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurVoidDesc->eType_Effect;

			/* 선택 파티클 활성화 */
			if (ImGui::Button(" Active Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pCurVoidDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
					CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
					m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					m_pCurVoidDesc->bActive_Tool = TRUE;
				}

				if (CEffect_Void::RECT == eType_Effect)
				{
					m_pCurVoidDesc->bActive_Tool = TRUE;
				}

				if (CEffect_Void::MESH == eType_Effect)
				{
					m_pCurVoidDesc->bActive_Tool = TRUE;
				}
			}
			ImGui::SameLine();
			/* 선택 파티클 비활성화 */
			if (ImGui::Button(" UnActive Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pCurVoidDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
					CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
					m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					m_pCurVoidDesc->bActive_Tool = FALSE;
				}
				if (CEffect_Void::RECT == eType_Effect)
				{
					m_pCurVoidDesc->bActive_Tool = FALSE;
				}

				if (CEffect_Void::MESH == eType_Effect)
				{
					m_pCurVoidDesc->bActive_Tool = FALSE;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(" ReSet_Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();
					CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
					m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom()->ReSet();
				}
				if (CEffect_Void::RECT == eType_Effect)
				{

				}

				if (CEffect_Void::MESH == eType_Effect)
				{
					m_pCurPartEffect->ReSet_Effect();
				}
			}

			/* 선택 파트 이펙트 삭제 */
			if (ImGui::Button(" Delete Part "))
			{
				Delete_CurPart();
				m_pCurPartEffect = nullptr;
				return;
			}

		}


		/* Effect Edit Start */
		ImGui::SeparatorText("");
		ImGui::CollapsingHeader("Effect_Times");
		//if (ImGui::CollapsingHeader("Effect_Times"))
		{
			ImGui::Text("    Waiting   |   LifeTime   |   RemainTime ");
			if (ImGui::DragFloat3("Times_Effect", m_vTimes_Effect, 0.2f, 0.f))
			{
				if (m_vTimes_Effect[0] > m_vTimes_Effect[1])
					m_vTimes_Effect[0] = m_vTimes_Effect[1];

				m_pCurEffectDesc->fWaitingTime = m_vTimes_Effect[0];

				m_pCurEffectDesc->fLifeTime = m_vTimes_Effect[1];
				m_pCurEffectDesc->fRemainTime = m_vTimes_Effect[2];
			}
			ImGui::Text("Total_Time : %.2f", m_vTimes_Effect[0] + m_vTimes_Effect[1] + m_vTimes_Effect[2]);

			ImGui::SeparatorText("");
		}


		/* 루프 키고 끄기 */
		ImGui::SeparatorText(" LOOP_EFFECT ");
		ImGui::RadioButton(" Loop_Effect ", &m_iLoop, 0);  ImGui::SameLine();
		ImGui::RadioButton("None Loop_Effect", &m_iLoop, 1);
		if (0 == m_iLoop)
			m_pCurEffectDesc->bLoop = TRUE;
		else if (1 == m_iLoop)
			m_pCurEffectDesc->bLoop = FALSE;


		/* 죽음 타입 정하기 */
		ImGui::SeparatorText(" DEAD TYPE_Effect ");
		ImGui::RadioButton(" DEAD_AUTO ", &m_iType_Dead, 0); 
		ImGui::RadioButton("DEAD_OWNER", &m_iType_Dead, 1);	 
		ImGui::RadioButton("DEAD_NONE", &m_iType_Dead, 2);
		if (0 == m_iType_Dead)
			m_pCurEffectDesc->eType_Dead = CEffect::DEAD_AUTO;
		else if (1 == m_iType_Dead)
			m_pCurEffectDesc->eType_Dead = CEffect::DEAD_OWNER;
		else if (2 == m_iType_Dead)
			m_pCurEffectDesc->eType_Dead = CEffect::DEAD_NONE;



		if (nullptr != m_pCurPartEffect)
		{
			ImGui::CollapsingHeader("Part_Times");
			//if (ImGui::CollapsingHeader("Part_Times"))
			{
				CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_Desc()->eType_Effect;

				ImGui::Text("    Waiting   |   LifeTime   |   RemainTime ");
				if (ImGui::DragFloat3("Times_Part", m_vTimes_Part, 0.2f, 0.f))
				{
					m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();

					m_pCurVoidDesc->fWaitingTime		= m_vTimes_Part[0];
					m_pCurVoidDesc->fLifeTime			= m_vTimes_Part[1];
					m_pCurVoidDesc->fRemainTime			= m_vTimes_Part[2];


					if (CEffect_Void::PARTICLE == eType_Effect)
					{				
						CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
						m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					}
					else if (CEffect_Void::RECT == eType_Effect)
					{
						m_pRectDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Desc();

					}


					if (CEffect_Void::MESH == eType_Effect)
					{

					}
				}
				ImGui::Text("Total_Part : %.2f", m_vTimes_Part[0] + m_vTimes_Part[1] + m_vTimes_Part[2]);
			}
		}



		/* 이펙트 위치 표시 & 트랜스폼 */
		ImGui::SeparatorText("");
		_float4 vPos = m_pCurEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Effect Pos  : %.2f %.2f %.2f", vPos.x, vPos.y, vPos.z);
		if (ImGui::CollapsingHeader("Effect_Transform"))
		{
			CTransform* pTransform = m_pCurEffect->Get_Transform();
			if (ImGui::DragFloat3("Effect_Pos", m_vWorldPosition_Effect, 0.5f))
			{
				m_pCurEffect->Set_Position(_float3(m_vWorldPosition_Effect[0], m_vWorldPosition_Effect[1], m_vWorldPosition_Effect[2]));
			}

			_float3 vScaled = pTransform->Get_Scaled();
			ImGui::Text("Effect Scaled  : %.2f %.2f %.2f", vScaled.x, vScaled.y, vScaled.z);
			if (ImGui::DragFloat3("Effect_Scale", m_vScale_Effect, 0.5f))
			{
				if (0.f == m_vScale_Effect[0])
					m_vScale_Effect[0] = 1.f;

				if (0.f == m_vScale_Effect[1])
					m_vScale_Effect[1] = 1.f;

				if (0.f == m_vScale_Effect[2])
					m_vScale_Effect[2] = 1.f;

				pTransform->Set_Scaling(m_vScale_Effect[0], m_vScale_Effect[1], m_vScale_Effect[2]);
			}

			_float3 vRotated = pTransform->Get_Rotated();
			ImGui::Text("Effect Rotated  : %.2f %.2f %.2f", vRotated.x, vRotated.y, vRotated.z);
			//if (ImGui::DragFloat3("Effect_Rotate", m_vRotate_Effect, 0.5f))
			//{
			//	pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_vRotate_Effect[0]);
			//	pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_vRotate_Effect[1]);
			//	pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_vRotate_Effect[2]);
			//}

			if (ImGui::DragFloat("Effect_Rotate_X", &m_vRotate_Effect[0], 0.5f))
			{
				pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Effect[0]));
			}
			if (ImGui::DragFloat("Effect_Rotate_Y", &m_vRotate_Effect[1], 0.5f))
			{
				pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Effect[1]));
			}
			if (ImGui::DragFloat("Effect_Rotate_Z", &m_vRotate_Effect[2], 0.5f))
			{
				pTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vRotate_Effect[2]));
			}

		}


		if (nullptr != m_pCurPartEffect)
		{
			/* 파트 위치 표시 & 트랜스폼 */
			ImGui::SeparatorText("");
			_float4 vPos = m_pCurPartEffect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			ImGui::Text("Part Pos  : %.2f %.2f %.2f", vPos.x, vPos.y, vPos.z);
			if (ImGui::CollapsingHeader("Part_Transform"))
			{
				CTransform* pPartTransform = m_pCurPartEffect->Get_Transform();

				if (ImGui::DragFloat3("Part_Pos", m_vWorldPosition_Part, 0.5f))
				{
					m_pCurPartEffect->Set_Position(_float3(m_vWorldPosition_Part[0], m_vWorldPosition_Part[1], m_vWorldPosition_Part[2]));
				}
				_float3 vScaled = pPartTransform->Get_Scaled();
				ImGui::Text("Part Scaled  : %.2f %.2f %.2f", vScaled.x, vScaled.y, vScaled.z);
				if (ImGui::DragFloat3("Part_Scale", m_vScale_Part, 0.1f, 0.1f))
				{
					if (0.f == m_vScale_Part[0])
						m_vScale_Part[0] = 1.f;

					if (0.f == m_vScale_Part[1])
						m_vScale_Part[1] = 1.f;

					if (0.f == m_vScale_Part[2])
						m_vScale_Part[2] = 1.f;

					m_pCurPartEffect->Get_Transform()->Set_Scaling(m_vScale_Part[0], m_vScale_Part[1], m_vScale_Part[2]);
				}

				_float3 vRotated = pPartTransform->Get_Rotated();
				ImGui::Text("Part Rotated  : %.2f %.2f %.2f", vRotated.x, vRotated.y, vRotated.z);
				//if (ImGui::DragFloat3("Part_Rotate", m_vRotate_Part, 0.5f))
				//{
				//	pPartTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[0]));
				//	pPartTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[1]));
				//	pPartTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vRotate_Part[2]));
				//}

				if (ImGui::DragFloat("Part_Rotate_X", &m_vRotate_Part[0], 0.5f))
				{
					pPartTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[0]));
				}
				if (ImGui::DragFloat("Part_Rotate_Y", &m_vRotate_Part[1], 0.5f))
				{
					pPartTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[1]));
				}
				if (ImGui::DragFloat("Part_Rotate_Z", &m_vRotate_Part[2], 0.5f))
				{
					pPartTransform->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_vRotate_Part[2]));
				}


			}
			ImGui::SeparatorText("");
		}
	}
}

HRESULT CWindow_EffectTool::Create_EffectObject(const wstring& strLayerTag, CGameObject* pOwner)
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	CEffect::EFFECT_DESC	tEffectDesc = {};
	tEffectDesc.fSpeedPerSec = { 5.f };
	tEffectDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

	tEffectDesc.bActive_Tool = { TRUE };
	tEffectDesc.bPlay		 = { TRUE };
	tEffectDesc.bLoop		 = { TRUE };
	tEffectDesc.bFinished	 = { FALSE };
	tEffectDesc.bReverse	 = { FALSE };
	tEffectDesc.bRender		 = { TRUE };

	tEffectDesc.fTimeAcc = { 0.f };
	tEffectDesc.fWaitingAcc = { 0.f };
	tEffectDesc.fSequenceAcc = { 0.f };

	tEffectDesc.fLifeTime = { 5.f };
	tEffectDesc.fWaitingTime = { 0.f };
	tEffectDesc.fRemainTime = { 0.f };
	tEffectDesc.fSequenceTime = tEffectDesc.fWaitingTime + tEffectDesc.fLifeTime + tEffectDesc.fRemainTime;
	tEffectDesc.fLifeTimeRatio = min(1.f, tEffectDesc.fTimeAcc / tEffectDesc.fLifeTime);


	if (nullptr != pOwner)
	{
		tEffectDesc.bParentPivot = TRUE;
		tEffectDesc.matPivot = pOwner->Get_Transform()->Get_WorldFloat4x4();
		XMStoreFloat4x4(&tEffectDesc.matCombined, XMMatrixIdentity());
	}
	else
	{
		tEffectDesc.bParentPivot = FALSE;
		XMStoreFloat4x4(&tEffectDesc.matPivot, XMMatrixIdentity());
		XMStoreFloat4x4(&tEffectDesc.matCombined, XMMatrixIdentity());
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
	m_pCurEffectDesc = pEffect->Get_Desc();

#pragma endregion


#pragma region 이름 리스트 초기화&업데이트

	m_iCurEffectIndex = (_int)m_pEffects.size();
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
		CEffect_Void::EFFECTVOID_DESC	tVoidDesc = {};
		tVoidDesc.fSpeedPerSec = { 5.f };
		tVoidDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tVoidDesc.eType_Effect = CEffect_Void::PARTICLE;

		tVoidDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tVoidDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tVoidDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 /*1*/ };

		tVoidDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tVoidDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

		tVoidDesc.iRenderGroup = { CRenderer::RENDER_EFFECT };

		tVoidDesc.iShaderPassIndex = { 0 };

		tVoidDesc.bBillBoard = { TRUE };

		tVoidDesc.fUV_RotDegree = { 0.f };

		tVoidDesc.bPlay				= { TRUE };
		tVoidDesc.bUseSpriteAnim	= { FALSE };
		tVoidDesc.bUseRigidBody		= { TRUE };

		tVoidDesc.iCurNumInstance = { 1000 };


#pragma region 리스트 문자열 관련 시작
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

		tVoidDesc.strPartTag = strName;
		tVoidDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Particle");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Particle"), strName, &tVoidDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Particle*>(m_pCurEffect->Find_PartObject(strName));

		m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();


		m_iCurPartIndex = (_int)m_CurPartObjects.size();
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
#pragma endregion 리스트 문자열 관련 끝

		Update_CurParameters_Parts();

	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Rect()
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Void::EFFECTVOID_DESC tVoidDesc = {};
		tVoidDesc.fSpeedPerSec = { 5.f };
		tVoidDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tVoidDesc.eType_Effect = CEffect_Void::RECT;

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };


		tVoidDesc.iShaderPassIndex = { 0 };
		tVoidDesc.vColor_Mul = { 1.f, 1.f, 1.f, 1.f };

		tVoidDesc.iRenderGroup = { CRenderer::RENDER_EFFECT };


		tVoidDesc.bPlay = { TRUE };


#pragma region 리스트 문자열 관련
		wstring strName = TEXT("");
		wstring strFrontName = TEXT("Part_Rect");

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

		tVoidDesc.strPartTag = strName;
		tVoidDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Rect");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Rect"), strName, &tVoidDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Rect*>(m_pCurEffect->Find_PartObject(strName));

		m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();

		m_iCurPartIndex = (_int)m_CurPartObjects.size();
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

		Update_CurParameters_Parts();
#pragma endregion

	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Mesh(wstring strModelTag)
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Void::EFFECTVOID_DESC tVoidDesc = {};
		tVoidDesc.fSpeedPerSec = { 5.f };
		tVoidDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tVoidDesc.eType_Effect = { CEffect_Void::MESH };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };

		tVoidDesc.iShaderPassIndex = { 1 };

		tVoidDesc.iRenderGroup = { CRenderer::RENDER_EFFECT };

		tVoidDesc.bBillBoard = { FALSE };


		tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_01] = strModelTag;
		tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_02] = TEXT("");


		tVoidDesc.bPlay				= { TRUE };
		tVoidDesc.bUseRigidBody		= { TRUE };
		tVoidDesc.iCurNumInstance	= { 1000 };

		CEffect_Instance::EFFECT_INSTANCE_DESC tInstanceDesc = {};
		//tInstanceDesc.vRimColor = { 2.f, 10.f, 255.f, 255.f };
		//tInstanceDesc.fRimPower = { 950.f };

#pragma region 리스트 문자열 관련
		wstring strName = TEXT("");
		wstring strFrontName = TEXT("Part_Mesh");

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

		tVoidDesc.strPartTag = strName;
		tVoidDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Instance");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Instance"), strName, &tVoidDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Instance*>(m_pCurEffect->Find_PartObject(strName));

		m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();

		m_iCurPartIndex = (_int)m_CurPartObjects.size();
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


		Update_CurParameters_Parts();
#pragma endregion
	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Mesh_Morph(wstring strModelTag1, wstring strModelTag2)
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Void::EFFECTVOID_DESC tVoidDesc = {};
		tVoidDesc.fSpeedPerSec = { 5.f };
		tVoidDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tVoidDesc.eType_Effect = { CEffect_Void::MESH };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

		tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };

		tVoidDesc.iShaderPassIndex = { 0 };

		tVoidDesc.iRenderGroup = { CRenderer::RENDER_EFFECT };

		tVoidDesc.bBillBoard = { FALSE };

		tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_01] = strModelTag1;
		tVoidDesc.strModelTag[CVIBuffer_Effect_Model_Instance::MORPH_02] = strModelTag2;

		tVoidDesc.bPlay = { TRUE };
		tVoidDesc.bUseRigidBody = { TRUE };

		CEffect_Instance::EFFECT_INSTANCE_DESC tInstanceDesc = {};
		//tVoidDesc.vRimColor = { 2.f, 10.f, 255.f, 255.f };
		//tVoidDesc.vBloomPower = { 950.f };

#pragma region 리스트 문자열 관련
		wstring strName = TEXT("");
		wstring strFrontName = TEXT("Part_Mesh");

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

		tVoidDesc.strPartTag = strName;
		tVoidDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Instance");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Instance"), strName, &tVoidDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Instance*>(m_pCurEffect->Find_PartObject(strName));

		m_pCurVoidDesc = m_pCurPartEffect->Get_Desc();

		m_iCurPartIndex = (_int)m_CurPartObjects.size();
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


		Update_CurParameters_Parts();
#pragma endregion
	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Create_Trail()
{

	// 보이드 Desc 초기화
	CEffect_Void::EFFECTVOID_DESC tVoidDesc = {};
	tVoidDesc.fSpeedPerSec = { 0.f };
	tVoidDesc.fRotationPerSec = { XMConvertToRadians(0.0f) };

	tVoidDesc.eType_Effect = { CEffect_Void::TRAIL };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

	tVoidDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
	tVoidDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };

	tVoidDesc.iShaderPassIndex = { 0 };

	tVoidDesc.iRenderGroup = { CRenderer::RENDER_EFFECT };

	tVoidDesc.bBillBoard = { FALSE };

	tVoidDesc.bPlay = { TRUE };


	tVoidDesc.bParentPivot = { FALSE };
	XMStoreFloat4x4(&tVoidDesc.matPivot, XMMatrixIdentity());
	XMStoreFloat4x4(&tVoidDesc.matCombined, XMMatrixIdentity());


	// 트레일 객체 Desc 초기화
	//CEffect_Trail::TRAIL_DESC	tTrailDesc = {};
	//tTrailDesc.bTrailOn = { TRUE };
	//tTrailDesc.vLocalSwordLow = {};
	//tTrailDesc.vLocalSwordHigh = {};


	wstring strName = TEXT("Effect_Trail");

	tVoidDesc.strPartTag = strName;
	tVoidDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Trail");
	
	m_pTrail = dynamic_cast<CEffect_Trail*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, LAYER_EFFECT, tVoidDesc.strProtoTag, &tVoidDesc));


	return S_OK;

}

HRESULT CWindow_EffectTool::Delete_Trail()
{
	if (nullptr != m_pTrail)
	{
		m_pTrail->Set_Dead(TRUE);
		m_pTrail = nullptr;
	}

	return S_OK;
}

void CWindow_EffectTool::Delete_CurEffectObject()
{
	m_pCurEffect->Set_Dead(TRUE);
	m_pCurEffect = nullptr;
	m_pCurPartEffect = nullptr;

	wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]);
	m_pEffects.erase(strCurName);



	m_iCurEffectIndex = (_int)m_pEffects.size();
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

}

void CWindow_EffectTool::Delete_CurPart()
{
	m_pCurEffect->Delete_PartObject(m_pCurPartEffect->Get_Desc()->strPartTag);	


	wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]);

	/* 문자열 초기화 */
	m_iCurPartIndex = (_int)m_CurPartObjects.size();
	if (nullptr != m_szPartNames)
	{
		for (_int i = 0; i < m_iCurPartIndex; ++i)
		{
			m_szPartNames[i] = nullptr;
		}
		m_szPartNames = nullptr;
	}

	Update_CurMembers(strCurName);

	m_iCurPartIndex = (_int)m_CurPartObjects.size();
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
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.begin()->second);
	else
		m_pCurPartEffect = nullptr;

}

void CWindow_EffectTool::Copy_CurPart()
{

	if (nullptr != m_pCurPartEffect)
	{
		// 현재 선택된 파트 이펙트가 있을 경우




	}

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
	if (nullptr != m_pCurEffect)
	{
		CEffect::EFFECT_DESC* pDesc = m_pCurEffect->Get_Desc();
		CTransform* pTransform = m_pCurEffect->Get_Transform();

		m_vTimes_Effect[0] = pDesc->fWaitingTime;
		m_vTimes_Effect[1] = pDesc->fLifeTime;
		m_vTimes_Effect[2] = pDesc->fRemainTime;

		m_vTimes_Effect[0] = pDesc->fWaitingTime;
		m_vTimes_Effect[1] = pDesc->fLifeTime;

		_float4 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		m_vWorldPosition_Effect[0] = vPos.x;
		m_vWorldPosition_Effect[1] = vPos.y;
		m_vWorldPosition_Effect[2] = vPos.z;

		_float3 vScaled = pTransform->Get_Scaled();
		m_vScale_Effect[0] = vScaled.x;
		m_vScale_Effect[1] = vScaled.y;
		m_vScale_Effect[2] = vScaled.z;

		_float3 vRotated = pTransform->Get_Rotated();
		m_vRotate_Effect[0] = vRotated.x;
		m_vRotate_Effect[1] = vRotated.y;
		m_vRotate_Effect[2] = vRotated.z;
	}

}

void CWindow_EffectTool::Update_NeoSequencer_Window()
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

						if (!keys.empty())
							keys.clear();

						CEffect_Void* pPartObj = dynamic_cast<CEffect_Void*>(iter.second);
						_float fWaitingTime = pPartObj->Get_Desc()->fWaitingTime;
						_float fPartLifeTime = pPartObj->Get_Desc()->fLifeTime;
						_float fSequenceTime = pPartObj->Get_Desc()->fSequenceTime;

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



void CWindow_EffectTool::Update_SaveLoad_Menu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save"))
			{
				m_eFile = FILE_EFFECT;
				m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;
				m_strDialogPath = "../Bin/DataFiles/Data_Effect/";

				OpenDialog(IMGUI_EFFECTTOOL_WINDOW);

			}
			if (ImGui::MenuItem("Load"))
			{
				m_eFile = FILE_EFFECT;
				m_eDialogType = DIALOG_TYPE::LOAD_DIALOG;
				m_strDialogPath = "../Bin/DataFiles/Data_Effect/";

				OpenDialog(IMGUI_EFFECTTOOL_WINDOW);

			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

HRESULT CWindow_EffectTool::Save_Function(string strPath, string strFileName)
{

	json Out_Json;

	if (FILE_EFFECT == m_eFile)
	{
		if (nullptr != m_pCurEffect)
		{
			m_pCurEffect->Write_Json(Out_Json);
		}
	}

	if (FILE_TRAIL == m_eFile)
	{
		if (nullptr != m_pTrail)
		{
			m_pTrail->Write_Json(Out_Json);
		}
	}

	string strSavePath = strPath + "\\" + strFileName;
	CJson_Utility::Save_Json(strSavePath.c_str(), Out_Json);

	return S_OK;
}

HRESULT CWindow_EffectTool::Load_Function(string strPath, string strFileName)
{
	json In_Json;
	string strLoadPath = strPath + "\\" + strFileName;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

	if (FILE_EFFECT == m_eFile)
	{
		if (nullptr != m_pCurPartEffect)
		{
			m_pCurPartEffect->Load_FromJson(In_Json);

			Update_CurParameters_Parts();
			return S_OK;
		}

		if (nullptr == m_pCurEffect)
		{
			Create_EffectObject(TEXT("Layer_Effect"));
		}

		m_pCurEffect->Load_FromJson(In_Json);
		m_pCurEffectDesc = m_pCurEffect->Get_Desc();
		m_pCurEffectDesc->bLoop = TRUE;

		Update_CurParameters();

		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Get_FirstPartObject());
		Update_CurParameters_Parts();
	}


	if (FILE_TRAIL == m_eFile)
	{
		if (nullptr != m_pTrail)
		{
			m_pTrail->Load_FromJson(In_Json);

			return S_OK;
		}

		if (nullptr == m_pTrail)
		{
			Create_Trail();
			m_pTrail->Load_FromJson(In_Json);

			if (nullptr != m_pPart_Preview)
			{
				m_pPart_Preview->Set_Trail(m_pTrail);
			}
		}

	}

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

	if (!m_CurPartObjects.empty())
	{
		for (auto& Pair : m_CurPartObjects)
			Safe_Release(Pair.second);
		m_CurPartObjects.clear();
	}


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

}
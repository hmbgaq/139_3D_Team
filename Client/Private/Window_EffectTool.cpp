#include "stdafx.h"
#include "Window_EffectTool.h"

#include "imNeoSequencer/imgui_neo_sequencer.h"

#include "GameInstance.h"
#include "Easing_Utillity.h"

#include "Effect.h"
#include "Effect_Rect.h"
#include "Effect_Particle.h"
#include "Effect_Instance.h"
#include "Effect_Trail.h"
//#include "Mesh.h"

#include "Sky.h"

#include "Clone_Manager.h"

CWindow_EffectTool::CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{

}


HRESULT CWindow_EffectTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	Load_CustomStyle();

	//if (FAILED(Ready_Layer_Greed(TEXT("Layer_Greed"))))
	//	return E_FAIL;

	return S_OK;
}

void CWindow_EffectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ShowDialog();

#pragma region 리스트 창
	SetUp_ImGuiDESC(" Object Lists ", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.2f));
	__super::Begin();

	Update_SaveLoad();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);

	Update_EffectList();

	__super::End();
#pragma endregion 리스트 창


#pragma region 시퀀서 창
	SetUp_ImGuiDESC(" Sequencer ", ImVec2{ 1000.f, 400.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.2f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	Update_Demo_Sequencer();

	__super::End();
#pragma endregion 시퀀서 창



#pragma region 재생바 창
	SetUp_ImGuiDESC(" Play ", ImVec2{ 400.f, 300.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.2f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	Update_PlayBarArea();

	__super::End();
#pragma endregion 재생바 창



#pragma region 이펙트 툴

	SetUp_ImGuiDESC(u8"이펙트 툴", ImVec2{ 300.f, 800.f }, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | /*ImGuiWindowFlags_NoResize | */ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus, ImVec4(0.f, 0.f, 0.f, 0.2f));
	__super::Begin();

	ImGui::Text("ImGui Window Size : %d, %d", (_int)ImGui::GetWindowContentRegionMax().x, (_int)ImGui::GetWindowContentRegionMax().y);
	POINT	pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ImGui::Text("Mouse Pos : %d, %d", pt.x, pt.y);

	ImGui::SeparatorText("");
	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	_vector vCamDirection = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW).r[2];
	vCamDirection = XMVector4Normalize(vCamDirection);
	_float4 vCamDirectionFloat4 = {};
	XMStoreFloat4(&vCamDirectionFloat4, vCamDirection);

	ImGui::Text("Cam Pos  : %.2f %.2f %.2f", vCamPos.x, vCamPos.y, vCamPos.z);
	ImGui::Text("Cam Look : %.2f %.2f %.2f", vCamDirectionFloat4.x, vCamDirectionFloat4.y, vCamDirectionFloat4.z);

	if (ImGui::Button("CamPos Reset"))
	{
		CGameObject* pCamera = m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, TEXT("Layer_Camera"));
		if (nullptr != pCamera)
		{
			pCamera->Set_Position(_float3(0.f, -1.f, -10.f));
		}
	}


	ImGui::SeparatorText("");
	if (ImGui::Button("Create Sky"))
	{
		Ready_Sky();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Sky"))
	{
		CGameObject* pSky = m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, TEXT("Layer_BackGround"));
		if(nullptr != pSky)
			pSky->Set_Dead(TRUE);
	}

	//if (ImGui::Button("On Sky"))
	//{
	//	CGameObject* pSky = m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, TEXT("Layer_BackGround"));
	//	if (nullptr != pSky)
	//		pSky->Set_Enable(TRUE);
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Off Sky"))
	//{
	//	CGameObject* pSky = m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, TEXT("Layer_BackGround"));
	//	if (nullptr != pSky)
	//		pSky->Set_Enable(FALSE);
	//}

	if (ImGui::InputInt(" Sky box Texture ", &m_iSkyTextureIndex, 1))
	{
		if (5 < m_iSkyTextureIndex)
			m_iSkyTextureIndex = 5;

		if (0 > m_iSkyTextureIndex)
			m_iSkyTextureIndex = 0;

		Set_SkyTexture();
	}


	ImGui::SeparatorText("");
	if (ImGui::BeginTabBar("Tab_Effect", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem(" Particle "))
		{
			Update_ParticleTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(" Rect "))
		{
			//ImGui::Text(" Texture Effect ");
			Update_RectTab();
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

HRESULT CWindow_EffectTool::Ready_Sky()
{
	CGameObject* pSky = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"));
	if (nullptr != pSky)
	{
		dynamic_cast<CSky*>(pSky)->Set_TextureIndex(3);
	}

	return S_OK;
}

void CWindow_EffectTool::Set_SkyTexture()
{
	CGameObject* pSky = m_pGameInstance->Get_GameObect_Last(LEVEL_TOOL, TEXT("Layer_BackGround"));

	if (nullptr != pSky)
	{
		dynamic_cast<CSky*>(pSky)->Set_TextureIndex(m_iSkyTextureIndex);
	}

}

void CWindow_EffectTool::Update_PlayBarArea()
{
	/* 재생바 */
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
				_float fSequenceTimePos = m_pCurPartEffect->Get_SequenceAcc();
				ImGui::SliderFloat("SequenceTimeAcc_Part", &fSequenceTimePos, 0.f, m_pCurEffectDesc->fSequenceTime);

				m_vTimeAccs_Part[0] = (m_pCurPartEffect->Get_WaitingTime() != 0.0f) ? min(1.0f, m_pCurPartEffect->Get_WaitingAcc() / m_pCurPartEffect->Get_WaitingTime()) : 1.0f;
				m_vTimeAccs_Part[1] = m_pCurPartEffect->Get_LifeTimeRatio();
				m_vTimeAccs_Part[2] = (m_pCurPartEffect->Get_RemainTime() != 0.0f) ? min(1.0f, m_pCurPartEffect->Get_RemainAcc() / m_pCurPartEffect->Get_RemainTime()) : 1.0f;
				ImGui::SliderFloat3("TimeAccs_Part", m_vTimeAccs_Part, 0.f, 1.f);
			}
		}

		/* 재생, 정지, 리셋 */
		if (ImGui::Button("   Play   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffectDesc->bPlay = TRUE;
		}
		//ImGui::SameLine();
		if (ImGui::Button("   Stop   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffectDesc->bPlay = FALSE;
		}
		//ImGui::SameLine();
		if (ImGui::Button("   Reset   ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
		{
			m_pCurEffect->ReSet_Effect();
		}

	}
}


void CWindow_EffectTool::Update_ParticleTab()
{
	if (nullptr != m_pCurEffect)
	{
		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();

			if (CEffect_Void::PARTICLE == eType_Effect)
			{
#pragma region Desc 얻어오기 업데이트_파티클
				m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
				CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
				m_pParticleBufferDesc = pVIBuffer->Get_Desc();
#pragma endregion 

				/* 이름_파티클 */
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pParticleDesc->strPartTag));

#pragma region 텍스처 설정_파티클
				/* 텍스처 변경_파티클 */
				ImGui::CollapsingHeader(" Texture_Particle ");
				if (m_pParticleDesc->bUseSpriteAnim)
				{
					if (ImGui::InputInt("Diffuse_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE], 1))
					{
						if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE];

						if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = 0;

						m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE];
					}
				}
				else
				{
					if (ImGui::InputInt("Diffuse_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE], 1))
					{
						if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE];

						if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = 0;

						m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE];
					}
				}

				if (ImGui::InputInt("Mask_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK], 1))
				{
					if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_MASK];

					if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = 0;

					m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK];
				}

				if (ImGui::InputInt("Noise_Particle", &m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE], 1))
				{
					if (m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Particle[CEffect_Void::TEXTURE_NOISE];

					if (0 > m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = 0;

					m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE];
				}
#pragma endregion

				/* 빌보드 키고 끄기 */
				ImGui::SeparatorText("BillBoard");
				ImGui::RadioButton("BillBoard", &m_iBillBoard, 0);  ImGui::SameLine();
				ImGui::RadioButton("Off BillBoard", &m_iBillBoard, 1);
				if (0 == m_iBillBoard)
					m_pParticleDesc->bBillBoard = TRUE;
				else if (1 == m_iBillBoard)
					m_pParticleDesc->bBillBoard = FALSE;


#pragma region 스프라이트 설정_파티클
				/* 스프라이트 키고 끄기 */
				ImGui::SeparatorText("Sprite");
				ImGui::RadioButton("Off Sprite_Particle", &m_iSprite_Particle, 0); ImGui::SameLine();
				ImGui::RadioButton("Sprite_Particle", &m_iSprite_Particle, 1);
				if (0 == m_iSprite_Particle)
					m_pParticleDesc->bUseSpriteAnim = FALSE;
				else if (1 == m_iSprite_Particle)
					m_pParticleDesc->bUseSpriteAnim = TRUE;


				ImGui::SeparatorText(" Sprite Setting_Particle ");
				if (m_pParticleDesc->bUseSpriteAnim)
				{
					m_pSpriteDesc_Particle = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Sprite_Desc();

					/* 스프라이트 재생 속도_파티클 */
					ImGui::SeparatorText("");
					if (ImGui::DragFloat(" Sprite Term ", &m_fSequenceTerm_Particle, 1, 1))
					{
						m_pSpriteDesc_Particle->fSequenceTerm = m_fSequenceTerm_Particle;
					}

					/* 스프라이트 최대 가로 세로 지정_파티클 */
					if (ImGui::InputInt2(" Max_TileCount ", m_vUV_MaxTileCount_Particle, 1))
					{
						_uint iX, iY;
						dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_TextureCom(CEffect_Void::TEXTURE_SPRITE)->Get_TextureSize(&iX, &iY, m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE]);
						m_pSpriteDesc_Particle->vTextureSize.x = iX;
						m_pSpriteDesc_Particle->vTextureSize.y = iY;

						_float fTileX, fTileY;
						fTileX = (_float)iX / m_vUV_MaxTileCount_Particle[0];
						fTileY = (_float)iY / m_vUV_MaxTileCount_Particle[1];

						m_pSpriteDesc_Particle->vTileSize.x = fTileX;
						m_pSpriteDesc_Particle->vTileSize.y = fTileY;

						m_pSpriteDesc_Particle->vUV_MaxTileCount.x = m_vUV_MaxTileCount_Particle[0];
						m_pSpriteDesc_Particle->vUV_MaxTileCount.y = m_vUV_MaxTileCount_Particle[1];


						m_pCurPartEffect->ReSet_Effect();
					}

					ImGui::Text("Current Index : %d, %d", m_pSpriteDesc_Particle->vUV_CurTileIndex.x, m_pSpriteDesc_Particle->vUV_CurTileIndex.y);
				}
#pragma endregion

				ImGui::CollapsingHeader(" General Setting_Particle ");

				/* 렌더그룹 변경_파티클(현재는 Effect그룹으로 고정(9번) */
				ImGui::SeparatorText(" Render Group_Particle ");
				if (ImGui::InputInt(" Render Group_Particle ", &m_iRenderGroup_Particle, 1))
				{
					if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Particle)
					{
						m_iRenderGroup_Particle = (_int)CRenderer::RENDER_END - 1;
					}
					m_pParticleDesc->iRenderGroup = m_iRenderGroup_Particle;
				}


				/* 쉐이더 패스 인덱스 변경_파티클 */
				ImGui::CollapsingHeader(" Shader_Particle ");
				if (ImGui::InputInt("Shader Pass_Particle", &m_iShaderPassIndex_Particle, 1))
				{
					if (m_iMaxShaderPassIndex_Particle < m_iShaderPassIndex_Particle)
						m_iShaderPassIndex_Particle = m_iMaxShaderPassIndex_Particle;

					if (0 > m_iShaderPassIndex_Particle)
						m_iShaderPassIndex_Particle = 0;

					m_pParticleDesc->iShaderPassIndex = m_iShaderPassIndex_Particle;
				}

				/* 쉐이더에 던질 디스카드 값 변경_파티클 */
				ImGui::SeparatorText("Discard_Value");
				if (ImGui::DragFloat4("Discard_Particle", m_vColor_Clip_Part, 0.1f, 0.f, 1.f))
				{
					m_pParticleDesc->vColor_Clip.x = m_vColor_Clip_Part[0];
					m_pParticleDesc->vColor_Clip.y = m_vColor_Clip_Part[1];
					m_pParticleDesc->vColor_Clip.z = m_vColor_Clip_Part[2];
					m_pParticleDesc->vColor_Clip.w = m_vColor_Clip_Part[3];
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
				ImGui::SeparatorText("Instance");
				ImGui::Text("MaxInstance : %d", m_iMaxNumInstance_Particle);
				if (ImGui::DragInt("Instance Count", &m_iNumInstance_Particle, 1, 1, m_iNumInstance_Particle))
				{
					if (1 > m_iNumInstance_Particle)
						m_iNumInstance_Particle = 1;

					m_pParticleDesc->iCurNumInstance = m_iNumInstance_Particle;
					m_pParticleBufferDesc->iCurNumInstance = m_iNumInstance_Particle;
				}

				/* 리지드바디 키고 끄기 */
				ImGui::SeparatorText("RigidBody");
				ImGui::RadioButton(" ON  RigidBody_Particle ", &m_iUseRigidBody_Particle, 0);  ImGui::SameLine();
				ImGui::RadioButton(" OFF RigidBody_Particle ", &m_iUseRigidBody_Particle, 1);
				if (0 == m_iUseRigidBody_Particle)
				{
					m_pParticleDesc->bUseRigidBody = TRUE;
					m_pParticleBufferDesc->bUseRigidBody = TRUE;
				}				
				else if (1 == m_iUseRigidBody_Particle)
				{
					m_pParticleDesc->bUseRigidBody = FALSE;
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


						/* 스피드 조절(러프 관련) */
						ImGui::SeparatorText("Speed");
						if (ImGui::DragFloat2("vMinMaxSpeed_Particle", m_vMinMaxSpeed_Particle, 0.f, 1.f, 1000.f))
						{
							if (0.f > m_vMinMaxSpeed_Particle[0])	// Min이 0보다 작아지면 0으로 고정
								m_vMinMaxSpeed_Particle[0] = 0.f;

							if (m_vMinMaxSpeed_Particle[0] > m_vMinMaxSpeed_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
								m_vMinMaxSpeed_Particle[1] = m_vMinMaxSpeed_Particle[0];


							m_pParticleBufferDesc->vMinMaxSpeed.x = m_vMinMaxSpeed_Particle[0];
							m_pParticleBufferDesc->vMinMaxSpeed.y = m_vMinMaxSpeed_Particle[1];
						}

						ImGui::SeparatorText("Speed_Easing");
						Select_EasingType(&m_pParticleBufferDesc->eType_SpeedLerp);


						/* 마찰계수 조절 */
						ImGui::SeparatorText("Friction");
						if (ImGui::DragFloat("Friction_Particle", &m_fFriction_Particle, 0.1f, 0.1f, 100.f))
						{
							m_pParticleBufferDesc->fFriction = m_fFriction_Particle;
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


				/* 라이프 타임 */
				ImGui::SeparatorText("LifeTimes");
				if (ImGui::DragFloat2("MinMaxLifeTime_Particle", m_vMinMaxLifeTime_Particle, 1.f, 0.f, 360.f))
				{
					if (m_vMinMaxLifeTime_Particle[0] > m_vMinMaxLifeTime_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxLifeTime_Particle[1] = m_vMinMaxLifeTime_Particle[0];


					m_pParticleBufferDesc->vMinMaxLifeTime.x = m_vMinMaxLifeTime_Particle[0];
					m_pParticleBufferDesc->vMinMaxLifeTime.y = m_vMinMaxLifeTime_Particle[1];
				}


				/* 센터위치 오프셋(000 에서 어느정도 떨어진 지점에서 스폰 될건지) */
				ImGui::SeparatorText("Center Offset_Particle");
				if (ImGui::DragFloat2("MinMaxCenterX_Particle", m_vMinMaxCenterX_Particle, 1.f, 0.f, 500.f))
				{
					if (m_vMinMaxCenterX_Particle[0] > m_vMinMaxCenterX_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxCenterX_Particle[1] = m_vMinMaxCenterX_Particle[0];


					m_pParticleBufferDesc->vMinMaxCenterX.x = m_vMinMaxCenterX_Particle[0];
					m_pParticleBufferDesc->vMinMaxCenterX.y = m_vMinMaxCenterX_Particle[1];
				}

				if (ImGui::DragFloat2("MinMaxCenterY_Particle", m_vMinMaxCenterY_Particle, 1.f, 0.f, 500.f))
				{
					if (m_vMinMaxCenterY_Particle[0] > m_vMinMaxCenterY_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxCenterY_Particle[1] = m_vMinMaxCenterY_Particle[0];


					m_pParticleBufferDesc->vMinMaxCenterY.x = m_vMinMaxCenterY_Particle[0];
					m_pParticleBufferDesc->vMinMaxCenterY.y = m_vMinMaxCenterY_Particle[1];
				}

				if (ImGui::DragFloat2("MinMaxCenterZ_Particle", m_vMinMaxCenterZ_Particle, 1.f, 0.f, 500.f))
				{
					if (m_vMinMaxCenterZ_Particle[0] > m_vMinMaxCenterZ_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxCenterZ_Particle[1] = m_vMinMaxCenterZ_Particle[0];


					m_pParticleBufferDesc->vMinMaxCenterZ.x = m_vMinMaxCenterZ_Particle[0];
					m_pParticleBufferDesc->vMinMaxCenterZ.y = m_vMinMaxCenterZ_Particle[1];
				}



				/* 퍼지는 범위(분포 범위) */
				ImGui::SeparatorText("");
				if (ImGui::DragFloat2("MinMaxRange_Particle", m_vMinMaxRange_Particle, 1.f, 0.1f, 360.f))
				{
					if (m_vMinMaxRange_Particle[0] > m_vMinMaxRange_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxRange_Particle[1] = m_vMinMaxRange_Particle[0];


					m_pParticleBufferDesc->vMinMaxRange.x = m_vMinMaxRange_Particle[0];
					m_pParticleBufferDesc->vMinMaxRange.y = m_vMinMaxRange_Particle[1];

				}


				/* 회전 범위(오프셋) */
				/* RotX */
				ImGui::SeparatorText("");
				if (ImGui::DragFloat2("RotationX", m_vRotationOffsetX_Particle, 1.f, 0.f, 360.f))
				{
					if (m_vRotationOffsetX_Particle[0] > m_vRotationOffsetX_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vRotationOffsetX_Particle[1] = m_vRotationOffsetX_Particle[0];


					m_pParticleBufferDesc->vMinMaxRotationOffsetX.x = XMConvertToRadians(m_vRotationOffsetX_Particle[0]);
					m_pParticleBufferDesc->vMinMaxRotationOffsetX.y = XMConvertToRadians(m_vRotationOffsetX_Particle[1]);

				}

				/* RotY */
				if (ImGui::DragFloat2("RotationY", m_vRotationOffsetY_Particle, 1.f, 0.f, 360.f))
				{
					if (m_vRotationOffsetY_Particle[0] > m_vRotationOffsetY_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vRotationOffsetY_Particle[1] = m_vRotationOffsetY_Particle[0];


					m_pParticleBufferDesc->vMinMaxRotationOffsetY.x = XMConvertToRadians(m_vRotationOffsetY_Particle[0]);
					m_pParticleBufferDesc->vMinMaxRotationOffsetY.y = XMConvertToRadians(m_vRotationOffsetY_Particle[1]);
				}

				/* RotZ */
				if (ImGui::DragFloat2("RotationZ", m_vRotationOffsetZ_Particle, 1.f, 0.f, 360.f))
				{
					if (m_vRotationOffsetZ_Particle[0] > m_vRotationOffsetZ_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vRotationOffsetZ_Particle[1] = m_vRotationOffsetZ_Particle[0];

					m_pParticleBufferDesc->vMinMaxRotationOffsetZ.x = XMConvertToRadians(m_vRotationOffsetZ_Particle[0]);
					m_pParticleBufferDesc->vMinMaxRotationOffsetZ.y = XMConvertToRadians(m_vRotationOffsetZ_Particle[1]);
				}

#pragma region 크기 변경_파티클 시작
				ImGui::CollapsingHeader(" Scale_Particle ");

				ImGui::SeparatorText("Scale_Lerp Pos");
				if (ImGui::DragFloat2("Scale Lerp Pos_Particle", m_vLerpScale_Pos_Particle, 0.1f, 0.f, 1.f))
				{
					if (m_vLerpScale_Pos_Particle[0] > m_vLerpScale_Pos_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vLerpScale_Pos_Particle[1] = m_vLerpScale_Pos_Particle[0];

					m_pParticleBufferDesc->vLerpScale_Pos.x = m_vLerpScale_Pos_Particle[0];
					m_pParticleBufferDesc->vLerpScale_Pos.y = m_vLerpScale_Pos_Particle[1];
				}

				ImGui::SeparatorText("Scale_Speed");
				if (ImGui::DragFloat2("Scale Speed_Particle", m_vScaleSpeed_Particle, 0.001f, 0.f, 1.f))
				{
					m_pParticleBufferDesc->vScaleSpeed.x = m_vScaleSpeed_Particle[0];
					m_pParticleBufferDesc->vScaleSpeed.y = m_vScaleSpeed_Particle[1];
				}


				ImGui::SeparatorText("Scale_Easing");
				Select_EasingType(&m_pParticleBufferDesc->eType_ScaleLerp);

				ImGui::SeparatorText("MinMax Width");
				if (ImGui::DragFloat2("MinMax Width_Particle", m_vMinMaxWidth_Particle, 0.5f, 0.f, 5000.f))
				{
					if (m_vMinMaxWidth_Particle[0] > m_vMinMaxWidth_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxWidth_Particle[1] = m_vMinMaxWidth_Particle[0];

					m_pParticleBufferDesc->vMinMaxWidth.x = m_vMinMaxWidth_Particle[0];
					m_pParticleBufferDesc->vMinMaxWidth.y = m_vMinMaxWidth_Particle[1];
				}


				ImGui::SeparatorText("MinMax Height");
				if (ImGui::DragFloat2("MinMax Height_Particle", m_vMinMaxHeight_Particle, 0.5f, 0.f, 5000.f))
				{
					if (m_vMinMaxHeight_Particle[0] > m_vMinMaxHeight_Particle[1])	// Min이 Max보다 크면 Max를 Min으로
						m_vMinMaxHeight_Particle[1] = m_vMinMaxHeight_Particle[0];

					m_pParticleBufferDesc->vMinMaxHeight.x = m_vMinMaxHeight_Particle[0];
					m_pParticleBufferDesc->vMinMaxHeight.y = m_vMinMaxHeight_Particle[1];
				}


#pragma endregion 크기_파티클 끝

#pragma region 색 변경_파티클
				/* 파티클 페이드 인아웃 사용 */
				ImGui::SeparatorText(" Fade Type ");
				if (ImGui::Button(" Fade_None "))
				{
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_NONE;
				}
				ImGui::SameLine();
				if (ImGui::Button(" Fade_In "))
				{
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_IN;
				}
				ImGui::SameLine();
				if (ImGui::Button(" Fade_Out "))
				{
					m_pParticleBufferDesc->eType_Fade = CVIBuffer_Particle::TYPE_FADE::FADE_OUT;
				}

				/* 색 변경 */
				ImGui::CollapsingHeader(" Color_Particle ");

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

				ImGui::SeparatorText("Color_Easing");
				Select_EasingType(&m_pParticleBufferDesc->eType_ColorLerp);

#pragma endregion 색 변경_파티클 끝

				//if (CVIBuffer_Particle_Point::TYPE_ACTION::SPHERE == m_pParticlePointDesc->eType_Action)
				//{
				//	if (ImGui::DragFloat2("MinMaxLength", m_vMinMaxLengthPosition, 1.f, 0.1f, 360.f))
				//	{
				//		m_pParticlePointDesc->vMinMaxRangeLength.x = m_vMinMaxLengthPosition[0];
				//		m_pParticlePointDesc->vMinMaxRangeLength.y = m_vMinMaxLengthPosition[1];
				//	}		
				//}

				
				///* 재생, 역재생 */
				//ImGui::SeparatorText(" Play Type ");
				//if (ImGui::Button(" Normal "))
				//{
				//	m_pParticlePointDesc->bReverse = FALSE;
				//}

				//ImGui::SameLine();
				//if (ImGui::Button(" Reverse "))
				//{
				//	m_pParticlePointDesc->bReverse = TRUE;
				//}
				//ImGui::SameLine(); HelpMarker(u8"재생, 역재생");





				/* 텍스처 UV회전 */
				ImGui::SeparatorText("");
				if (ImGui::DragFloat(" Uv Degree ", &m_fUV_RotDegree, 1.f, 0.f, 360.f))
					m_pParticleDesc->fUV_RotDegree = m_fUV_RotDegree;


				///* 추가 크기 조절 */
				//ImGui::SeparatorText("");
				//if (ImGui::DragFloat("AddScale_1", &m_fAddScale, 1.f, 0.f, 360.f))
				//{
				//	m_pParticlePointDesc->vAddScale.x = m_fAddScale;
				//	m_pParticlePointDesc->vAddScale.y = m_fAddScale;
				//}

				//if (ImGui::DragFloat2("AddScale_2", m_vAddScale, 1.f, 0.f, 360.f))
				//{
				//	if (0 > m_vAddScale[0])
				//		m_vAddScale[0] = 0.f;

				//	if (0 > m_vAddScale[1])
				//		m_vAddScale[1] = 0.f;

				//	m_pParticlePointDesc->vAddScale.x = m_vAddScale[0];
				//	m_pParticlePointDesc->vAddScale.y = m_vAddScale[1];
				//}






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
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();

			if (CEffect_Void::RECT == eType_Effect)
			{
				m_pRectDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Desc();
				CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Distortion_Desc();

				/* 이름 */
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pRectDesc->strPartTag));

				/* 텍스처 변경 */
				if (CEffect_Rect::SINGLE == m_pRectDesc->eType)
				{
					if (ImGui::InputInt("Diffuse_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE], 1))
					{
						if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE];

						if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE] = 0;

						m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE];
					}
				}
				else
				{
					if (ImGui::InputInt("Sprite_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE], 1))
					{
						if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE];

						if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE])
							m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE] = 0;

						m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_SPRITE];
					}
				}


				if (ImGui::InputInt("Mask_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK], 1))
				{
					if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_MASK];

					if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK] = 0;

					m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_MASK];
				}

				if (ImGui::InputInt("Noise_Rect", &m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE], 1))
				{
					if (m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Rect[CEffect_Void::TEXTURE_NOISE];

					if (0 > m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE] = 0;

					m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Rect[CEffect_Void::TEXTURE_NOISE];
				}

				/* 쉐이더 패스 인덱스 변경 */
				if (ImGui::InputInt("Shader Pass_Rect", &m_iShaderPassIndex_Rect, 1))
				{
					if (m_iMaxShaderPassIndex_Rect < m_iShaderPassIndex_Rect)
						m_iShaderPassIndex_Rect = m_iMaxShaderPassIndex_Rect;

					if (0 > m_iShaderPassIndex_Rect)
						m_iShaderPassIndex_Rect = 0;

					m_pRectDesc->iShaderPassIndex = m_iShaderPassIndex_Rect;
				}
				/* 쉐이더에 던질 디스카드 값 변경 */
				//vColor_Clip
				if (ImGui::DragFloat4("Discard_Rect", m_vColor_Clip_Rect, 0.1f, 0.f, 1.f))
				{
					m_pRectDesc->vColor_Clip.x = m_vColor_Clip_Rect[0];
					m_pRectDesc->vColor_Clip.y = m_vColor_Clip_Rect[1];
					m_pRectDesc->vColor_Clip.z = m_vColor_Clip_Rect[2];
					m_pRectDesc->vColor_Clip.w = m_vColor_Clip_Rect[3];
				}

				/* 렌더그룹 변경 */
				ImGui::SeparatorText("");
				if (ImGui::InputInt("Render Group_Rect", &m_iRenderGroup_Rect, 1))
				{
					if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Rect)
					{
						m_iRenderGroup_Rect = (_int)CRenderer::RENDER_END - 1;
					}
					m_pRectDesc->iRenderGroup = m_iRenderGroup_Rect;
				}

				ImGui::SeparatorText("RECT_TYPE");
				if (ImGui::Button("SINGLE"))
				{
					m_pRectDesc->eType = CEffect_Rect::SINGLE;

				}
				ImGui::SameLine();
				if (ImGui::Button("SPRITE"))
				{
					m_pRectDesc->eType = CEffect_Rect::SPRITE;
				}


				if (CEffect_Rect::SINGLE == m_pRectDesc->eType)
				{
					ImGui::SeparatorText("");
					if (ImGui::CollapsingHeader(" Distortion Preset "))
					{
						if (ImGui::Button("FIRE"))
						{
							m_pRectDesc->iShaderPassIndex = { 2 };
							m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 1 };
							m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 17 };
							m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 1 };

							pDistortionDesc->fSequenceTerm = 1.f;
							pDistortionDesc->vScrollSpeeds = { 1.f, 1.f, 1.f };
							pDistortionDesc->vScales = { 1.f, 1.f, 1.f };

							pDistortionDesc->vDistortion1 = { 0.1f, 0.1f };
							pDistortionDesc->vDistortion2 = { 0.f, 0.f };
							pDistortionDesc->vDistortion3 = { 0.f, 0.1f };

							pDistortionDesc->fDistortionScale = { 1.f };
							pDistortionDesc->fDistortionBias = { 1.f };

							Update_CurParameters_Parts();
						}
					}
					ImGui::SeparatorText("");

					if (ImGui::DragFloat("Distortion_Term", &m_fSequenceTerm_Distortion, 1.f, 0.f))
					{
						pDistortionDesc->fSequenceTerm = m_fSequenceTerm_Distortion;
					}
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
				else
				{
					CEffect_Void::UVSPRITE_DESC* pSpriteDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Sprite_Desc();

					ImGui::SeparatorText("");
					if (ImGui::DragFloat(" Sprite Term ", &m_fSequenceTerm_RectSprite, 1, 1))
					{
						pSpriteDesc->fSequenceTerm = m_fSequenceTerm_RectSprite;
					}

					if (ImGui::InputInt2(" Max_TileCount ", m_vUV_MaxTileCount, 1))
					{
						_uint iX, iY;
						dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_TextureCom(CEffect_Void::TEXTURE_DIFFUSE)->Get_TextureSize(&iX, &iY, m_iTexIndex_Rect[CEffect_Void::TEXTURE_DIFFUSE]);
						pSpriteDesc->vTextureSize.x = iX;
						pSpriteDesc->vTextureSize.y = iY;

						_float fTileX, fTileY;
						fTileX = (_float)iX / m_vUV_MaxTileCount[0];
						fTileY = (_float)iY / m_vUV_MaxTileCount[1];

						pSpriteDesc->vTileSize.x = fTileX;
						pSpriteDesc->vTileSize.y = fTileY;

						pSpriteDesc->vUV_MaxTileCount.x = m_vUV_MaxTileCount[0];
						pSpriteDesc->vUV_MaxTileCount.y = m_vUV_MaxTileCount[1];


						m_pCurPartEffect->ReSet_Effect();
					}

					ImGui::SeparatorText("");
					if (ImGui::Button("Play_Sprite"))
					{
						m_pRectDesc->bPlay = TRUE;
					}
					ImGui::SameLine();
					if (ImGui::Button("Stop_Sprite"))
					{
						m_pRectDesc->bPlay = FALSE;
					}
					ImGui::Text("Current Index : %d, %d", pSpriteDesc->vUV_CurTileIndex.x, pSpriteDesc->vUV_CurTileIndex.y);
				}

			}
		}
	}
}

void CWindow_EffectTool::Update_MeshTab()
{
	if (nullptr != m_pCurEffect)
	{
		if (ImGui::Button("Demo_Xray"))
		{
			Add_Part_Mesh(TEXT("Prototype_Component_Model_Xray_ManHeavy"));
		}
		ImGui::SameLine();
		if (ImGui::Button("Demo_Dome"))
		{
			Add_Part_Mesh(TEXT("Prototype_Component_Model_ShieldDome"));
		}


		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();

			if (CEffect_Void::MESH == eType_Effect)
			{
				m_pInstanceDesc = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_Desc();
				CVIBuffer_Effect_Model_Instance* pVIBuffer = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_VIBufferCom();
				m_pMeshBufferDesc = pVIBuffer->Get_Desc();


				//// 이펙트 모델 리스트박스
				//if (ImGui::ListBox(" Meshes ", &m_iCurEffectIndex, m_szEffectNames, m_pEffects.size(), (_int)6))
				//{
				//	
				//}


				/* 이름 */
				ImGui::Text(m_pGameInstance->ConverWStringtoC(m_pInstanceDesc->strPartTag));

				/* 텍스처 변경 */
				if (ImGui::InputInt("Diffuse_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE], 1))
				{
					if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE];

					if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = 0;

					m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE];
				}

				if (ImGui::InputInt("Mask_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK], 1))
				{
					if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_MASK];

					if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = 0;

					m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK];
				}

				if (ImGui::InputInt("Noise_Mesh", &m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE], 1))
				{
					if (m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] <= m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = m_iMaxTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE];

					if (0 > m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE])
						m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = 0;

					m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE];
				}

				/* 쉐이더 패스 인덱스 변경 */
				if (ImGui::InputInt("Shader Pass_Mesh", &m_iShaderPassIndex_Mesh, 1))
				{
					if (m_iMaxShaderPassIndex_Mesh < m_iShaderPassIndex_Mesh)
						m_iShaderPassIndex_Mesh = m_iMaxShaderPassIndex_Mesh;

					if (0 > m_iShaderPassIndex_Mesh)
						m_iShaderPassIndex_Mesh = 0;

					m_pInstanceDesc->iShaderPassIndex = m_iShaderPassIndex_Mesh;
				}
				/* 쉐이더에 던질 디스카드 값 변경 */
				//vColor_Clip
				if (ImGui::DragFloat4("Discard_Mesh", m_vColor_Clip_Part, 0.1f, 0.f, 1.f))
				{
					m_pInstanceDesc->vColor_Clip.x = m_vColor_Clip_Part[0];
					m_pInstanceDesc->vColor_Clip.y = m_vColor_Clip_Part[1];
					m_pInstanceDesc->vColor_Clip.z = m_vColor_Clip_Part[2];
					m_pInstanceDesc->vColor_Clip.w = m_vColor_Clip_Part[3];
				}

				/* 렌더그룹 변경 */
				ImGui::SeparatorText("");
				if (ImGui::InputInt("Render Group_Mesh", &m_iRenderGroup_Mesh, 1))
				{
					if ((_int)CRenderer::RENDER_END < m_iRenderGroup_Mesh)
					{
						m_iRenderGroup_Mesh = (_int)CRenderer::RENDER_END - 1;
					}
					m_pInstanceDesc->iRenderGroup = m_iRenderGroup_Mesh;
				}

				/* UV 값 조절 */
				ImGui::SeparatorText("");
				if (ImGui::DragFloat2(" UV_Offset ", m_fUV_Offset, 1.f, 0.f, 100.f))
				{
					m_pInstanceDesc->vUV_Offset.x = m_fUV_Offset[0];
					m_pInstanceDesc->vUV_Offset.y = m_fUV_Offset[1];
				}

				if (ImGui::DragFloat2(" UV_Scale ", m_vUV_Scale, 1.f, 0.f, 100.f))
				{
					m_pInstanceDesc->vUV_Scale.x = m_vUV_Scale[0];
					m_pInstanceDesc->vUV_Scale.y = m_vUV_Scale[1];
				}


				if (ImGui::DragFloat(" RotDegree_Mesh ", &m_fUV_RotDegree_Mesh, 1.f, 0.f, 360.f))
				{
					m_pInstanceDesc->fUV_RotDegree = m_fUV_RotDegree_Mesh;
				}


				/* 디졸브 값 확인 */
				ImGui::SeparatorText("");
				ImGui::SliderFloat(" DissolveAmount ", &m_pInstanceDesc->fDissolveAmount, 0.f, 1.f);
				ImGui::SameLine();
				HelpMarker(u8"마스크:1/노이즈:5/쉐이더패스:6/렌더그룹:7");


				Select_EasingType(&m_pInstanceDesc->eType_Easing);

				/* RimRight */
				/* 림라이트 색 변경 */
				ImGui::SeparatorText("");
				if (ImGui::ColorEdit4("RimColor", m_fRimColor_Mesh, ImGuiColorEditFlags_None))
				{
					m_pInstanceDesc->vRimColor.x = m_fRimColor_Mesh[0];
					m_pInstanceDesc->vRimColor.y = m_fRimColor_Mesh[1];
					m_pInstanceDesc->vRimColor.z = m_fRimColor_Mesh[2];
					m_pInstanceDesc->vRimColor.w = m_fRimColor_Mesh[3];
				}

				/* 림라이트 세기 */
				if (ImGui::DragFloat(" RimPower ", &m_fRimPower_Mesh, 0.1f, 0.f, 1000.f))
				{
					m_pInstanceDesc->fRimPower = m_fRimPower_Mesh;
				}
				ImGui::SeparatorText("");


				/* Bloom */
				/* 색 변경 */
				if (ImGui::ColorEdit4("BloomColor_Mesh", m_vBloomColor_Mesh, ImGuiColorEditFlags_None))
				{
					m_pInstanceDesc->vBloomColor.x = m_vBloomColor_Mesh[0];
					m_pInstanceDesc->vBloomColor.y = m_vBloomColor_Mesh[1];
					m_pInstanceDesc->vBloomColor.z = m_vBloomColor_Mesh[2];
					m_pInstanceDesc->vBloomColor.w = m_vBloomColor_Mesh[3];
				}

				if (ImGui::DragFloat3(" BloomPower ", m_vBloomPower_Mesh, 100.f, 0.f, 1000.f))
				{
					m_pInstanceDesc->vBloomPower.x = m_vBloomPower_Mesh[0];
					m_pInstanceDesc->vBloomPower.y = m_vBloomPower_Mesh[1];
					m_pInstanceDesc->vBloomPower.z = m_vBloomPower_Mesh[2];
				}


				/* 메쉬 파티클 움직임 옵션 */
				ImGui::SeparatorText("");
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

				if (ImGui::DragFloat2("Power_Mesh", m_vMinMaxPower_Mesh, 10.f, 0.1f))
				{
					m_pMeshBufferDesc->vMinMaxPower.x = m_vMinMaxPower_Mesh[0];
					m_pMeshBufferDesc->vMinMaxPower.y = m_vMinMaxPower_Mesh[1];
				}


				/* 인스턴스 개수 변경 */
				ImGui::SeparatorText("");
				ImGui::Text("MaxInstance_Mesh : %d", m_iMaxNumInstance_Mesh);
				if (ImGui::DragInt("Instance Count", &m_iNumInstance_Mesh, 1, 1, m_iMaxNumInstance_Mesh))
				{
					m_pInstanceDesc->iCurNumInstance = m_iNumInstance_Mesh;
					m_pMeshBufferDesc->iCurNumInstance = m_iNumInstance_Mesh;
				}


				/* 퍼지는 범위 */
				if (ImGui::DragFloat2("MinMaxRange_Mesh", m_vMinMaxRange_Mesh, 1.f, 0.1f, 360.f))
				{
					if (m_vMinMaxRange_Mesh[0] > m_vMinMaxRange_Mesh[1])
						m_vMinMaxRange_Mesh[0] = m_vMinMaxRange_Mesh[1];

					m_pMeshBufferDesc->vMinMaxRange.x = m_vMinMaxRange_Mesh[0];
					m_pMeshBufferDesc->vMinMaxRange.y = m_vMinMaxRange_Mesh[1];
				}

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

			}
		}
	}
}

void CWindow_EffectTool::Update_TrailTab()
{
}

void CWindow_EffectTool::Update_CurParameters_Parts()
{
	if (nullptr != m_pCurPartEffect)
	{
		CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();
		CTransform* pPartTransform = m_pCurPartEffect->Get_Transform();

		m_vTimes_Part[0] = m_pCurPartEffect->Get_WaitingTime();
		m_vTimes_Part[1] = m_pCurPartEffect->Get_LifeTime();
		m_vTimes_Part[2] = m_pCurPartEffect->Get_RemainTime();

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
			m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
			CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
			m_pParticleBufferDesc = pVIBuffer->Get_Desc();

			m_pCurPartEffect->Set_RemainTime(m_vTimes_Part[2]);

			if (m_pParticleDesc->bUseSpriteAnim)
			{
				m_iTexIndex_Particle[CEffect_Void::TEXTURE_DIFFUSE] = m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE];
			}
			else
			{
				m_iTexIndex_Particle[CEffect_Void::TEXTURE_SPRITE] = m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_SPRITE];
			}

			m_iTexIndex_Particle[CEffect_Void::TEXTURE_MASK] = m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK];
			m_iTexIndex_Particle[CEffect_Void::TEXTURE_NOISE] = m_pParticleDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE];
			m_iShaderPassIndex_Particle = m_pParticleDesc->iShaderPassIndex;
			m_iRenderGroup_Particle = m_pParticleDesc->iRenderGroup;

			//m_iNumInstance = pVIBuffer->Get_NumInstance();
			m_iNumInstance_Particle = m_pParticleBufferDesc->iCurNumInstance; // 인스턴스 개수


			/* 빌보드 여부 */
			if (m_pParticleDesc->bBillBoard)
				m_iBillBoard = 0;
			else
				m_iBillBoard = 1;

			/* Z소팅 여부 */
			if (dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_SortZ())
				m_iSortZ = 0;
			else
				m_iSortZ = 1;

			/* 스프라이트 애니메이션 사용여부 */
			if (m_pParticleDesc->bUseSpriteAnim)
				m_iSprite_Particle = 1;
			else
				m_iSprite_Particle = 0;

			if (0 == m_iSprite_Particle)	// 스프라이트 애니메이션 사용이면
			{
				m_pSpriteDesc_Particle = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Sprite_Desc();

				m_vUV_MaxTileCount_Particle[0] = m_pSpriteDesc_Particle->vUV_MaxTileCount.x;
				m_vUV_MaxTileCount_Particle[1] = m_pSpriteDesc_Particle->vUV_MaxTileCount.y;

				m_fSequenceTerm_Particle = m_pSpriteDesc_Particle->fSequenceTerm;
			}

			/* UV회전 */
			m_fUV_RotDegree = m_pParticleDesc->fUV_RotDegree;

			/* 리지드바디 */
			/* 리지드바디 사용 여부 */
			if (m_pParticleDesc->bUseRigidBody)
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
				m_fFriction_Particle = m_pParticleBufferDesc->fFriction;	// 마찰 계수
				m_fSleepThreshold_Particle = m_pParticleBufferDesc->fSleepThreshold;	// 슬립 한계점


				/* 파워 */
				m_vMinMaxPower_Particle[0] = m_pParticleBufferDesc->vMinMaxPower.x;
				m_vMinMaxPower_Particle[1] = m_pParticleBufferDesc->vMinMaxPower.y;


				/* 질량(Mass) */
				m_vMinMaxMass_Particle[0] = m_pParticleBufferDesc->vMinMaxMass.x;
				m_vMinMaxMass_Particle[1] = m_pParticleBufferDesc->vMinMaxMass.y;

				/* 스피드 */
				m_vMinMaxSpeed_Particle[0] = m_pParticleBufferDesc->vMinMaxSpeed.x;
				m_vMinMaxSpeed_Particle[1] = m_pParticleBufferDesc->vMinMaxSpeed.y;

			}


			/* 라이프타임_파티클 */
			m_vMinMaxLifeTime_Particle[0] = m_pParticleBufferDesc->vMinMaxLifeTime.x;
			m_vMinMaxLifeTime_Particle[1] = m_pParticleBufferDesc->vMinMaxLifeTime.y;


			/* 퍼지는 범위(분포 범위) */
			m_vMinMaxRange_Particle[0] = m_pParticleBufferDesc->vMinMaxRange.x;
			m_vMinMaxRange_Particle[1] = m_pParticleBufferDesc->vMinMaxRange.y;


			/* 회전 범위 */
			m_vRotationOffsetX_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetX.x;
			m_vRotationOffsetX_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetX.y;

			m_vRotationOffsetY_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetY.x;
			m_vRotationOffsetY_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetY.y;

			m_vRotationOffsetZ_Particle[0] = m_pParticleBufferDesc->vMinMaxRotationOffsetZ.x;
			m_vRotationOffsetZ_Particle[1] = m_pParticleBufferDesc->vMinMaxRotationOffsetZ.y;

			/* 스케일 */
			m_vLerpScale_Pos_Particle[0] = m_pParticleBufferDesc->vLerpScale_Pos.x;
			m_vLerpScale_Pos_Particle[1] = m_pParticleBufferDesc->vLerpScale_Pos.y;

			m_vMinMaxWidth_Particle[0] = m_pParticleBufferDesc->vMinMaxWidth.x;
			m_vMinMaxWidth_Particle[1] = m_pParticleBufferDesc->vMinMaxWidth.y;

			m_vMinMaxHeight_Particle[0] = m_pParticleBufferDesc->vMinMaxHeight.x;
			m_vMinMaxHeight_Particle[1] = m_pParticleBufferDesc->vMinMaxHeight.y;

			m_vScaleSpeed_Particle[0] = m_pParticleBufferDesc->vScaleSpeed.x;
			m_vScaleSpeed_Particle[1] = m_pParticleBufferDesc->vScaleSpeed.y;


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

		}

		if (CEffect_Void::RECT == eType_Effect)
		{
			m_pRectDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Desc();
			CEffect_Void::DISTORTION_DESC* pDistortionDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Distortion_Desc();
			CEffect_Void::UVSPRITE_DESC* pSpriteDesc = dynamic_cast<CEffect_Rect*>(m_pCurPartEffect)->Get_Sprite_Desc();

			m_pRectDesc->iShaderPassIndex = { 2 };
			m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 1 };
			m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 17 };
			m_pRectDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 1 };


			m_fSequenceTerm_Distortion = pDistortionDesc->fSequenceTerm;
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


			m_fSequenceTerm_RectSprite = pSpriteDesc->fSequenceTerm;

			m_vUV_MaxTileCount[0] = pSpriteDesc->vUV_MaxTileCount.x;
			m_vUV_MaxTileCount[1] = pSpriteDesc->vUV_MaxTileCount.y;
		}

		if (CEffect_Void::MESH == eType_Effect)
		{
			m_pInstanceDesc = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_Desc();
			CVIBuffer_Effect_Model_Instance* pVIBuffer = dynamic_cast<CEffect_Instance*>(m_pCurPartEffect)->Get_VIBufferCom();
			m_pMeshBufferDesc = pVIBuffer->Get_Desc();

			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_DIFFUSE] = m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE];
			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_MASK] = m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_MASK];
			m_iTexIndex_Mesh[CEffect_Void::TEXTURE_NOISE] = m_pInstanceDesc->iTextureIndex[CEffect_Void::TEXTURE_NOISE];

			m_iShaderPassIndex_Mesh = m_pInstanceDesc->iShaderPassIndex;
			m_iRenderGroup_Mesh = m_pInstanceDesc->iRenderGroup;

			m_vColor_Clip_Part[0] = m_pInstanceDesc->vColor_Clip.x;
			m_vColor_Clip_Part[1] = m_pInstanceDesc->vColor_Clip.y;
			m_vColor_Clip_Part[2] = m_pInstanceDesc->vColor_Clip.z;
			m_vColor_Clip_Part[3] = m_pInstanceDesc->vColor_Clip.w;

			m_vMinMaxPower_Mesh[0] = m_pMeshBufferDesc->vMinMaxPower.x;
			m_vMinMaxPower_Mesh[1] = m_pMeshBufferDesc->vMinMaxPower.y;

			m_vMinMaxRange_Mesh[0] = m_pMeshBufferDesc->vMinMaxRange.x;
			m_vMinMaxRange_Mesh[1] = m_pMeshBufferDesc->vMinMaxRange.y;

			m_vRotationOffsetX_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetX.x;
			m_vRotationOffsetX_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetX.y;

			m_vRotationOffsetY_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetY.x;
			m_vRotationOffsetY_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetY.y;

			m_vRotationOffsetZ_Mesh[0] = m_pMeshBufferDesc->vMinMaxRotationOffsetZ.x;
			m_vRotationOffsetZ_Mesh[1] = m_pMeshBufferDesc->vMinMaxRotationOffsetZ.y;
		}


		if (CEffect_Void::TRAIL == eType_Effect)
		{


		}

	}

}

void CWindow_EffectTool::Select_EasingType(EASING_TYPE* eType)
{
	/* Easing Type : 이징 타입 */
	if (ImGui::CollapsingHeader(" Easing Types "))
	{
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
}


void CWindow_EffectTool::Update_EffectList()
{
	auto& style = ImGui::GetStyle();

	/* 비어있는 이펙트 객체 생성 */
	ImGui::SeparatorText("");
	if (ImGui::Button("         Create         ", ImVec2(ImGui::GetWindowContentRegionMax().x - style.WindowPadding.x, 30)))
	{
		Create_EffectObject(TEXT("Layer_Effect"));
	}

	ImGui::SeparatorText("");
	if (ImGui::Button("         Create Test        "))
	{
		CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(LEVEL_TOOL, LAYER_EFFECT, "save_test.json");
	}


	/* 이펙트 리스트 & 현재 이펙트 선택 */
	if (ImGui::ListBox(" Effects ", &m_iCurEffectIndex, m_szEffectNames, m_pEffects.size(), (_int)6))
	{
		wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szEffectNames[m_iCurEffectIndex]);
		m_pCurEffect = m_pEffects.find(strCurName)->second;

		/* 문자열 초기화 */
		m_iCurPartIndex = m_CurPartObjects.size();
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
			m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.begin()->second);
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


		// =========================================

		ImGui::SeparatorText(" Add Part Effect ");
		if (ImGui::Button(" Add Particle "))
		{
			Add_Part_Particle();
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Rect "))
		{
			Add_Part_Rect();
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Mesh "))
		{
			//Add_Part_Mesh(TEXT("Prototype_Component_Model_Xray_ManHeavy"));
			//Add_Part_Mesh(TEXT("Prototype_Component_Model_splineMesh_tornado"));
			//Add_Part_Mesh(TEXT("Prototype_Component_Model_ShieldDome"));
			Add_Part_Mesh(TEXT("Prototype_Component_Model_Particle_Test"));
		}
		ImGui::SameLine();
		if (ImGui::Button(" Add Trail "))
		{
			Add_Part_Trail();
		}

		ImGui::SeparatorText("");

		// =========================================

		/* 이펙트 파트오브젝트 리스트 & 현재 파트오브젝트 선택 */
		if (ImGui::ListBox(" Parts ", &m_iCurPartIndex, m_szPartNames, m_CurPartObjects.size(), (_int)6))
		{
			wstring strCurName = m_pGameInstance->Char_To_Wstring(m_szPartNames[m_iCurPartIndex]);
			m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_CurPartObjects.find(strCurName)->second);

			Update_CurParameters_Parts();
		}

		if (nullptr != m_pCurPartEffect)
		{
			CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();

			/* 선택 파티클 활성화 */
			if (ImGui::Button(" Active Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
					CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
					m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					m_pParticleDesc->bActive_Tool = TRUE;
				}

				if (CEffect_Void::RECT == eType_Effect)
				{
					m_pRectDesc->bActive_Tool = TRUE;
				}

				if (CEffect_Void::MESH == eType_Effect)
				{
					m_pInstanceDesc->bActive_Tool = TRUE;
				}
			}
			ImGui::SameLine();
			/* 선택 파티클 비활성화 */
			if (ImGui::Button(" UnActive Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
					CVIBuffer_Particle* pVIBuffer = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_VIBufferCom();
					m_pParticleBufferDesc = pVIBuffer->Get_Desc();

					m_pParticleDesc->bActive_Tool = FALSE;
				}
				if (CEffect_Void::RECT == eType_Effect)
				{
					m_pRectDesc->bActive_Tool = FALSE;
				}

				if (CEffect_Void::MESH == eType_Effect)
				{
					m_pInstanceDesc->bActive_Tool = FALSE;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(" ReSet_Part "))
			{
				if (CEffect_Void::PARTICLE == eType_Effect)
				{
					m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
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
		}


		/* Effect Edit Start */
		ImGui::SeparatorText("");
		ImGui::CollapsingHeader("Effect_Times");
		//if (ImGui::CollapsingHeader("Effect_Times"))
		{
			ImGui::Text("    Waiting   |   LifeTime   |   SequenceTime ");
			if (ImGui::DragFloat3("Times_Effect", m_vTimes_Effect, 0.2f, 0.f))
			{
				if (m_vTimes_Effect[0] > m_vTimes_Effect[1])
					m_vTimes_Effect[0] = m_vTimes_Effect[1];

				m_pCurEffectDesc->fWaitingTime = m_vTimes_Effect[0];

				m_pCurEffectDesc->fLifeTime = m_vTimes_Effect[1];
				m_pCurEffectDesc->fRemainTime = m_vTimes_Effect[2];
			}
			ImGui::Text("Total_Time : %.2f", m_vTimes_Effect[0] + m_vTimes_Effect[1] + m_vTimes_Effect[2]);
		}


		/* 루프 키고 끄기 */
		ImGui::SeparatorText("");
		ImGui::RadioButton(" Loop_Effect ", &m_iLoop, 0);  ImGui::SameLine();
		ImGui::RadioButton("None Loop_Effect", &m_iLoop, 1);
		if (0 == m_iLoop)
			m_pCurEffectDesc->bLoop = TRUE;
		else if (1 == m_iLoop)
			m_pCurEffectDesc->bLoop = FALSE;



		if (nullptr != m_pCurPartEffect)
		{
			ImGui::SeparatorText("");
			ImGui::CollapsingHeader("Part_Times");
			//if (ImGui::CollapsingHeader("Part_Times"))
			{
				CEffect_Void::TYPE_EFFECT eType_Effect = m_pCurPartEffect->Get_EffectType();

				ImGui::Text("    Waiting   |   LifeTime   |   SequenceTime ");
				if (ImGui::DragFloat3("Times_Part", m_vTimes_Part, 0.2f, 0.f))
				{
					m_pCurPartEffect->Set_WaitingTime(m_vTimes_Part[0]);
					m_pCurPartEffect->Set_LifeTime(m_vTimes_Part[1]);
					m_pCurPartEffect->Set_RemainTime(m_vTimes_Part[2]);

					if (CEffect_Void::PARTICLE == eType_Effect)
					{
						m_pParticleDesc = dynamic_cast<CEffect_Particle*>(m_pCurPartEffect)->Get_Desc();
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
			if (ImGui::DragFloat3("Effect_Scale", m_vScale_Part, 0.5f))
			{
				pTransform->Set_Scaling(m_vScale_Part[0], m_vScale_Part[1], m_vScale_Part[2]);
			}

			_float3 vRotated = pTransform->Get_Rotated();
			ImGui::Text("Effect Rotated  : %.2f %.2f %.2f", vRotated.x, vRotated.y, vRotated.z);
			if (ImGui::DragFloat3("Effect_Rotate", m_vRotate_Effect, 0.5f))
			{
				pTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Effect[0]));
				pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Effect[1]));
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
				if (ImGui::DragFloat3("Part_Scale", m_vScale_Part, 0.5f))
				{
					m_pCurPartEffect->Get_Transform()->Set_Scaling(m_vScale_Part[0], m_vScale_Part[1], m_vScale_Part[2]);
				}

				_float3 vRotated = pPartTransform->Get_Rotated();
				ImGui::Text("Part Rotated  : %.2f %.2f %.2f", vRotated.x, vRotated.y, vRotated.z);
				if (ImGui::DragFloat3("Part_Rotate", m_vRotate_Part, 0.5f))
				{
					pPartTransform->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[0]));
					pPartTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_vRotate_Part[1]));
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
	m_pCurEffectDesc = pEffect->Get_Desc();

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
		tParticleDesc.fSpeedPerSec = { 5.f };
		tParticleDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		//tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Particle_Base");
		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

		tParticleDesc.strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
		tParticleDesc.iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = { 0 };

		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 /*1*/ };

		tParticleDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tParticleDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

		tParticleDesc.iRenderGroup = { 9 };

		tParticleDesc.iShaderPassIndex = { 0 };

		tParticleDesc.bBillBoard = { TRUE };

		tParticleDesc.fUV_RotDegree = { 0.f };

		tParticleDesc.bPlay				= { TRUE };
		tParticleDesc.bUseSpriteAnim	= { FALSE };
		tParticleDesc.bUseRigidBody		= { TRUE };


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

		tParticleDesc.strPartTag = strName;
		tParticleDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Particle");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Particle"), strName, &tParticleDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Find_PartObject(strName));
		m_pCurPartEffect->Set_EffectType(CEffect_Void::PARTICLE);
		dynamic_cast<CEffect_Void*>(m_pCurPartEffect)->Set_Owner(m_pCurEffect);

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
#pragma endregion 리스트 문자열 관련 끝

		Update_CurParameters_Parts();

	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Rect()
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Rect::EFFECT_RECT_DESC tRectDesc = {};
		tRectDesc.fSpeedPerSec = { 5.f };
		tRectDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tRectDesc.eType = CEffect_Rect::SINGLE;
		tRectDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tRectDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 0 };

		tRectDesc.strTextureTag[CEffect_Void::TEXTURE_SPRITE] = TEXT("Prototype_Component_Texture_Effect_Sprite");
		tRectDesc.iTextureIndex[CEffect_Void::TEXTURE_SPRITE] = { 0 };

		tRectDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tRectDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

		tRectDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tRectDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };


		if (CEffect_Rect::TYPE::SINGLE == tRectDesc.eType)
		{
			tRectDesc.iShaderPassIndex = { 2 };
		}
		else
		{
			tRectDesc.iShaderPassIndex = { 1 };
		}

		tRectDesc.iRenderGroup = { 9 };


		tRectDesc.bPlay = { TRUE };


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

		tRectDesc.strPartTag = strName;
		tRectDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Rect");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Rect"), strName, &tRectDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Find_PartObject(strName));
		m_pCurPartEffect->Set_EffectType(CEffect_Void::RECT);
		m_pCurPartEffect->Set_Owner(m_pCurEffect);

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

		Update_CurParameters_Parts();
#pragma endregion

	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Mesh(wstring strModelTag)
{
	if (nullptr != m_pCurEffect)
	{
		CEffect_Instance::EFFECT_INSTANCE_DESC tMeshDesc = {};
		tMeshDesc.fSpeedPerSec = { 5.f };
		tMeshDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

		tMeshDesc.eType_Mesh = CEffect_Instance::MESH_PARTICLE;
		tMeshDesc.strTextureTag[CEffect_Void::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tMeshDesc.iTextureIndex[CEffect_Void::TEXTURE_DIFFUSE] = { 11 };

		tMeshDesc.strTextureTag[CEffect_Void::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tMeshDesc.iTextureIndex[CEffect_Void::TEXTURE_MASK] = { 0 };

		tMeshDesc.strTextureTag[CEffect_Void::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tMeshDesc.iTextureIndex[CEffect_Void::TEXTURE_NOISE] = { 0 };

		tMeshDesc.iShaderPassIndex = { 0 };

		tMeshDesc.iRenderGroup = { 9 };

		tMeshDesc.bBillBoard = { FALSE };

		tMeshDesc.strModelTag = strModelTag;

		tMeshDesc.bPlay = { TRUE };
		tMeshDesc.bUseRigidBody = { TRUE };

		tMeshDesc.vRimColor = { 2.f, 10.f, 255.f, 255.f };
		tMeshDesc.fRimPower = { 950.f };

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

		tMeshDesc.strPartTag = strName;
		tMeshDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Instance");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Instance"), strName, &tMeshDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Find_PartObject(strName));
		m_pCurPartEffect->Set_EffectType(CEffect_Void::MESH);
		dynamic_cast<CEffect_Void*>(m_pCurPartEffect)->Set_Owner(m_pCurEffect);


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


		Update_CurParameters_Parts();
#pragma endregion
	}

	return S_OK;
}

HRESULT CWindow_EffectTool::Add_Part_Trail()
{

	if (nullptr != m_pCurEffect)
	{
		CEffect_Trail::TRAIL_DESC	tTrailDesc = {};
		tTrailDesc.fSpeedPerSec = { 5.f };
		tTrailDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };


		tTrailDesc.strTextureTag[CEffect_Particle::TEXTURE_DIFFUSE] = TEXT("Prototype_Component_Texture_Effect_Diffuse");
		tTrailDesc.iTextureIndex[CEffect_Particle::TEXTURE_DIFFUSE] = { 0 };

		tTrailDesc.strTextureTag[CEffect_Particle::TEXTURE_MASK] = TEXT("Prototype_Component_Texture_Effect_Mask");
		tTrailDesc.iTextureIndex[CEffect_Particle::TEXTURE_MASK] = { 0 /*1*/ };

		tTrailDesc.strTextureTag[CEffect_Particle::TEXTURE_NOISE] = TEXT("Prototype_Component_Texture_Effect_Noise");
		tTrailDesc.iTextureIndex[CEffect_Particle::TEXTURE_NOISE] = { 0 };

		tTrailDesc.iRenderGroup = { 7 };

		tTrailDesc.iShaderPassIndex = { 1 };

		tTrailDesc.bBillBoard = TRUE;

		tTrailDesc.fUV_RotDegree = { 0.f };

		tTrailDesc.bPlay = { TRUE };

		tTrailDesc.bTrailOn = TRUE;
		tTrailDesc.vPos_0 = m_pCurEffect->Get_Transform()->Get_Position();
		tTrailDesc.vPos_1 = m_pCurEffect->Get_Transform()->Get_Position();

		tTrailDesc.iVtxCnt = 0;
		tTrailDesc.iMaxCnt = 24;




#pragma region 리스트 문자열 관련
		wstring strName = TEXT("");
		wstring strFrontName = TEXT("Part_Trail");

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

		tTrailDesc.strPartTag = strName;
		tTrailDesc.strProtoTag = TEXT("Prototype_GameObject_Effect_Trail");
		FAILED_CHECK(m_pCurEffect->Add_PartObject(TEXT("Prototype_GameObject_Effect_Trail"), strName, &tTrailDesc));

		m_CurPartObjects = *m_pCurEffect->Get_PartObjects();
		m_pCurPartEffect = dynamic_cast<CEffect_Void*>(m_pCurEffect->Find_PartObject(strName));
		m_pCurPartEffect->Set_EffectType(CEffect_Void::TRAIL);
		dynamic_cast<CEffect_Void*>(m_pCurPartEffect)->Set_Owner(m_pCurEffect);

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


		Update_CurParameters_Parts();
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
	if (nullptr != m_pCurEffect)
	{
		CEffect::EFFECT_DESC* pDesc = m_pCurEffect->Get_Desc();
		CTransform* pTransform = m_pCurEffect->Get_Transform();

		m_vTimes_Effect[0] = pDesc->fWaitingTime;
		m_vTimes_Effect[1] = pDesc->fLifeTime;
		m_vTimes_Effect[2] = pDesc->fSequenceTime;

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

						if (!keys.empty())
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
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Save"))
			{
				m_eDialogType = DIALOG_TYPE::SAVE_DIALOG;
				m_strDialogPath = "../Bin/DataFiles/Data_Effect/";

				OpenDialog(IMGUI_EFFECTTOOL_WINDOW);

			}
			if (ImGui::MenuItem("Load"))
			{
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
	if (nullptr != m_pCurEffect)
	{
		json Out_Json;
		m_pCurEffect->Write_Json(Out_Json);

		string strSavePath = strPath + "\\" + strFileName;
		CJson_Utility::Save_Json(strSavePath.c_str(), Out_Json);
	}
	else
	{


	}


	return S_OK;
}

HRESULT CWindow_EffectTool::Load_Function(string strPath, string strFileName)
{
	json In_Json;
	string strLoadPath = strPath + "\\" + strFileName;
	CJson_Utility::Load_Json(strLoadPath.c_str(), In_Json);

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
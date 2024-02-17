#include "stdafx.h"
#include "Window_EffectTool.h"

#include "GameInstance.h"

#include "Particle_Custom.h"

CWindow_EffectTool::CWindow_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
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
			Update_ParticleTab();
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

void CWindow_EffectTool::Update_ParticleTab()
{
	//if (ImGui::CollapsingHeader(" Particle ", ImGuiTabBarFlags_None))
	{
		Update_ListArea_Particle();

		ImGui::SeparatorText("");
		Update_PlayArea_Particle();

		ImGui::SeparatorText("");
		Update_AppearArea_Particle();

		ImGui::SeparatorText("");
		Update_ActionArea_Particle();

		ImGui::SeparatorText("");
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

	}
}

void CWindow_EffectTool::Update_Demo_Particle()
{


}

void CWindow_EffectTool::Update_ListArea_Particle()
{
	/* 기본 파티클 생성 */
	if (ImGui::Button(" Create "))
	{
		Create_NewParticle();
	}

	/* 선택 파티클 삭제 */
	ImGui::SameLine();
	if (ImGui::Button(" Delete "))
	{

	}

	if (nullptr != m_pCurParticle)
	{
		_float4 vParticlePos = m_pCurParticle->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		ImGui::Text("Particle Pos  : %.2f %.2f %.2f", vParticlePos.x, vParticlePos.y, vParticlePos.z);
	}


	/* 파티클 리스트 & 현재 파티클 선택 */
	if (ImGui::ListBox(" List ", &m_iCurParticleIndex, m_szParticleNames, m_pParticles.size(), 4))
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

}

void CWindow_EffectTool::Update_PlayArea_Particle()
{
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


	/* 재생, 정지, 리셋 */
	ImGui::SeparatorText(" ");
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


	/* 재생바 */
	if (nullptr != m_pCurParticle)
	{
		_float fLifeTimePosition = m_pCurParticle->Get_VIBufferCom()->Get_TimePosition();
		ImGui::SliderFloat("TimePos_Particle", &fLifeTimePosition, 0.f, m_pCurParticle->Get_VIBufferCom()->Get_Desc()->vMinMaxLifeTime.y);


		///* 현재 재생 위치(포지션) 이동 */
		//if (ImGui::SliderFloat(" ", &m_fLifeTimePositionEdit, 0.f, m_pCurParticle->Get_Desc()->vLifeTime.y))
		//{
		//	m_pCurParticle->Set_Play(FALSE);
		//	m_pCurParticle->Set_TimePosition(m_fLifeTimePositionEdit);
		//}

		if (ImGui::DragFloat2("MinMaxLifeTime", m_vMinMaxLifeTime, 1.f, 0.f, 360.f))
		{
			if (m_vMinMaxLifeTime[0] > m_vMinMaxLifeTime[1])
				m_vMinMaxLifeTime[0] = m_vMinMaxLifeTime[1];

			m_pCurParticle->Get_VIBufferCom()->Set_LifeTime(m_vMinMaxLifeTime[0], m_vMinMaxLifeTime[1]);
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
	if (ImGui::Button(" Tornado "))
	{
		if (nullptr != m_pCurParticle)
		{
			m_pCurParticle->Get_VIBufferCom()->Set_Type_Action(CVIBuffer_Particle_Point::TYPE_ACTION::TORNADO);
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
	/* 텍스처 선택 (이미지 보이게 수정하기) */
	if (ImGui::CollapsingHeader(" Texture ", ImGuiTabBarFlags_None))
	{
		if (nullptr != m_pCurParticle)
		{
			if (ImGui::DragInt("Texture", &m_iTextureIndex, 1, 0, 5))
				m_pCurParticle->Set_TextureIndex(CParticle_Custom::TEXTURE::TYPE_DIFFUSE, m_iTextureIndex);
		}
	}

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

	/* Color */
	ImGui::ColorEdit3("Particle Color 1", m_fColor_Particle, ImGuiColorEditFlags_None);
	if (nullptr != m_pCurParticle)
	{
		m_pCurParticle->Get_VIBufferCom()->Set_Color(m_fColor_Particle[0], m_fColor_Particle[1], m_fColor_Particle[2]);
	}

}

void CWindow_EffectTool::Update_ActionArea_Particle()
{
	if (nullptr != m_pCurParticle)
	{
		CVIBuffer_Particle_Point* pVIBuffer = m_pCurParticle->Get_VIBufferCom();

		if (ImGui::DragFloat2("MinMaxRange", m_vMinMaxRange, 1.f, 0.1f, 360.f))
		{
			if (m_vMinMaxRange[0] > m_vMinMaxRange[1])
				m_vMinMaxRange[0] = m_vMinMaxRange[1];

			pVIBuffer->Set_Range(m_vMinMaxRange[0], m_vMinMaxRange[1]);
		}


		/* 회전 범위(오프셋) */
		/* RotX */
		if (ImGui::DragFloat2("RotationX", m_vRotationX, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationX[0])
				m_vRotationX[0] = 0.f;

			if (m_vRotationX[0] > m_vRotationX[1])
				m_vRotationX[1] = m_vRotationX[0];

			pVIBuffer->Set_RotationOffset(MIN, AXIS_X, m_vRotationX[0]);
			pVIBuffer->Set_RotationOffset(MAX, AXIS_X, m_vRotationX[1]);
		}


		/* RotY */
		if (ImGui::DragFloat2("RotationY", m_vRotationY, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationY[0])
				m_vRotationY[0] = 0.f;

			if (m_vRotationY[0] > m_vRotationY[1])
				m_vRotationY[1] = m_vRotationY[0];

			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MIN, AXIS_Y, m_vRotationY[0]);
			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MAX, AXIS_Y, m_vRotationY[1]);
		}


		/* RotZ */
		if (ImGui::DragFloat2("RotationZ", m_vRotationZ, 1.f, 0.f, 360.f))
		{
			if (0 > m_vRotationZ[0])
				m_vRotationZ[0] = 0.f;

			if (m_vRotationZ[0] > m_vRotationZ[1])
				m_vRotationZ[1] = m_vRotationZ[0];

			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MIN, AXIS_Z, m_vRotationZ[0]);
			m_pCurParticle->Get_VIBufferCom()->Set_RotationOffset(MAX, AXIS_Z, m_vRotationZ[1]);
		}
	}

	ImGui::SeparatorText("");
	/* 가속도 */
	if (nullptr != m_pCurParticle)
	{
		if (ImGui::DragFloat("AccPos_Particle", &m_fParticleAccPosition, 1.f, 0.f, 1.f))
			m_pCurParticle->Get_VIBufferCom()->Set_AccPosition(m_fParticleAccPosition);


		if (ImGui::DragFloat("ACC_Particle", &m_fParticleAcceleration, 1.f, 0.f, 100.f))
			m_pCurParticle->Get_VIBufferCom()->Set_Acceleration(m_fParticleAcceleration);
	}
}

void CWindow_EffectTool::Update_CurParameters()
{
	if (nullptr != m_pCurParticle)
	{
		CVIBuffer_Particle_Point* pVIBuffer = m_pCurParticle->Get_VIBufferCom();
		CVIBuffer_Particle_Point::PARTICLE_POINT_DESC* pDesc = pVIBuffer->Get_Desc();

		m_iNumInstance = pVIBuffer->Get_NumInstance();

		m_vMinMaxLifeTime[0] = pDesc->vMinMaxLifeTime.x;
		m_vMinMaxLifeTime[1] = pDesc->vMinMaxLifeTime.y;

		m_vMinMaxRange[0] = pDesc->vMinMaxRange.x;
		m_vMinMaxRange[1] = pDesc->vMinMaxRange.y;

		m_vRotationX[0] = pDesc->vMinMaxRotationOffsetX.x;
		m_vRotationY[0] = pDesc->vMinMaxRotationOffsetY.x;
		m_vRotationZ[0] = pDesc->vMinMaxRotationOffsetZ.x;

		m_vRotationX[1] = pDesc->vMinMaxRotationOffsetX.y;
		m_vRotationY[1] = pDesc->vMinMaxRotationOffsetY.y;
		m_vRotationZ[1] = pDesc->vMinMaxRotationOffsetZ.y;

		m_fParticleAcceleration = pDesc->fAcceleration;
		m_fParticleAccPosition = pDesc->fAccPosition;
	}
}



HRESULT CWindow_EffectTool::Ready_Layer_Particle(const wstring& strLayerTag)
{
	CParticle_Custom::PARTICLE_CUSTOM_DESC	tDesc = {};
	tDesc.fSpeedPerSec = { 5.f };
	tDesc.fRotationPerSec = { XMConvertToRadians(50.0f) };

	tDesc.strTextureTag = TEXT("Prototype_Component_Texture_Effect_Particle_Base");


	CParticle_Custom* pParticle = dynamic_cast<CParticle_Custom*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, TEXT("Prototype_GameObject_Particle_Custom"), &tDesc));

	wstring strName = Make_NameWithPin(TEXT("Particle"));
	m_pParticles.emplace(strName, pParticle);

	m_pCurParticle = pParticle;

	Update_CurParameters();

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
		const string utf8Str = m_pGameInstance->Wstring_To_UTF8(Pair.first);
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

	if (nullptr != m_pCurParticle)
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
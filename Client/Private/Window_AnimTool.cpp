#include "stdafx.h"
#include "Window_AnimTool.h"
#include "GameInstance.h"
#include "PreviewAnimationModel.h"
#include "CustomDialogFont.h"
#include "Model.h"
#include "Animation.h"
#include "Bone.h"
#include "Bounding_Sphere.h"
#include "Collider.h"
#include "Weapon_Player.h"
#include "Character.h"
#include "Weapon.h"
#pragma region Effect_Test
#include "Clone_Manager.h"
#include "Effect.h"
#include "Effect_Particle.h"
#pragma endregion

CWindow_AnimTool::CWindow_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImgui_Window(pDevice, pContext)
{

}


HRESULT CWindow_AnimTool::Initialize()
{
	//! 현재는 특별한 기능없음. 추후 필요할 것 같아서 셋팅.
	if(FAILED(__super::Initialize()))
		return E_FAIL;
	
	//! ImGuiFileDialog  파일별 색은 슈퍼에서 처리하는 거로 이동
	
	m_pGameInstance->Fill_PrototypeTags(&m_vObjectTag);

	m_pSphere = new BoundingSphere();
	//ID3D11Device::GetFeatureLevel();
	return S_OK;
}

void CWindow_AnimTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	__super::Begin();

	if (ImGui::Checkbox("RenderTargetOFF", &m_bRenderTargetOnOff))
	{
#ifdef _DEBUG
		m_pGameInstance->Set_RenderDebug(m_bRenderTargetOnOff);
#endif
		
	}
	//dialog========================================================================
	
	if (ImGui::Button(u8"저장하기")) { m_eDialogType = DIALOG_TYPE::SAVE_DIALOG; OpenDialog(CImgui_Window::IMGUI_ANIMATIONTOOL_WINDOW); } 
	ImGui::SameLine(); 
	if (ImGui::Button(u8"불러오기")) { m_eDialogType = CImgui_Window::LOAD_DIALOG; OpenDialog(CImgui_Window::IMGUI_ANIMATIONTOOL_WINDOW); }

	if (ImGui::Button(u8"승용이전용"))
	{
		m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"));
		m_pGameInstance->Get_CloneGameObjects(LEVEL_TOOL, &m_CreateList);
	}
	//disPlay
	ShowDialog();

	//===============================dialog============================================
	
	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Animation View", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Player"))
		{
			Draw_AnimationList(fTimeDelta);
			
			Draw_BoneList(fTimeDelta);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Monster")))
		{
			Draw_AnimationList(fTimeDelta);

			Draw_BoneList(fTimeDelta);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Event Editer"))
		{
			Draw_KeyEventEditer();

			ImGui::EndTabItem();
		}

		
		ImGui::EndTabBar();
	}

	BonePoint_Update();//콜라이더 렌더

	__super::End();

	ImGui::Begin("Weapon");
	if (ImGui::BeginTabBar("Weapon View", tab_bar_flags))
	{
		if (m_CreateList.size() > 0)
		{
			if (ImGui::BeginTabItem("Weapon"))
			{
				Draw_Weapon(fTimeDelta);

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CWindow_AnimTool::Render()
{
	__super::Begin();

	__super::End();
	
}

void CWindow_AnimTool::Call_UpdatePreViewModel()
{
}

void CWindow_AnimTool::Call_NextAnimationKey(const _uint& In_Key)
{
}

HRESULT CWindow_AnimTool::Read_EffectPath(const _tchar* StartDirectoryPath) //! 준호
{

	//! C++ 17부터 지원하는 filesystem을 이용해서 특정 경로안에 하위경로들을 전부 탐색 하여 fbx확장자들을 찾아준다
	//! 
	namespace fs = std::filesystem;

	//! 폴더명으로 타입을 분류하기위해
	wstring strDirName = {};
	
	for (const auto& entry : fs::recursive_directory_iterator(StartDirectoryPath))
	{
		
		if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json")
		{
			wstring strSearchPath = entry.path().wstring();

			fs::path PathObj(strSearchPath);

			wstring wstrFileName = PathObj.stem().wstring();
			//wstring wstrFBXPath = PathObj.parent_path() / wstrFileName;

			string strConvertEffectName;
			m_pGameInstance->WString_To_String(wstrFileName, strConvertEffectName);
			m_vecEffectName.push_back(strConvertEffectName);
		}
	}

	return S_OK;
}

void CWindow_AnimTool::Add_EffectKeyEvent()
{
	if (m_bEffectLoad == false)
	{
		Read_EffectPath(L"../Bin/DataFiles/Data_Effect/");
		m_bEffectLoad = true;
	}
	if (ImGui::BeginListBox("EffectList"))
	{
		static int Effect_idx = 0;
		for (int n = 0; n < m_vecEffectName.size(); n++)
		{
			const bool is_selected = (Effect_idx == n);
			if (ImGui::Selectable(m_vecEffectName[n].c_str(), is_selected))
				Effect_idx = n;
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
				if (m_bCreateEffect)
				{
					CEffect* pEffect = CClone_Manager::GetInstance()->Create_Effect(LEVEL_TOOL, LAYER_EFFECT, m_vecEffectName[n]+".json");
					if (m_pBones.size() > 0)// 본이 존재한다면 
					{
						_float4x4 BoneMatrix = {};
						BoneMatrix = m_pBones[m_iSelectBoneIndex]->Get_CombinedTransformationFloat4x4();
						m_EffectPosition.x = BoneMatrix._41;
						m_EffectPosition.y = BoneMatrix._42;
						m_EffectPosition.z = BoneMatrix._43;

						pEffect->Set_Position(m_EffectPosition);
						m_bCreateEffect = false;
					}
					else // 만약 본이 선택되지 않고 없다라면 0값으로 초기화
					{
						pEffect->Set_Position({ 0.0f, 0.0f, 0.0f });
					}
					if (m_bAddEffectposition) // 값을 넣어서 위치를 수정하고 싶다면 조절하고 조절 값을 여기에 다시 늘린다.
					{
						m_EffectPosition.x = m_EffectPosition.x + m_AddPositions[0];
						m_EffectPosition.y = m_EffectPosition.y + m_AddPositions[1];
						m_EffectPosition.z = m_EffectPosition.z + m_AddPositions[2];
						pEffect->Set_Position(m_EffectPosition);
					}
				}
				
			}

		}
		ImGui::EndListBox();
	}
	if (ImGui::Button(" EffectCreate "))
	{
		m_bCreateEffect = true;
	}
	ImGui::DragFloat3("Add_Trans", m_AddPositions);
	{
		m_bAddEffectposition = true;
	}
}

void CWindow_AnimTool::Add_EnableWeaponEvent(const _bool In_bEnable)
{
}

void CWindow_AnimTool::Add_SoundKeyEvent()
{
}

void CWindow_AnimTool::Add_RandomSoundKeyEvent()
{
}

HRESULT CWindow_AnimTool::Save_Function(string strPath, string strFileName)
{
	string strBody = "TypeBody";
	string strWeapon = "TypeWeapon";

	json SaveJson = {};
	json BodyJson = {};
	json WeaponJson = {};

	if (m_CreateList.size() > 0)
	{
		
		string ProtoType;
		m_pGameInstance->WString_To_String(m_PickingObject->Get_ProtoTypeTag(), ProtoType);
		BodyJson.emplace("Type", strBody);
		BodyJson.emplace("Tag", ProtoType);
		BodyJson.emplace("Layer", m_strLayer);
		BodyJson.emplace("SelectCreateIndex", m_iSelectCreateListIndex);
		BodyJson.emplace("AnimationName", m_pAnimation[m_CurrentAnimationIndex]->Get_Name());
		BodyJson.emplace("AnimIndex", m_CurrentAnimationIndex);
		BodyJson.emplace("AnimDuration", m_fDuration);
		BodyJson.emplace("CurrentTrackPosition", m_fCurrentTrackPosition);
		BodyJson.emplace("AnimationSpeed", m_fSpeed);
		if (m_pBones.size() > 0)
		{
			BodyJson.emplace("BoneName", m_pBones[m_iSelectBoneIndex]->Get_Name());
			BodyJson.emplace("BoneIndex", m_iSelectBoneIndex);
			BodyJson.emplace("ColliderSize", m_iColliderSize);
			BodyJson.emplace("ColliderTrackPositionOn", m_iColliderOnTrackPosition);
			BodyJson.emplace("ColliderTrackPositionOff", m_iColliderOffTrackPosition);
		}
		
		

	}

	if (m_CreateWeaponList.size() > 0)
	{
		string ProtoType;
		m_pGameInstance->WString_To_String(m_PickingWeapon->Get_ProtoTypeTag(), ProtoType);
		WeaponJson.emplace("Type", strWeapon);
		WeaponJson.emplace("WeaponTag", ProtoType);
		WeaponJson.emplace("AnimationName", m_pAnimation[m_CurrentAnimationIndex]->Get_Name());
		WeaponJson.emplace("AnimIndex", m_CurrentAnimationIndex);
		WeaponJson.emplace("AnimDuration", m_fDuration);
		WeaponJson.emplace("CurrentTrackPosition", m_fCurrentTrackPosition);
		WeaponJson.emplace("AnimationSpeed", m_fSpeed);
		if (m_pBones.size() > 0)
		{
			WeaponJson.emplace("BoneName", m_pBones[m_iSelectBoneIndex]->Get_Name());
			//m_PickingWeapon->Write_Json(WeaponJson);
			CJson_Utility::Write_Float3(WeaponJson["GuizmoTranslation"], m_fGuizmoTranslation);
			CJson_Utility::Write_Float3(WeaponJson["GuizmoRotation"], m_fGuizmoRotation);
			CJson_Utility::Write_Float3(WeaponJson["GuizmoScale"], m_fGuizmoScale);
			_float4x4 pPickingWeapon = dynamic_cast<CWeapon*>(m_PickingWeapon)->Get_WeaponWorldMatrix();
			for (_int i = 0; i < 4; ++i)
				CJson_Utility::Write_Float4(WeaponJson["WeaponWorldMatrix"][i], XMLoadFloat4x4(&pPickingWeapon).r[i]);

			WeaponJson.emplace("WeaponColliderSize", m_iColliderWeaponSize);
			WeaponJson.emplace("WeaponColliderTrackPositionOn", m_iColliderWeaponOnTrackPosition);
			WeaponJson.emplace("WeaponColliderTrackPositionOff", m_iColliderWeaponOffTrackPosition);
		}
	}

	SaveJson.emplace("Body", BodyJson);
	SaveJson.emplace("Weapon", WeaponJson);

	string strSavePath = strPath + "/" +strFileName+  "_AnimationData.json";
	if (FAILED(CJson_Utility::Save_Json(strSavePath.c_str(), SaveJson)))
	{
		MSG_BOX("애니메이션툴 저장 실패");
	}
	else
	{
		MSG_BOX("애니메이션툴 저장 성공");
	}

	return S_OK;
}

HRESULT CWindow_AnimTool::Load_Function(string strPath, string strFileName)
{
	json LoadJson = {};

	string strFullPath = strPath + "/" + strFileName;

	if (FAILED(CJson_Utility::Load_Json(strFullPath.c_str(), LoadJson)))
	{
		MSG_BOX("애니메이션툴 로드 실패");
		return E_FAIL;
	}
	else
	{
		Reset_AnimFunction();
	}

	json BodyJson = LoadJson["Body"];
	json WeaponJson = LoadJson["Weapon"];

	//Body
	{
		
		wstring strLoadprotoTag;
		string proto_Tag = BodyJson["Tag"];

		wstring strLoadLayer;
		string strJsonLayer = BodyJson["Layer"];

		m_pGameInstance->String_To_WString(proto_Tag, strLoadprotoTag);
		m_pGameInstance->String_To_WString(strJsonLayer, strLoadLayer);

		m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLoadLayer, strLoadprotoTag);

		m_pGameInstance->Get_CloneGameObjects(LEVEL_TOOL, &m_CreateList);
		m_bCloneCount = true;
		m_bListCheck = true;
		m_bCreateCheck = false;
		m_PickingObject = m_CreateList.back();
		if (m_CreateList.size() > 0)
		{
		
			CCharacter* pcharacters = dynamic_cast<CCharacter*>(m_PickingObject);
			if (pcharacters != nullptr)
			{
				m_pBody = pcharacters->Get_Body();
				m_pAnimation = *(pcharacters->Get_Body()->Get_Model()->Get_Animations());
				m_iAnimationNum = pcharacters->Get_Body()->Get_Model()->Get_AnimationNum();
			}
		
		
		}
		m_iSelectCreateListIndex = BodyJson["SelectCreateIndex"];

		m_CurrentAnimationIndex = BodyJson["AnimIndex"];

		m_fDuration = BodyJson["AnimDuration"];

		m_TargetTrackPosition = BodyJson["CurrentTrackPosition"];

		m_fSpeed = BodyJson["AnimationSpeed"];

		m_iSelectBoneIndex = BodyJson["BoneIndex"];
		//콜라이더 생성
		m_iColliderSize = BodyJson["ColliderSize"];
		//Create_Bounding(m_iColliderSize);
		m_iColliderOnTrackPosition = BodyJson["ColliderTrackPositionOn"];
		m_iColliderOffTrackPosition = BodyJson["ColliderTrackPositionOff"];
		
		
	}
	//Weapon
	if(WeaponJson != nullptr)
	{
		wstring strLoadWeaponTag;
		string strWeaponTag = WeaponJson["WeaponTag"];

		m_pGameInstance->String_To_WString(strWeaponTag, strLoadWeaponTag);

		CCharacter* pObject = dynamic_cast<CCharacter*>(m_PickingObject);

		m_pBones = *(pObject->Get_Body()->Get_Model()->Get_Bones());
		string strBoneName = WeaponJson["BoneName"];
		Create_Weapon(pObject, strBoneName, strLoadWeaponTag);

		m_PickingWeapon = m_CreateWeaponList.back();

		//CJson_Utility::Load_JsonFloat4x4(WeaponJson["WeaponWorldMatrix"], m_fWeaponWorldMatrix);

		//dynamic_cast<CWeapon*>(m_PickingWeapon)->Set_WorldMatrix(m_fWeaponWorldMatrix);

		//CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_PickingWeapon);
		//
		//_float4x4 pPickObject = m_PickingWeapon->Get_Transform()->Get_WorldMatrix();
		//m_fWeaponMatrix = pPickObject;
		//m_fWeaponPos.x = m_fWeaponMatrix._41;
		//m_fWeaponPos.y = m_fWeaponMatrix._42;
		//m_fWeaponPos.z = m_fWeaponMatrix._43;
		CJson_Utility::Load_Float3(WeaponJson["GuizmoTranslation"], m_fGuizmoTranslation);
		CJson_Utility::Load_Float3(WeaponJson["GuizmoRotation"], m_fGuizmoRotation);
		CJson_Utility::Load_Float3(WeaponJson["GuizmoScale"], m_fGuizmoScale);

		m_iColliderWeaponSize = WeaponJson["WeaponColliderSize"];
		//Create_Weapon_Bounding(m_fWeaponPos, m_iColliderWeaponSize);

		m_iColliderWeaponOnTrackPosition = WeaponJson["WeaponColliderTrackPositionOn"];
		m_iColliderWeaponOffTrackPosition = WeaponJson["WeaponColliderTrackPositionOff"];
	}

	return S_OK;
}

void CWindow_AnimTool::Reset_AnimFunction()
{


	if (m_CreateWeaponList.size() > 0)
	{
		for (int i = 0; i < m_CreateWeaponList.size(); ++i)
		{
			
			m_CreateWeaponList[i]->Set_Dead(true);
			for (int j = 0; j < m_iCreateWeaponColliderNum; ++j)
			{
				if (m_pWeaponCollider[j] == nullptr)
					m_pWeaponCollider.erase(m_pWeaponCollider.begin() + j);
				--m_iCreateWeaponColliderNum;
			}
		}
	}
	
	if (m_CreateList.size() > 0)
	{
		for (int i = 1; i < m_CreateList.size(); ++i)
		{
		
			m_CreateList[i]->Set_Dead(true);
			for (int j = 0; j < m_iCreateColliderNum; ++j)
			{
				if(m_pBoneCollider[j] == nullptr)
					m_pBoneCollider.erase(m_pBoneCollider.begin() + j);
				--m_iCreateColliderNum;
			}

		}
	}

	m_CreateWeaponList.clear();
	m_pAnimation.clear();
	m_pBoneCollider.clear();
	m_pWeaponCollider.clear();
	m_pBones.clear();
	m_vBoneColliderIndex.clear();

	m_fSpeed = 1.f;
	m_fDuration = 0.0f;
	m_fCurrentTrackPosition = 0.0f;

	m_iColliderSize = 0.0f;
	m_iColliderOnTrackPosition = 0.0f;
	m_iColliderOffTrackPosition = 0.0f;

	m_iColliderWeaponSize = 0.0f;
	m_iColliderWeaponOnTrackPosition = 0.0f;
	m_iColliderWeaponOffTrackPosition = 0.0f;

	m_fBonePosition[3] = { 0.f };
	m_fWeaponPosition[3] = { 0.f };

	m_fWeaponPos = { 0.f,0.f,0.f };

	m_fBoneMatrix = {};
	m_fWeaponMatrix = {};

	m_CurrentAnimationIndex = 0;
	m_iCreateObjectSize = 0;
	m_iCreateWeaponSize = 0;

	m_iSelectCreateListIndex = 0;
	m_iAnimationNum = 0;
	m_iBoneNum = 0;
	m_iSelectBoneIndex = 0;
	m_iCreateColliderNum = 0;
	m_iSelectColliderIndex = 0;
	m_iCreateWeaponColliderNum = 0;
	m_iSelectWeaponColliderIndex = 0;

}

void CWindow_AnimTool::Save_KeyEvent()
{
}

HRESULT CWindow_AnimTool::Load_KeyEvent()
{
	return E_NOTIMPL;
}

void CWindow_AnimTool::Clear_KeyEvent()
{
}

void CWindow_AnimTool::ClearBack_KeyEvent()
{
}

void CWindow_AnimTool::ClearAll_KeyEvent()
{
}

void CWindow_AnimTool::Clear_WeaponEvent()
{
}

void CWindow_AnimTool::Create_Object(const wstring& strLayerTag, const wstring& strPrototypeTag)
{
	m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, strPrototypeTag);
// 	if (strLayerTag == L"Layer_Player")
// 	{
// 		m_pGameInstance->Set_Player(dynamic_cast<CCharacter*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, strPrototypeTag)));
// 	}
// 	else
// 		m_pGameInstance->Add_CloneObject_And_Get(LEVEL_TOOL, strLayerTag, strPrototypeTag);

}

void CWindow_AnimTool::Create_Weapon(CCharacter* ParentObject, string strBonename, const wstring& strPrototypeTag)
{

	//_float4x4 Temp = m_CreateList[m_iSelectCreateListIndex]->Get_Transform()->Get_WorldMatrix();
	//_float4x4 Desc = Temp * m_pBones[m_iSelectBoneIndex]->Get_CombinedTransformationMatrix();
	CWeapon_Player::WEAPON_DESC weaponDesc = {};

	weaponDesc.m_pSocketBone = m_pBones[m_iSelectBoneIndex];
	weaponDesc.m_pParentTransform = m_CreateList[m_iSelectCreateListIndex]->Get_Transform();
	

	const wstring str = TEXT("Weapon") + m_CreateWeaponList.size();

	ParentObject->Add_Weapon(strPrototypeTag, strBonename, weaponDesc, str);

	CGameObject* pWeapon = ParentObject->Get_Weapon(str);

	m_CreateWeaponList.push_back(pWeapon);
}



void CWindow_AnimTool::Draw_KeyEventEditer()
{
	Add_EffectKeyEvent();
}

void CWindow_AnimTool::Draw_AnimationList(_float fTimeDelta)
{
	ImGui::SeparatorText("AnimationModel");
	ImGui::NewLine();

	if (ImGui::TreeNode("AnimationModel"))
	{
		string items[] = { "Layer_Player", "Layer_Monster","Layer_Environment","Layer_Object","Layer_Effect","Layer_Something"};

		static int Object_idx = 0; // Here we store our selection data as an index.
		static int Layer_idx = 0; // Here we store our selection data as an index.
		int ObjectTagSize = (int)m_vObjectTag.size();

		if (ImGui::BeginListBox("ObjectList"))
		{
			for (int n = 0; n < ObjectTagSize; n++)
			{
				const bool is_selected = (Object_idx == n);
				if (ImGui::Selectable(m_vObjectTag[n].c_str(), is_selected))
					Object_idx = n;
				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::SeparatorText("LayerList");
		ImGui::NewLine();
		if (ImGui::BeginListBox("LayerList"))
		{
			for (int n = 0; n <6; n++)
			{
				const bool is_selected = (Layer_idx == n);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
					Layer_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					if (m_bCreateCheck)
						if (m_pGameInstance->Mouse_Down(DIM_LB))
						{
							Create_Object(ConvertCtoWC(items[Layer_idx].c_str()), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							m_strLayer = items[Layer_idx].c_str();
							//Create_Object_On_Map(ConvertCtoWC(items[Layer_idx].c_str()), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							m_bCloneCount = true;
							m_bListCheck = true;
							m_bCreateCheck = false;
							
						}
					
				}
				
			}
			
			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::Checkbox("Create",&m_bCreateCheck);
		ImGui::SameLine();
		ImGui::Checkbox("Delete",& m_bDeleteCheck);

		ImGui::SeparatorText("CreateList");
		ImGui::NewLine();
		static int CreateIndex = 0; // Here we store our selection data as an index.
		
		if (m_bListCheck)
		{
			m_iCreateObjectSize = (_int)m_CreateList.size();
			if (ImGui::BeginListBox("CreateList"))
			{

				for (int n = 0; n < m_iCreateObjectSize; n++)
				{
					string str ="Object";
					string str2 = to_string(n);

					const bool is_selected = (CreateIndex == n);
					if (ImGui::Selectable((str + "." + str2).c_str(), is_selected))
						CreateIndex = n;
					m_iSelectCreateListIndex = CreateIndex;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						m_PickingObject = m_CreateList[CreateIndex];
						ImGui::SetItemDefaultFocus();
						if (m_bDeleteCheck)
						{
							_bool isdead = true;
							m_CreateList[CreateIndex]->Set_Dead(isdead);
							m_CreateList.erase(m_CreateList.begin() + CreateIndex);
							m_bDeleteCheck = false;
						}
							
					}
				}
				ImGui::EndListBox();
			}
			
		}

		ImGui::Checkbox(u8"기즈모on/off", &m_bguizmo);
		if (m_bguizmo)
		{
			if (nullptr == m_PickingObject)
				return;
		
			Set_GuizmoCamProj();
			Set_GuizmoCamView();
			Set_Guizmo(m_PickingObject);
		}
		
		ImGui::Spacing();
		ImGui::SeparatorText("AnimationList");
		ImGui::NewLine();
		
		if (ImGui::BeginListBox("AnimationList"))
		{
			if (m_CreateList.size() > 0)
			{
				CCharacter* pcharacters = dynamic_cast<CCharacter*>(m_CreateList.back());
				m_pBody = pcharacters->Get_Body();
				m_pAnimation = *(pcharacters->Get_Body()->Get_Model()->Get_Animations());
				m_iAnimationNum = pcharacters->Get_Body()->Get_Model()->Get_AnimationNum();

			}
			static int AnimationIndex = 0;
			for (int n = 0; n < (_int)m_iAnimationNum; n++)
			{
				const bool is_selected = (AnimationIndex == n);
				if (ImGui::Selectable(m_pAnimation[n]->Get_Name(), is_selected))
					AnimationIndex = n;
				m_CurrentAnimationIndex = AnimationIndex;
				if (is_selected)
				{
					
					ImGui::SetItemDefaultFocus();
					if (m_bFirstcheck == true)
					{
						m_fDuration = m_pAnimation[AnimationIndex]->Get_Duration();
						m_iColliderOffTrackPosition = m_fDuration;
						m_iColliderWeaponOffTrackPosition = m_fDuration;
						m_fCurrentTrackPosition = m_pAnimation[AnimationIndex]->Get_TrackPosition();
						m_pBody->Get_Model()->Set_Animation(AnimationIndex, CModel::ANIM_STATE_LOOP);
						m_bFirstcheck = false;
					}

					if (m_bTrackPositionCheck)
					{
						m_pAnimation[AnimationIndex]->Set_TrackPosition(m_fCurrentTrackPosition);
						m_bTrackPositionCheck = false;
					}
					
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (ImGui::Button(" Play "))
	{
		if (nullptr != m_pBody)
		{
			m_bFirstcheck = true;
			m_bStop = false;
			m_pBody->Get_Model()->Play_Animation(fTimeDelta, true);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(" Stop "))
	{
		m_bStop = true;
		m_pAnimation[m_CurrentAnimationIndex]->Set_TrackPosition(m_pAnimation[m_CurrentAnimationIndex]->Get_TrackPosition());
	}
	if (m_bStop)
	{
		if (nullptr != m_pBody)
			{
				m_pBody->Get_Model()->Set_Animation(m_pAnimation[m_CurrentAnimationIndex]->Get_TrackPosition(),CModel::ANIM_STATE_LOOP);
			}
	}
	
	if (nullptr != m_pBody)
	{
		m_fCurrentTrackPosition = m_pAnimation[m_CurrentAnimationIndex]->Get_TrackPosition();


	}
	
	
	ImGui::Spacing();
	//if (ImGui::SliderFloat("CurrnetTrackPosition", &m_fCurrentTrackPosition, 0.f, m_fDuration))
	
	if (ImGui::SliderFloat("TrackPosition", &m_fCurrentTrackPosition, 0.f, m_fDuration))
	{
		m_bTrackPositionCheck = true;
	}
	
	
// 
	if (ImGui::InputFloat("TrackPosition", &m_fCurrentTrackPosition, 0.f, m_fDuration))
	{
		m_bTrackPositionCheck = true;
	}

	//속도 값은 거꾸로 생각해야 함 ! 1/n의 분수 형태임 
	if (ImGui::SliderFloat("AnimationSpeed", &m_fSpeed, 0.f, 1000.f))
	{
		if (nullptr != m_pBody)
		{
			m_pBody->Get_Model()->Set_StiffnessRate(m_fSpeed);
		}
	}
	
	if (ImGui::InputFloat("AnimationSpeed", &m_fSpeed, 0.f, 1000.f))
	{
		if (nullptr != m_pBody)
		{
			m_pBody->Get_Model()->Set_StiffnessRate(m_fSpeed);
		
		}
		
	}
	
	if (m_bCloneCount)
	{
		m_CreateList.clear();
		m_pGameInstance->Get_CloneGameObjects(LEVEL_TOOL, &m_CreateList);

		m_bCloneCount = false;
	}
}

void CWindow_AnimTool::Draw_BoneList(_float fTimeDelta)
{
	if (m_PickingObject == nullptr)
		return;
	static int BoneIndex = 0;

	//static int ColliderIndex = 0;
	if (ImGui::TreeNode("ModelBones"))
	{
		ImGui::SeparatorText("BoneList");
		ImGui::NewLine();
		if (ImGui::BeginListBox("BoneList"))
		{
			if (m_PickingObject != nullptr)
			{
				CCharacter* pcharacters = dynamic_cast<CCharacter*>(m_PickingObject);
				/*m_pBody = pcharacters->Get_Body();*/ //위에서 넣어주고 있어서 여기서 굳이 또 할필요 없음 
				m_pBones = *(pcharacters->Get_Body()->Get_Model()->Get_Bones());
				m_iBoneNum = (_uint)m_pBones.size();

			}
			//m_PickingObject

			for (_uint n = 0; n < m_iBoneNum; n++)
			{
				const bool is_selected = (BoneIndex == n);
				if (ImGui::Selectable(m_pBones[n]->Get_Name(), is_selected))
					BoneIndex = n;
				m_iSelectBoneIndex = BoneIndex;
				//m_pBoneCollider.reserve(m_iBoneNum);

				if (is_selected)
				{                                                                              
					ImGui::SetItemDefaultFocus();
					if (m_bCreatCollider)
					{
						//m_fBoneMatrix = m_pBones[BoneIndex]->Get_CombinedTransformationMatrix();
						//_float4x4 pPickObject = m_PickingObject->Get_Transform()->Get_WorldMatrix();
						//m_fBoneMatrix = pPickObject * m_fBoneMatrix;
						//m_fBonePosition.x = m_fBoneMatrix._41;
						//m_fBonePosition.y = m_fBoneMatrix._42;
						//m_fBonePosition.z = m_fBoneMatrix._43;
						Create_Bounding(m_iColliderSize);
						m_vBoneColliderIndex.push_back(m_pBones[BoneIndex]);
						m_bCreatCollider = false;

						
						
						
					}
				}    
					
			}
			ImGui::EndListBox();
		}
		ImGui::SeparatorText("ColliderList");
		ImGui::NewLine();
		if (ImGui::BeginListBox("ColliderList"))
		{
			if (m_pBoneCollider.size() <0 )
				return;

			//m_PickingObject
			//m_strTest = m_pBones[BoneIndex]->Get_Name();

			static int iSelectColliderIndex;

			for (_uint n = 0; n < m_iCreateColliderNum; n++)
			{
				if (m_iCreateColliderNum == 0)
					break;
				string str = "Collider";
				string str2 = to_string(n);

				const bool is_selected = (iSelectColliderIndex == n);
				if (ImGui::Selectable((str + "." + str2).c_str(), is_selected))
					iSelectColliderIndex = n;
				m_iSelectColliderIndex = iSelectColliderIndex;
				m_pBoneCollider.reserve(m_iBoneNum);

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					m_pBoneCollider[iSelectColliderIndex]->Set_isCollision(true);
					if (m_bColliderSize)
					{
						//m_pSphere->Radius = m_iColliderSize;
						_float4x4	Temp = XMMatrixIdentity();
						Temp.m[0][0] = m_iColliderSize;
						Temp.m[1][1] = m_iColliderSize;
						Temp.m[2][2] = m_iColliderSize;

						m_pBoneCollider[iSelectColliderIndex]->Get_Bounding()->Set_matScale(Temp);
						
					}
					if (m_bDeleteCollider)
					{
						CComponent* pDeleteCollider = m_pBoneCollider[iSelectColliderIndex];

						pDeleteCollider->Set_Enable(false);

						m_PickingObject->Remove_Component(TEXT("Com_Collider"), &pDeleteCollider);

						m_pBoneCollider.erase(m_pBoneCollider.begin() + iSelectColliderIndex);
						
						Safe_Release(pDeleteCollider);

						--m_iCreateColliderNum;

						m_bDeleteCollider = false;

						break;
					}
				
					
				}
				else
				{
					m_pBoneCollider[n]->Set_isCollision(false);
				}
				if (m_pBoneCollider.size() > 0)
				{
					m_fBoneMatrix = XMMatrixIdentity();
					m_fBoneMatrix._41 = m_fBonePosition[0];
					m_fBoneMatrix._42 = m_fBonePosition[1];
					m_fBoneMatrix._43 = m_fBonePosition[2];
					_float4x4 Temp = m_fBoneMatrix + m_vBoneColliderIndex[n]->Get_CombinedTransformationMatrix();

					m_pBoneCollider[n]->Update(Temp);
				}
				
				
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
	//애니메이션 콜라이더 끄고 키는 조건 
	if (m_pBoneCollider.size() > 0)
	{
		if (m_fCurrentTrackPosition >= m_iColliderOnTrackPosition && m_fCurrentTrackPosition < m_iColliderOffTrackPosition)
		{
			m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(true);
		}
		else
		{
			m_pBoneCollider[m_iSelectColliderIndex]->Set_Enable(false);
		}
	}


	//현재 해야 하는 것은 콜라이더 생성하는 버튼을 일단 만들어 보자 
	ImGui::SeparatorText("Create/Delete");

	if (ImGui::Button("Collider Crate"))
	{
		m_bCreatCollider = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Collider Delete"))
	{
		m_bDeleteCollider = true;
	}
	ImGui::SeparatorText("ColliderSize");

	if (ImGui::DragFloat("ColliderSize", &m_iColliderSize, 0.f, 10.f))
	{
		m_bColliderSize = true;

	}

	if (ImGui::DragFloat3("ColliderPosition", m_fBonePosition, 0.01, -100.f, 100.f));

	ImGui::SeparatorText("ColliderOn");
	if (ImGui::InputFloat("ColliderOn", &m_iColliderOnTrackPosition, 0.01f, 1.f));
	ImGui::SeparatorText("ColliderOff");
	if (ImGui::InputFloat("ColliderOff", &m_iColliderOffTrackPosition, 0.01f, 1.f));

}

void CWindow_AnimTool::Draw_Weapon(_float fTimeDelta)
{
	ImGui::SeparatorText("WeaponList");

	if (m_pBones.empty())
		return;

	if (ImGui::TreeNode("Weapon"))
	{
		string items[] = { "Layer_Player", "Layer_Monster","Layer_Effect" };

		static int Object_idx = 0; // Here we store our selection data as an index.
		static int Layer_idx = 0; // Here we store our selection data as an index.
		int ObjectTagSize = (int)m_vObjectTag.size();

		if (ImGui::BeginListBox("ObjectList"))
		{
			for (int n = 0; n < ObjectTagSize; n++)
			{
				const bool is_selected = (Object_idx == n);
				if (ImGui::Selectable(m_vObjectTag[n].c_str(), is_selected))
					Object_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

			}
			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::SeparatorText("LayerList");
		ImGui::NewLine();
		if (ImGui::BeginListBox("LayerList"))
		{
			for (int n = 0; n < 3; n++)
			{
				const bool is_selected = (Layer_idx == n);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
					Layer_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					if (m_bCreateWeaponCheck)
						if (m_pGameInstance->Mouse_Down(DIM_LB))
						{
							
							CCharacter* pickingObject = dynamic_cast<CCharacter*>(m_PickingObject);
							Create_Weapon(pickingObject,m_pBones[m_iSelectBoneIndex]->Get_Name(), ConvertCtoWC(m_vObjectTag[Object_idx].c_str()));
							m_bCloneCount = true;
							m_bListCheck = true;
							m_bCreateWeaponCheck = false;
						}

				}

			}

			ImGui::EndListBox();
		}

		ImGui::Spacing();
		ImGui::Checkbox("Create", &m_bCreateWeaponCheck);
		ImGui::SameLine();
		ImGui::Checkbox("Delete", &m_bDeleteWeaponCheck);

		ImGui::SeparatorText("CreateWeaponList");
		ImGui::NewLine();
		static int CreateIndex = 0; // Here we store our selection data as an index.

		if (m_bListCheck)
		{
			m_iCreateWeaponSize = (_int)m_CreateWeaponList.size();
			if (ImGui::BeginListBox("CreateList"))
			{

				for (int n = 0; n < m_iCreateWeaponSize; n++)
				{
					string str = "Weapon";
					string str2 = to_string(n);

					const bool is_selected = (CreateIndex == n);
					if (ImGui::Selectable((str + "." + str2).c_str(), is_selected))
						CreateIndex = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						CWeapon* test = dynamic_cast<CWeapon*>(m_CreateWeaponList[CreateIndex]);
						m_PickingWeapon = test;
						
						ImGui::SetItemDefaultFocus();

						if (m_bCreatWeaponCollider)
						{
							//_float3 Temp = m_PickingWeapon->Get_Transform()->
						
							//m_fWeaponMatrix = m_CreateWeaponList[CreateIndex]->Get_Transform()->Get_WorldMatrix();
							//m_PickingWeapon->Set_WorldMatrix(m_PickingObject->Get_Transform()->Get_WorldMatrix());
							_float4x4 pPickObject = m_PickingWeapon->Get_Transform()->Get_WorldMatrix();
							m_fWeaponMatrix = pPickObject;
							m_fWeaponPos.x = m_fWeaponMatrix._41;
							m_fWeaponPos.y = m_fWeaponMatrix._42;
							m_fWeaponPos.z = m_fWeaponMatrix._43;
							Create_Weapon_Bounding(m_fWeaponPos,m_iColliderWeaponSize);
							m_bCreatWeaponCollider = false;
						}

						if (m_bDeleteWeaponCheck)
						{
							_bool isdead = true;
							m_CreateWeaponList[CreateIndex]->Set_Dead(isdead);
							m_CreateWeaponList.erase(m_CreateWeaponList.begin() + CreateIndex);
							m_bDeleteWeaponCheck = false;
						}

					}
				}
				ImGui::EndListBox();
			}

		}
		ImGui::Checkbox(u8"기즈모on/off", &m_bWeaponguizmo);
		if (m_bWeaponguizmo)
		{
			if (nullptr == m_PickingWeapon)
				return;
			/*	ImGuizmo_Initialize();*/
			Set_GuizmoCamProj();
			Set_GuizmoCamView();
			Set_Guizmo(m_PickingWeapon);

			m_fGuizmoTranslation = Get_GuizmoTranslation();
			m_fGuizmoRotation = Get_GuizmoRotation();
			m_fGuizmoScale = Get_GuizmoScale();
		}

		ImGui::SeparatorText("CreateWeaponCollider");
		if (ImGui::BeginListBox("CreateWeaponColliderList"))
		{
			if (m_pWeaponCollider.size() < 0)
				return;

			static int iSelectColliderIndex;

			for (_uint n = 0; n < m_iCreateWeaponColliderNum; n++)
			{
				if (m_iCreateWeaponColliderNum == 0)
					break;

				string str = "ColliderWeapon";
				string str2 = to_string(n);

				const bool is_selected = (iSelectColliderIndex == n);
				if (ImGui::Selectable((str + "." + str2).c_str(), is_selected))
					iSelectColliderIndex = n;
				m_iSelectWeaponColliderIndex = iSelectColliderIndex;
				

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
					m_pWeaponCollider[iSelectColliderIndex]->Set_isCollision(true);
					if (m_bColliderWeaponSize)
					{
						_float4x4	Temp = XMMatrixIdentity();
						Temp.m[0][0] = m_iColliderWeaponSize;
						Temp.m[1][1] = m_iColliderWeaponSize;
						Temp.m[2][2] = m_iColliderWeaponSize;

						m_pWeaponCollider[iSelectColliderIndex]->Get_Bounding()->Set_matScale(Temp);

					}
					if (m_bDeleteWeaponCollider)
					{
						CComponent* pDeleteCollider = m_pWeaponCollider[iSelectColliderIndex];

						m_PickingWeapon->Remove_Component(TEXT("Com_Collider"), &pDeleteCollider);

						m_pWeaponCollider.erase(m_pWeaponCollider.begin() + iSelectColliderIndex);

						Safe_Release(pDeleteCollider);

						--m_iCreateWeaponColliderNum;

						m_bDeleteWeaponCollider = false;

						break;
					}


				}
				else
				{
					m_pWeaponCollider[n]->Set_isCollision(false);
				}
				if (m_pWeaponCollider.size() > 0)
				{
					m_fWeaponMatrix = XMMatrixIdentity(); // 1 1 1 
					m_fWeaponMatrix._41 = m_fWeaponPosition[0];
					m_fWeaponMatrix._42 = m_fWeaponPosition[1];
					m_fWeaponMatrix._43 = m_fWeaponPosition[2];

					m_fWeaponWorldMatrix = dynamic_cast<CWeapon*>(m_PickingWeapon)->Get_WeaponWorldMatrix();
					m_fWeaponWorldMatrix = m_fWeaponMatrix + m_fWeaponWorldMatrix; //

					m_pWeaponCollider[n]->Update(m_fWeaponWorldMatrix);
				}
				
				//_float4x4 Temp = m_fWeaponMatrix + m_pBones[m_iSelectBoneIndex]->Get_CombinedTransformationMatrix();
				//
				//m_pBoneCollider[n]->Update(Temp);
			}


			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (m_pWeaponCollider.size() > 0)
	{
		if (m_fCurrentTrackPosition >= m_iColliderOnTrackPosition && m_fCurrentTrackPosition < m_iColliderOffTrackPosition)
		{
			m_pWeaponCollider[m_iSelectWeaponColliderIndex]->Set_Enable(true);
		}
		else
		{
			m_pWeaponCollider[m_iSelectWeaponColliderIndex]->Set_Enable(false);
		}
	}


	//현재 해야 하는 것은 콜라이더 생성하는 버튼을 일단 만들어 보자 
	ImGui::SeparatorText("Create/Delete");

	if (ImGui::Button("ColliderWeapon Crate"))
	{
		m_bCreatWeaponCollider = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("ColliderWeapon Delete"))
	{
		m_bDeleteWeaponCollider = true;
	}
	ImGui::SeparatorText("ColliderWeaponSize");

	if (ImGui::DragFloat("ColliderWeaponSize", &m_iColliderWeaponSize, 0.f, 10.f))
	{
		m_bColliderWeaponSize = true;

	}

	if (ImGui::DragFloat3("ColliderPosition", m_fWeaponPosition, 0.01, -100.f, 100.f));

	ImGui::SeparatorText("ColliderWeaponOn");
	if (ImGui::InputFloat("ColliderWeaponOn", &m_iColliderWeaponOnTrackPosition, 0.01f, 1.f));
	ImGui::SeparatorText("ColliderWeaponOff");
	if (ImGui::InputFloat("ColliderWeaponOff", &m_iColliderWeaponOffTrackPosition, 0.01f, 1.f));
}

void CWindow_AnimTool::BonePoint_Update()
{
	if (m_pBoneCollider.size() > 0)
	{
		for (auto& pCollider : m_pBoneCollider)
		{
			m_pGameInstance->Add_DebugRender(pCollider);
		}
	}

	if (m_pWeaponCollider.size() > 0)
	{
		for (auto& pColliderWeapon : m_pWeaponCollider)
		{
			m_pGameInstance->Add_DebugRender(pColliderWeapon);
		}
	}
}

void CWindow_AnimTool::Create_Bounding(_float fRadius)
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC pBoundingSphere;

	pBoundingSphere.vCenter = _float3(0.0f,0.0f,0.0f);
	pBoundingSphere.fRadius = fRadius;
	
	m_PickingObject->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"),TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &pBoundingSphere);
	
	//m_pCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), &pBoundingSphere));
	
	m_pBoneCollider.push_back(m_pCollider);
	++m_iCreateColliderNum;
}

void CWindow_AnimTool::Create_Weapon_Bounding(_float3 fPoint, _float fRadius)
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC pBoundingSphere;

	pBoundingSphere.vCenter = fPoint;
	pBoundingSphere.fRadius = fRadius;

	m_PickingWeapon->Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pWCollider), &pBoundingSphere);

	//m_pWCollider = dynamic_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Collider_Sphere"), &pBoundingSphere));

	m_pWeaponCollider.push_back(m_pWCollider);
	++m_iCreateWeaponColliderNum;
}



char* CWindow_AnimTool::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* result = new char[size_needed];

	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);


	return result;
}

char* CWindow_AnimTool::ConvertWCtoC(const wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;
	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];
	//형 변환
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);


	return pStr;
}

wchar_t* CWindow_AnimTool::ConvertCtoWC(const char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);


	return pStr;
}

CWindow_AnimTool* CWindow_AnimTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow_AnimTool* pInstance = new CWindow_AnimTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow_AnimTool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_AnimTool::Free()
{
	__super::Free();
	Safe_Delete(m_pSphere);
}
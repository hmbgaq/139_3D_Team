#include "..\Public\UI.h"
#include "GameInstance.h"
#include "SMath.h"
#include "Data_Manager.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CGameObject(pDevice, pContext, strPrototypeTag)
	, m_pData_Manager(CData_Manager::GetInstance())
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
	, m_pData_Manager(rhs.m_pData_Manager)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	/* Info */
	if (pArg != nullptr)
		m_tUIInfo = *(UI_DESC*)pArg;

#pragma region Transform
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, 1.f, 1.f);

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (nullptr != Find_Component(g_pTransformTag))
		return E_FAIL;

	m_Components.emplace(g_pTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);
#pragma endregion End

#pragma region 2D
	m_pTransformCom->Set_Scaling(m_tUIInfo.fScaleX, m_tUIInfo.fScaleY, m_fScaleZ);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tUIInfo.fPositionX - (_float)g_iWinSizeX * 0.5f, -m_tUIInfo.fPositionY + (_float)g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	m_fPositionX = m_tUIInfo.fPositionX;
	m_fPositionY = m_tUIInfo.fPositionY;
	m_fScaleX = m_tUIInfo.fScaleX;
	m_fScaleY = m_tUIInfo.fScaleY;

	SetUp_UIRect(m_fPositionX, m_fPositionY, m_fScaleX, m_fScaleY);

	m_Origin_WorldMatrix = m_pTransformCom->Get_WorldMatrix();
#pragma endregion End

	//FAILED_CHECK(Ready_UI("../Bin/DataFiles/Data_UI/UI_Info"));

	return S_OK;
}

void CUI::Priority_Tick(_float fTimeDelta)
{

}

void CUI::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::UISTATE::READY:
		break;
	case Client::UISTATE::APPEAR:
		UI_AppearTick(fTimeDelta);
		break;
	case Client::UISTATE::TICK:
		break;
	case Client::UISTATE::DISAPPEAR:
		UI_DisappearTick(fTimeDelta);
		break;
	case Client::UISTATE::LEVEL_UP:
		//Tick_LevelUp(fTimeDelta);
		break;
	case Client::UISTATE::STATE_END:
		break;
	default:
		break;
	}

	Play_Animation(); // 애니메이션 재생 m_eState를 통해 애니메이션 UI 타입일때만 탈 수 있게 해줘도 될듯하다.
	Update_Child_Transform();
	Check_RectPos();
	Picking_UI();
}

void CUI::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI::UI_DisappearTick(_float fTimeDelta)
{
	m_eState = UISTATE::READY;
}

void CUI::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::Picking_UI()
{

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);  // 클라이언트 내에 마우스 포인터 가져오기 

	_uint ViewPortIndex = 1;
	CD3D11_VIEWPORT ViewPort;

	m_pContext->RSGetViewports(&ViewPortIndex, &ViewPort); // 뷰포트 가져오기 
	
	if (PtInRect(&m_rcUI, pt))   //  PtInRect(렉트주소, 마우스 포인트)
		m_bPick = true;
	else
		m_bPick = false;
}

HRESULT CUI::Ready_Components()
{

	return S_OK;
}

HRESULT CUI::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CUI::SetUp_UIRect(_float fPosX, _float fPosY, _float fSizeX, _float fSizeY)
{
	/* 렉트 크기를 표시할만한 디버깅 요소로 뭐가 좋을까 (Collider, Texture, Color..) */
	m_rcUI.left = static_cast<LONG>(fPosX - (fSizeX * 0.5f));
	m_rcUI.top = static_cast<LONG>(fPosY - (fSizeY * 0.5f));
	m_rcUI.right = static_cast<LONG>(fPosX + (fSizeX * 0.5f));
	m_rcUI.bottom = static_cast<LONG>(fPosY + (fSizeY * 0.5f));

	return S_OK;
}

void CUI::Change_SizeX(_float MMX)
{
	m_fScaleX += MMX;
	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Change_SizeY(_float MMY)
{
	m_fScaleY += MMY;
	m_fPositionY = m_fPositionY;
	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Change_SizeRight(_float MMX)
{
	m_fScaleX += MMX;
	m_fPositionX += MMX * 0.5f;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Change_SizeLeft(_float MMX)
{
	m_fScaleX += -1.f * MMX;
	m_fPositionX += MMX * 0.5f;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Change_SizeTop(_float MMY)
{
	m_fScaleY += MMY;
	m_fPositionY -= MMY * 0.5f;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Change_SizeBottom(_float MMY)
{
	m_fScaleY += MMY;
	m_fPositionY += MMY * 0.5f;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

void CUI::Set_Size(_float fSizeX, _float fSizeY)
{
	m_fScaleX = fSizeX;
	m_fScaleY = fSizeY;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, m_fScaleZ);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ));
}

HRESULT CUI::Set_ParentTransform(CTransform* pParentTransformCom)
{
	m_tUIInfo.pParentTransformCom = pParentTransformCom;

	return S_OK;
}

void CUI::Add_Create_Parts(void* pArg)
{
	if (pArg == nullptr)
		return;

	CUI::UI_DESC* pUIDesc = (CUI::UI_DESC*)pArg;

	wstring wstrLayerTag = TEXT("");
	m_pGameInstance->String_To_WString(pUIDesc->strLayerTag, wstrLayerTag);
	wstring wstrPartsTag = TEXT("");
	m_pGameInstance->String_To_WString(pUIDesc->strCloneTag, wstrPartsTag);
	CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, wstrLayerTag, wstrPartsTag, &pArg));
	m_vecUIParts.push_back(pUI);
}

void CUI::Add_Parts(CUI* pUI)
{
	if (pUI == nullptr)
		return;
	//pUI->Set_ParentTransform(m_pTransformCom);

	m_Origin_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	m_vecUIParts.push_back(pUI);
}

CUI* CUI::Get_UIPart(const wstring& strPartTag)
{
	for (auto& iter : m_vecUIParts)
	{
		//if (strPartTag == iter->Get_UITag())
		//	return	 iter;
	}

	return nullptr;
}

vector<CUI*> CUI::Get_UIParts()
{
	return m_vecUIParts;
}

void CUI::Set_PosZ(_float fZ)
{
	_float Z = fZ;
	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, Z));
}

void CUI::Set_Pos(_float fPosX, _float fPosY)
{
	m_fPositionX = fPosX;
	m_fPositionY = fPosY;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, 0.2f));
}

void CUI::Check_RectPos()
{
	m_fPositionX = m_pTransformCom->Get_Position().x + g_iWinSizeX * 0.5f;
	m_fPositionY = -m_pTransformCom->Get_Position().y + g_iWinSizeY * 0.5f;

	m_fScaleX = m_pTransformCom->Get_Scaled().x;
	m_fScaleY = m_pTransformCom->Get_Scaled().y;

	m_rcUI.left = LONG(m_fPositionX - (m_fScaleX / 2));
	m_rcUI.top = LONG(m_fPositionY - (m_fScaleY / 2));
	m_rcUI.right = LONG(m_fPositionX + (m_fScaleX / 2));
	m_rcUI.bottom = LONG(m_fPositionY + (m_fScaleY / 2));
}

void CUI::Moving_Picking_Point(POINT pt)
{
	m_fPositionX = (_float)pt.x;
	m_fPositionY = (_float)pt.y;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_fPositionX - g_iWinSizeX * 0.5f, -m_fPositionY + g_iWinSizeY * 0.5f, 0.2f));
}

void CUI::Parts_Delete()
{
	if (m_vecUIParts.empty())
		return;

	_int iSize = m_vecUIParts.size();

	for (_int i = 0; i < iSize; ++i)
	{
		if (m_vecUIParts[i]->m_bDead == true)
		{
			m_vecUIParts.erase(m_vecUIParts.begin() + i);
		}
	}
}

HRESULT CUI::SetUp_BillBoarding()
{
	_matrix CamWorldMatrix;

	CamWorldMatrix = m_pGameInstance->Get_TransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	_float3 vScale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, CamWorldMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, CamWorldMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, CamWorldMatrix.r[2]);
	m_pTransformCom->Set_Scaling(vScale.x, vScale.y, vScale.z);

	return S_OK;
}

HRESULT CUI::SetUp_Transform(_float fPosX, _float fPosY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_fPositionX = fPosX;
	m_fPositionY = fPosY;
	m_fScaleX = fSizeX;
	m_fScaleY = fSizeY;

	m_pTransformCom->Set_Scaling(m_fScaleX, m_fScaleY, 1.f);

	// 위치 이동
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fPositionX - (_float)g_iWinSizeX * 0.5f, -m_fPositionY + (_float)g_iWinSizeY * 0.5f, m_tUIInfo.fPositionZ, 1.f));

	// View Matrix 및 Projection Matrix 설정
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

HRESULT CUI::Ready_UI(const char* cFilePath) // 컨테이너에 담을 UI 파츠들 불러오기
{
	json json_in;

	_int		iPathNum = 0;


	CJson_Utility::Load_Json(cFilePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		tUI_Info.strLayerTag = object["LayerTag"];
		tUI_Info.strCloneTag = object["CloneTag"];
		tUI_Info.strProtoTag = object["ProtoTag"];
		tUI_Info.strFilePath = object["FilePath"];

		tUI_Info.strMapTextureTag = object["FilePath"];

		tUI_Info.iShaderNum = object["ShaderNum"];

		wstring wstrLayertag;
		m_pGameInstance->String_To_WString(tUI_Info.strLayerTag, wstrLayertag);

		wstring wstrClonetag;
		m_pGameInstance->String_To_WString(tUI_Info.strCloneTag, wstrClonetag);

		wstring wstrPrototag;
		m_pGameInstance->String_To_WString(tUI_Info.strProtoTag, wstrPrototag);

		wstring wstrFilePath;
		m_pGameInstance->String_To_WString(tUI_Info.strFilePath, wstrFilePath);

		CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, wstrLayertag, wstrClonetag, &tUI_Info));

		pUI->Get_Transform()->Load_FromJson(object);

	}

	return S_OK;
}

HRESULT CUI::Create_UIParts(UI_DESC tUI_Desc)
{
	wstring wstrPrototag;
	m_pGameInstance->String_To_WString(tUI_Desc.strProtoTag, wstrPrototag);

	wstring wstrFilePath;
	m_pGameInstance->String_To_WString(tUI_Desc.strFilePath, wstrFilePath);

	CUI* pUI_Object = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), wstrPrototag, &tUI_Desc));
	m_vecUIParts.push_back(pUI_Object);

	return S_OK;
}

HRESULT CUI::Update_Child_Transform()
{
	if (m_vecUIParts.empty())
		return S_OK;

	if (m_tUIInfo.bParent)
	{
		if (m_Origin_WorldMatrix != m_pTransformCom->Get_WorldFloat4x4())
		{
			_vector Origin_scale;
			_vector Curr_scale;
			_float3 Origin_rotation;
			_float3 Curr_rotation;
			_vector Origin_translation;
			_vector Curr_translation;

			/* 회전 */
			Origin_rotation = SMath::Extract_PitchYawRollFromRotationMatrix(m_Origin_WorldMatrix);
			Curr_rotation = SMath::Extract_PitchYawRollFromRotationMatrix(m_pTransformCom->Get_WorldFloat4x4());

			_float RotX = Curr_rotation.x - Origin_rotation.x;
			_float RotY = Curr_rotation.y - Origin_rotation.y;
			_float RotZ = Curr_rotation.z - Origin_rotation.z;

			/* 크기 */
			Origin_scale = SMath::Get_Scale(m_Origin_WorldMatrix);
			Curr_scale = SMath::Get_Scale(m_pTransformCom->Get_WorldFloat4x4());

			_vector vOrigin_rotation;
			_vector vCurr_rotation;
			
			XMMATRIX RotMat;
			RotMat = XMMatrixRotationRollPitchYaw(RotX, RotY, RotZ);

			XMMatrixDecompose(&Origin_scale, &vOrigin_rotation, &Origin_translation, m_Origin_WorldMatrix);
			XMMatrixDecompose(&Curr_scale, &vCurr_rotation, &Curr_translation, m_pTransformCom->Get_WorldFloat4x4());

			_float3 Result_Scale = Curr_scale / Origin_scale;
			_float3 Result_Rotation = vCurr_rotation - vOrigin_rotation;
			_float3 Result_Position = Curr_translation - Origin_translation;


			for (auto& iter : m_vecUIParts)
			{
				_vector Child_Scale = iter->Get_Transform()->Get_Scaled();
				_vector Child_Rot = iter->Get_Transform()->Get_Rotated();
				_vector Child_Pos = iter->Get_Transform()->Get_State(CTransform::STATE_POSITION);

				_float3 Finish_Scale;
				_float3 Finish_Rot;
				_float3 Finish_Pos;

				Finish_Scale.x = Result_Scale.x * Child_Scale.m128_f32[0];
				Finish_Scale.y = Result_Scale.y * Child_Scale.m128_f32[1];
				Finish_Scale.z = Result_Scale.z * Child_Scale.m128_f32[2];

				Finish_Rot.x = Result_Rotation.x + Child_Rot.m128_f32[0];
				Finish_Rot.y = Result_Rotation.y + Child_Rot.m128_f32[1];
				Finish_Rot.z = Result_Rotation.z + Child_Rot.m128_f32[2];

				Finish_Pos.x = Result_Position.x + Child_Pos.m128_f32[0];
				Finish_Pos.y = Result_Position.y + Child_Pos.m128_f32[1];
				Finish_Pos.z = Result_Position.z + Child_Pos.m128_f32[2];

				iter->Get_Transform()->Set_Scaling(Finish_Scale.x, Finish_Scale.y, Finish_Scale.z); // Scale
				iter->Get_Transform()->Set_State(CTransform::STATE_POSITION, Finish_Pos);			// Pos
				iter->Set_WorldMatrix(iter->Get_Transform()->Get_WorldMatrix() * RotMat);			// Rot
			}
			m_Origin_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		}
	}

	return S_OK;
}

void CUI::Tick_LevelUp(_float fTimeDelta)
{
	LifeTime_LevelUp(fTimeDelta);
}

/* @@@보류@@@ */
void CUI::Load_UIData(const char* _FilePath)
{
	_int		iPathNum = 0;
	string		strFileName;
	string		strFilePath;

	json json_in;

	CJson_Utility::Load_Json(_FilePath, json_in);

	for (auto& item : json_in.items())
	{
		json object = item.value();

		CUI::UI_DESC tUI_Info;

		//tUI_Info.strProtoTag = object["ProtoTag"];
		tUI_Info.strFilePath = object["FilePath"];
	}
}

json CUI::Save_Desc(json& out_json)
{
	out_json["Parent"] = m_tUIInfo.bParent;

	out_json["World"] = m_tUIInfo.bWorld;
	
	out_json["Group"] = m_tUIInfo.bGroup;

	//out_json["ScaleX"] = m_tUIInfo.fScaleX;

	//out_json["ScaleY"] = m_tUIInfo.fScaleY;

	//out_json["PositionX"] = m_tUIInfo.fPositionX;

	//out_json["PositionY"] = m_tUIInfo.fPositionY;

	//out_json["PositionZ"] = m_tUIInfo.fPositionZ;

	out_json["Alpha"] = m_tUIInfo.fAlpha;

	out_json["ObjectNum"] = m_tUIInfo.iObjectNum;

	out_json["ShaderNum"] = m_tUIInfo.iShaderNum;

	out_json["ObjectName"] = m_tUIInfo.strObjectName;

	out_json["LayerTag"] = m_tUIInfo.strLayerTag;

	out_json["CloneTag"] = m_tUIInfo.strCloneTag;

	out_json["ProtoTag"] = m_tUIInfo.strProtoTag;

	out_json["FilePath"] = m_tUIInfo.strFilePath;

	out_json["MapTextureTag"] = m_tUIInfo.strMapTextureTag;
	
	out_json["ColorR"] = m_tUIInfo.vColor.m128_f32[0];
	out_json["ColorG"] = m_tUIInfo.vColor.m128_f32[1];
	out_json["ColorB"] = m_tUIInfo.vColor.m128_f32[2];
	out_json["ColorA"] = m_tUIInfo.vColor.m128_f32[3];

	/* TransformCom */
	m_pTransformCom->Write_Json(out_json);

	///* Group Save */
	//if (!m_vecUIParts.empty())
	//{
	//	_int iSize = m_vecUIParts.size();

	//	for (_int i = 0; i < iSize; ++i)
	//	{

	//	}
	//}

	return out_json;
}

void CUI::Play_Animation()
{
	if (m_vecAnimation[m_eKeyframe] == nullptr)
		return;

	// 비었는지 검사
	if (!m_vecAnimation[m_eKeyframe]->empty())
	{
		// 프레임 재생 여부
		if (m_bPlayAnim)
		{
			// 현재 프레임을 시간(프레임)마다 증가시키기


			// 현재 프레임이 최대 프레임에 도달한 경우
			if (m_fCurrTime > (*m_vecAnimation[m_eKeyframe])[m_vecAnimation[m_eKeyframe]->size() - 1].fTime)
			{
				// 현재 프레임 초기화
				//m_pAnimationTool->Get_currentTime() = 0.f;

				// 반복 On/Off
				if (m_bRepetition)
				{
					m_bPlayAnim = false;
				}

				// 시간 초기화
				m_fCurrTime = 0.f;
			}
		}
	}

	if (!m_vecAnimation[m_eKeyframe]->empty()) // 비었는지 체크
	{
		//if (m_pAnimationTool->Get_FramePlaying()) // 재생 버튼을 눌렀을 경우만
		//{
		if (m_fCurrTime >= 0.f &&
			m_fCurrTime <= m_vecAnimation[m_eKeyframe]->back().fTime)
		{
			//m_eAnimationInfo = m_vecAnimation[(int)m_iFrameCount].front();
			_uint iFrameIndex = 0U;
			for (_uint i = m_vecAnimation[m_eKeyframe]->size() - (_uint)1; i >= 0; i--)
			{
				if ((*m_vecAnimation[m_eKeyframe])[i].fTime <= m_fCurrTime)
				{
					iFrameIndex = i;
					break;
				}
			}

			// Constant
			//m_fSizeX = (*m_vecAnimation[*m_eKeyframe])[iFrameIndex].vScale.x;
			//m_fSizeY = (*m_vecAnimation[*m_eKeyframe])[iFrameIndex].vScale.y;

			//m_fX = m_fSizeX * 0.5f; // 중점위치 
			//m_fY = m_fSizeY * 0.5f;

			//m_pTransformComp->Set_Pos({ (*m_vecAnimation[*m_eKeyframe])[iFrameIndex].vPos.x,
			//							(*m_vecAnimation[*m_eKeyframe])[iFrameIndex].vPos.y,
			//							0.f });	// 이미지 위치

			//m_pTransformComp->Set_Scale({ m_fSizeX, m_fSizeY, 1.f });	// 이미지 크기


			// Linear
			if (iFrameIndex + 1U < m_vecAnimation[m_eKeyframe]->size())
			{
				// 키 프레임간 시간 변화율
				fFrameTimeDelta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].fTime - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].fTime;
				// 현재 키 프레임시간부터 현재 시간 변화율
				fCurFrameTimeDelta = (m_fCurrTime - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].fTime);

				fSizeX_Delta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].vScale.x - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.x;
				fSizeX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
				fSizeY_Delta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].vScale.y - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.y;
				fSizeY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

				fRotZ_Delta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].fRot - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].fRot;
				fRotZ_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

				fPosX_Delta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].vPos.x - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.x;
				fPosX_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;
				fPosY_Delta = (*m_vecAnimation[m_eKeyframe])[iFrameIndex + 1U].vPos.y - (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.y;
				fPosY_Delta *= fCurFrameTimeDelta / fFrameTimeDelta;

				m_pTransformCom->Set_Position({ (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.x + fPosX_Delta,
											(*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.y + fPosY_Delta,
											0.f });	// 이미지 위치

				m_pTransformCom->Set_Scaling((*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.x + fSizeX_Delta, 	// 이미지 크기
											  (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.y + fSizeY_Delta,
											  1.f);

				m_pTransformCom->Rotation({ 0.0f, 0.0f, 1.0f, 0.0f }, (*m_vecAnimation[m_eKeyframe])[iFrameIndex].fRot + fRotZ_Delta);// 이미지 회전

				// 현재 키타입
				switch (m_eKeyframe)
				{
					case KEYTYPE::KEYTYPE_NORMAL:
					{
						m_iTextureNum[KEYTYPE_NORMAL] = (*m_vecAnimation[m_eKeyframe])[iFrameIndex].iTexureframe;
						break;
					}
				}

			}
			else
			{
				m_pTransformCom->Set_Scaling((*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.x, 	// 이미지 크기
											 (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vScale.y,
											  1.f );

				m_pTransformCom->Set_Position({ (*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.x,
												(*m_vecAnimation[m_eKeyframe])[iFrameIndex].vPos.y,
												0.f });	// 이미지 위치

				// 현재 키타입
				switch (m_eKeyframe)
				{
					case KEYTYPE::KEYTYPE_NORMAL:
					{
						m_iTextureNum[KEYTYPE_NORMAL] = (*m_vecAnimation[m_eKeyframe])[iFrameIndex].iTexureframe;
						break;
					}
				}

			}

		}
		//}

	}
}

void CUI::LifeTime_LevelUp(_float fTimeDelta)
{
	/* 레벨 변동이 있을 경우 */
	if (m_pData_Manager->Get_ShowLevelBox()/*m_pData_Manager->Limit_EXP()*/)
	{
		m_pData_Manager->Set_ShowLevelBox(false);
		m_bActive = true;
		m_fAlpha = 0.f;
	}

	if (m_fTime + m_fLifeTime < GetTickCount64())
	{
		m_bEventOn = true;
		m_fTime = GetTickCount64();
	}

	if (m_bEventOn)
	{
		m_fAlpha += fTimeDelta;
	}

	if (m_fAlpha >= 1.f)
	{
		m_bActive = false;
		m_bEventOn = false;
	}
}

void CUI::Free()
{
	__super::Free();

	if (m_pData_Manager)
		Safe_Release(m_pData_Manager);
	if (m_pVIBufferCom)
		Safe_Release(m_pVIBufferCom);
	if (m_pShaderCom)
		Safe_Release(m_pShaderCom);
	//Safe_Release(m_pTextureCom);
	if (m_pMapTextureCom)
		Safe_Release(m_pMapTextureCom);
}

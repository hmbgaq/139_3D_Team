#include "stdafx.h"
#include "..\Public\Environment_Interact.h"

#include "GameInstance.h"
#include "Character.h"
#include "Player.h"
#include "Level_Loading.h"

CEnvironment_Interact::CEnvironment_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
	
}

CEnvironment_Interact::CEnvironment_Interact(const CEnvironment_Interact & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_Interact::Initialize_Prototype()
{	

	return S_OK;
}

HRESULT CEnvironment_Interact::Initialize(void* pArg)
{	
	m_tEnvironmentDesc = *(ENVIRONMENT_INTERACTOBJECT_DESC*)pArg;

	m_iCurrentLevelIndex = m_pGameInstance->Get_NextLevel();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (false == m_tEnvironmentDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);

	if(m_iCurrentLevelIndex != (_uint)LEVEL_TOOL)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());
		Safe_AddRef(m_pPlayer);
		m_bFindPlayer = true;
	}


	m_pTransformCom->Set_Speed(m_fSplineSpeed);
	m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(90.f));
	XMStoreFloat4x4(&m_InitMatrix, XMMatrixIdentity());

	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN && m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
	{
		Init_WagonEvent();
	}

	return S_OK;
}

void CEnvironment_Interact::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_Interact::Tick(_float fTimeDelta)
{
	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		if(m_pGameInstance->Key_Down(DIK_F7))
			Start_SplineEvent();
		if(m_pGameInstance->Key_Down(DIK_NUMPAD1))
			m_vecPointChecks[0] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD2))
			m_vecPointChecks[1] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
			m_vecPointChecks[2] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
			m_vecPointChecks[3] = true;
		if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
			Reset_TestEvent();
		

	}



	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL && m_bFindPlayer == false)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());

		if(m_pPlayer != nullptr)
		{
			Safe_AddRef(m_pPlayer);
			m_bFindPlayer = true;
		}
	}


	if (true == m_tEnvironmentDesc.bAnimModel && true == m_bPlay)
	{
		m_pModelCom->Play_Animation(fTimeDelta, true);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	Interact();

	if (m_bSpline == true)
	{
		Spline_Loop(fTimeDelta);
	}

}

void CEnvironment_Interact::Late_Tick(_float fTimeDelta)
{

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return ;

	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		m_pGameInstance->Add_DebugRender(m_pColliderCom);
	}
}

HRESULT CEnvironment_Interact::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i);

		m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	//#몬스터모델렌더
	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(-20.f, 100.f, -20.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / (float)g_iWinSizeY, 0.1f, m_pGameInstance->Get_CamFar()));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	//TODO 클라에서 모델의 메시 개수를 받아와서 순회하면서 셰이더 바인딩해주자.

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(2); //TODO 추후 ENUM 으로 변경

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_bool CEnvironment_Interact::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_Interact::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

#ifdef _DEBUG

void CEnvironment_Interact::Set_ColliderSize(_float3 vColliderSize)
{
	CBounding* pBounding = m_pColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);
	
	if(pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Extents = vColliderSize;
	m_tEnvironmentDesc.vColliderSize = vColliderSize;

	//m_pColliderCom->Set_Bounding()
}

void CEnvironment_Interact::Set_ColliderCenter(_float3 vColliderCenter)
{
	CBounding* pBounding = m_pColliderCom->Get_Bounding();

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

	if (pAABB == nullptr)
		return;

	BoundingBox* pBox = pAABB->Get_OriginBounding();

	pBox->Center = vColliderCenter;
	m_tEnvironmentDesc.vColliderCenter = vColliderCenter;
}

void CEnvironment_Interact::Set_LevelChangeType(_bool bLevelChange, LEVEL eLevel)
{
	m_tEnvironmentDesc.bLevelChange = bLevelChange;
	m_tEnvironmentDesc.eChangeLevel = eLevel;
}

_bool CEnvironment_Interact::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}


#endif

void CEnvironment_Interact::Interact()
{
	if(m_bFindPlayer == false)
		return;

		if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_LOOP)
		{
			if (true == m_pColliderCom->Is_Collision(m_pPlayer->Get_Collider()))
			{
				switch (m_tEnvironmentDesc.eInteractType)
				{
					case CEnvironment_Interact::INTERACT_JUMP100:
					{
						//CPlayer::Player_State::Player_InteractionJumpDown100;


						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown100();
						}
						
						break;
					}

					case CEnvironment_Interact::INTERACT_JUMP200:
					{
						
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown200();
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_JUMP300:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown300();
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_VAULT100:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractVault100();

						break;
					}

					case CEnvironment_Interact::INTERACT_VAULT200:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractVault200();
							
						break;
					}
				}

				if (m_tEnvironmentDesc.bUseGravity == false)
				{
					m_pPlayer->Set_UseGravity(false);
				}

				m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);

				if (true == m_tEnvironmentDesc.bLevelChange)
				{
					if (true == m_pPlayer->Is_Animation_End() && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
					{
						m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));

						m_bInteract = true;
					}
				}
			}
		}
		else if (m_tEnvironmentDesc.eInteractState == CEnvironment_Interact::INTERACTSTATE_ONCE && m_bInteract == false)
		{
			if (true == m_pColliderCom->Is_Collision(m_pPlayer->Get_Collider()))
			{
				switch (m_tEnvironmentDesc.eInteractType)
				{
					case CEnvironment_Interact::INTERACT_JUMP100:
					{

						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown100();
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_JUMP200:
					{

						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown200();
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_JUMP300:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractJumpDown300();
						}
							

						break;
					}

					case CEnvironment_Interact::INTERACT_VAULT100:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractVault100();
						}

						break;
					}
						


					case CEnvironment_Interact::INTERACT_VAULT200:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
						{
							m_pPlayer->SetState_InteractVault200();
							//m_pPlayer->Set_RootMoveRate(_float3(1.f, 0.5f, 1.f));	// 이런 식으로 루트 모션 비율 조정하면 됨
							
						}

						break;
					}


				}

				if (m_tEnvironmentDesc.bUseGravity == false)
				{
					m_pPlayer->Set_UseGravity(false);
				}

				m_pPlayer->Set_RootMoveRate(m_tEnvironmentDesc.vPlayerRootMoveRate);


				
			
				m_bInteract = true;
			

			}

			
		}	

		if (true == m_tEnvironmentDesc.bLevelChange && m_bInteract == true)
		{
			if (m_pPlayer->Is_Inputable_Front(32) && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
			{
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
				m_bInteract = false;
			}



			//if (true == m_pPlayer->Is_Animation_End() && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
			//{
			//	//m_pPlayer->Is_Inputable_Front()
			//	//m_pPlayer->Get_Body()->Get_Model()->Get_TrackPosition()
			//	//m_pPlayer->Get_Actor()->Free()
			//	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
			//	m_bInteract = false;
			//}
			
		}
}

void CEnvironment_Interact::Start_SplineEvent()
{
	m_bSpline = true;
}

void CEnvironment_Interact::Reset_TestEvent()
{
	m_vecPointChecks[0] = false;
	m_vecPointChecks[1] = false;
	m_vecPointChecks[2] = false;
	m_vecPointChecks[3] = false;

	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	
	m_vecSplinePoints.clear();
	m_iCurrentSplineIndex = 0;

	m_strCurrentSplineTrack = L"분기1지점오답";

	auto& StartIter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

	m_vecSplinePoints = StartIter->second;

	m_iDivergingCount = 4;

	Set_SplineSpeed(10.f);

	for (_int i = 0; i < m_iDivergingCount; ++i)
	{
		m_vecPointChecks.push_back(false);
	}
}

void CEnvironment_Interact::Start_Spline(vector<_float4>* SplinePoints)
{
	if (true == SplinePoints->empty() || m_bSpline == true)
		return;

	if (SplinePoints->size() % 2 != 0)
	{
		MSG_BOX("스플라인 보간은 짝수여야됨~");
		return;
	}

	
	m_InitMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_vector vFirstPos, vSecondPos, vResultPos;
	_int iPointSize = SplinePoints->size();
	_int iRoopCount = SplinePoints->size() / 2;


	for (_int i = 0; i < iPointSize; ++i)
	{
		

		m_vecSplinePoints.push_back((*SplinePoints)[i]);
	}

	// 포인트가 6개면 루프카운트는 3
	

	//for (_int i = 0; i < iRoopCount; i++) 
	//{
	//	//! 루프카운트는 3. i는 0 
	//	
	//	vFirstPos = XMLoadFloat4(&SplinePoints[i * 2]); //! 0 2 4
	//	vSecondPos = XMLoadFloat4(&SplinePoints[i * 2 + 1]); //! 1 3 5
	//	vResultPos = XMVectorCatmullRom(vFirstPos, vFirstPos, vSecondPos, vSecondPos, m_fSplineTimeAcc);
	//	m_vecSplinePoints.push_back(vResultPos);
	//}

	m_bSpline = true;



	//TODO XMVectorCatmullRom()
	//!XMVECTOR Result;
	//!
	//!float t2 = t * t;
	//!float t3 = t2 * t;
	//!
	//!float P0 = -t3 + 2.0f * t2 - t;
	//!float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	//!float P2 = -3.0f * t3 + 4.0f * t2 + t;
	//!float P3 = t3 - t2;
	//!
	//!Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	//!Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;
	//!Result.z = (P0 * Position0.z + P1 * Position1.z + P2 * Position2.z + P3 * Position3.z) * 0.5f;
	//!Result.w = (P0 * Position0.w + P1 * Position1.w + P2 * Position2.w + P3 * Position3.w) * 0.5f;
	//!
	//!return Result;
	
	//TODO XMVectorCatmullRomV() 
	
	//!이 함수는 T 에서 독립적인 가중치 인자가 제공될 수 있다는 점을 제외하면 XMVectorCatmullRom 과 동일합니다 . 
	//!예를 들어, 한 2D 위치 세트에 대한 위치 벡터의 x 및 y 구성 요소와 다른 2D 위치 세트에 대한 위치 벡터의 z 및 w 구성 요소를 사용하여 두 세트의 Catmull-Rom 보간법을 계산할 수 있습니다. 
	//!2D 위치. T 의 x 및 y 구성 요소는 첫 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다. 
	//!마찬가지로 T 의 z 및 w 구성 요소는 두 번째 Catmull-Rom 보간에 대한 보간 요소를 결정합니다.
	
	
}

void CEnvironment_Interact::Reset_StartMatrix()
{
	m_pTransformCom->Set_WorldMatrix(m_InitMatrix); 
	m_iCurrentSplineIndex = 0; 
	m_fSplineTimeAcc = 0.f; 
	m_vecSplinePoints.clear();
	m_bSpline = false;
}

void CEnvironment_Interact::Spline_Loop(const _float fTimeDelta)
{
	
	m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed; 

	_float t = m_fSplineTimeAcc / m_fArrivalTime;

	t = max(0.0f, min(1.0f, t));


	//m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;//m_pTransformCom->Get_Speed();

	
	//
	//	if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
	//	{
	//		m_bSpline = false;
	//		return;
	//	}
	//	else
	//		++m_iCurrentSplineIndex;
	//}


	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
		{
			m_bSpline = false;
			return;
		}

		_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		_vector vMovePoint1, vMovePoint2, vCalcPosition;
		vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
		vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
		//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
		vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, t);

		if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
		{
			m_iCurrentSplineIndex++;
			//m_fSplineTimeAcc = 0.f;
		}
	}
	else
	{
		Change_WagonTrack(fTimeDelta);


		
	}

}

void CEnvironment_Interact::Spline_Clear()
{
	m_bSpline = false;
	m_vecSplinePoints.clear();
	m_fSplineTimeAcc = 0.f;
	m_fSplineEndRaidus = 1.f;
	m_iCurrentSplineIndex = 0;
}

HRESULT CEnvironment_Interact::Load_SplineJson()
{

	json SplineJson = {};
//C:\Users\PC\Desktop\3D_TeamPortpolio\Client\Bin\DataFiles\Data_Map\Spline
	string strHardPath = "../Bin/DataFiles/Data_Map/Spline/SplineData_202403252212.json";

	
	if (FAILED(CJson_Utility::Load_Json(strHardPath.c_str()/*m_tEnvironmentDesc.strSplineJsonPath.c_str()*/, SplineJson)))
	{
		MSG_BOX("스플라인 데이터 불러오기 실패");
		return E_FAIL;
	}

	_int iSplineJsonSize = SplineJson.size();

	for (_int i = 0; i < iSplineJsonSize; ++i)
	{
		string strSplineKey = SplineJson[i]["SplineKey"];//(string)SplineJson[i]["SplineKey"];
		wstring strTest;

		m_pGameInstance->String_To_WString(strSplineKey, strTest);


		//m_pGameInstance->WString_To_String(strTest, strSplineKey);

		

		//strSplineKey = strSplineKey.c_str();
		
		json   SplineVectorJson = SplineJson[i]["SplineVectorJson"];
		_float fSplineSpeed = SplineJson[i]["SplineSpeed"]; //! 해당 구간 속도

		_int iSplineVectorJsonSize = SplineVectorJson.size();

		vector<_float4> vecSplinePoint;

		for (_int j = 0; j < iSplineVectorJsonSize; ++j)
		{
			_float4 vSplinePoint = {};
			CJson_Utility::Load_Float4(SplineVectorJson[j]["SplinePoint"], vSplinePoint);
			vecSplinePoint.push_back(vSplinePoint);
		}

		m_mapSplineSpeeds.emplace(strTest, fSplineSpeed);
		m_mapSplineVectors.emplace(strTest,vecSplinePoint);
		
		
	}
	
	
//	m_iDivergingCount = SplineJson[0][""]




	//json

	//todo vector<_float4>						m_vecSplinePoints;
	//todo map<string, vector<_float4>>				m_mapSplineVectors;
	//todo map<string, vector<_bool>>				m_mapSplinePointChecks;
	//todo 
	//todo vector<_bool>							m_vecPointChecks;
	//todo _float								m_fSplineTimeAcc = 0.f;
	//todo _float								m_fSplineEndRaidus = 1.f;
	//todo _int								m_iCurrentSplineIndex = 0;
	//todo _float								m_fSplineSpeed = 1.f;
	//todo _float								m_fArrivalTime = 3.f; // ! 스플라인에 마지막 점까지 도착하는데 걸리는 시간.

	//m_tEnvironmentDesc.strSplineJsonPath;
	//
	//json SplineJson;
	//
	//if (FAILED(CJson_Utility::Load_Json(m_tEnvironmentDesc.strSplineJsonPath.c_str(), SplineJson)))
	//{
	//	MSG_BOX("스플라인 불러오기 실패");
	//	return E_FAIL;
	//}
	//
	//
	//_int iSplineJsonSize = SplineJson.size();
	//
	//for (_int i = 0; i < iSplineJsonSize; ++i)
	//{
	//	string strSplineKey = SplineJson[i]["SplineKey"];
	//
	//	
	//
	//
	//	_float4 vSplinePoint;
	//
	//	CJson_Utility::Load_Float4(SplineJson[i]["SplinePoint"], vSplinePoint);
	//
	//
	//	
	//
	//	//!Desc.iShaderPassIndex = BasicJson[i]["ShaderPassIndex"];
	//	//!Desc.iPlayAnimationIndex = BasicJson[i]["PlayAnimationIndex"];
	//}

	//m_pGameInstance->Load_FromJson(SplineJson);


	return S_OK;
}

HRESULT CEnvironment_Interact::Init_WagonEvent()
{
	Load_SplineJson();

	m_strCurrentSplineTrack = L"분기1지점오답";
	
	auto& StartIter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

	if (StartIter == m_mapSplineVectors.end())
		return E_FAIL;

	m_vecSplinePoints = StartIter->second;

	m_iDivergingCount = 4;

	Set_SplineSpeed(10.f);

	for (_int i = 0; i < m_iDivergingCount; ++i)
	{
		m_vecPointChecks.push_back(false);
	}

	return S_OK;
}

void CEnvironment_Interact::Change_WagonTrack(const _float fTimeDelta)
{
	_float t = m_fSplineTimeAcc / m_fArrivalTime;

	

	t = max(0.0f, min(1.0f, t));

	if (m_strCurrentSplineTrack == L"분기1지점오답" && m_vecPointChecks[0] == true)
	{
		_float4 vChangePoint = m_vecSplinePoints[m_iCurrentSplineIndex];

		m_vecSplinePoints.clear();

		m_vecSplinePoints.push_back(vChangePoint);
		
		m_strCurrentSplineTrack = L"분기1지점정답";

		auto& iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

		_int iNewSplineVectorSize = iter->second.size();

		for (_int i = 0; i < iNewSplineVectorSize; ++i)
		{
			m_vecSplinePoints.push_back(iter->second[i]);
		}
		
		auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

		Set_SplineSpeed(Speediter->second);
		
		m_iCurrentSplineIndex = 0;
		m_bSpline = true;
		
		
	}

	

	if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
	{
		if (m_strCurrentSplineTrack == L"분기1지점정답")
		{
			if (m_vecPointChecks[1] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점정답";
				
				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기2지점정답")
		{
			if (m_vecPointChecks[2] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기3지점정답")
		{
			if (m_vecPointChecks[3] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기1지점오답" || m_strCurrentSplineTrack == L"분기2지점오답" || m_strCurrentSplineTrack == L"분기3지점오답" || m_strCurrentSplineTrack == L"분기4지점오답" || m_strCurrentSplineTrack == L"분기4지점정답")
		{
			m_bSpline = false;
		}
		return;
	}

	_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	_vector vMovePoint1, vMovePoint2, vCalcPosition;
	vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
	vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
	//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
	vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, t);

	if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
	{
		m_iCurrentSplineIndex++;
		//m_fSplineTimeAcc = 0.f;
	}

	_float3 vOffsetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vOffsetPosition.y += 1.5f;

	m_pPlayer->Set_Position(vOffsetPosition);
	


	//_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//
	//
	//_vector vMovePoint1, vMovePoint2, vCalcPosition;
	//vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
	//vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
	////vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
	//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, t);

	//if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
	//{
	//	m_iCurrentSplineIndex++;
	//	//m_fSplineTimeAcc = 0.f;
	//}


}




HRESULT CEnvironment_Interact::Ready_Components()
{
	
	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, m_tEnvironmentDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if(FAILED(Ready_InteractCollider(m_tEnvironmentDesc.eInteractType)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEnvironment_Interact::Ready_InteractCollider(INTERACT_TYPE eInteractType)
{
	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::INTERACT);
	BoundingDesc.vExtents = m_tEnvironmentDesc.vColliderSize;
	BoundingDesc.vCenter = m_tEnvironmentDesc.vColliderCenter;

	switch (eInteractType)
	{
		case INTERACT_JUMP100:
		{
			BoundingDesc.iLayer   = ECast(COLLISION_LAYER::INTERACT);
			BoundingDesc.vExtents = m_tEnvironmentDesc.vColliderSize;
			BoundingDesc.vCenter  = m_tEnvironmentDesc.vColliderCenter;
			break;
		}

		case INTERACT_JUMP200:
		{
			BoundingDesc.iLayer = ECast(COLLISION_LAYER::INTERACT);
			BoundingDesc.vExtents = m_tEnvironmentDesc.vColliderSize;
			BoundingDesc.vCenter = m_tEnvironmentDesc.vColliderCenter;
			break;
		}

		case INTERACT_JUMP300:
		{
			BoundingDesc.iLayer = ECast(COLLISION_LAYER::INTERACT);
			BoundingDesc.vExtents = m_tEnvironmentDesc.vColliderSize;
			BoundingDesc.vCenter = m_tEnvironmentDesc.vColliderCenter;
			break;
		}
	}

	if (FAILED(__super::Add_Component(m_iCurrentLevelIndex, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment_Interact::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	
	return S_OK;
}

CEnvironment_Interact * CEnvironment_Interact::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CEnvironment_Interact*		pInstance = new CEnvironment_Interact(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_Interact");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnvironment_Interact::Clone(void* pArg)
{
	CEnvironment_Interact*		pInstance = new CEnvironment_Interact(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_Interact");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_Interact::Pool()
{
	return new CEnvironment_Interact(*this);
}

void CEnvironment_Interact::Free()
{
	__super::Free();
	
	if(m_pPlayer != nullptr)
		Safe_Release(m_pPlayer);

	Safe_Release(m_pModelCom);	
	Safe_Release(m_pShaderCom);
}




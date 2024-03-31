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
	m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(m_tEnvironmentDesc.fRotationSpeed));
	XMStoreFloat4x4(&m_InitMatrix, XMMatrixIdentity());

	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN && m_tEnvironmentDesc.eInteractType == CEnvironment_Interact::INTERACT_WAGONEVENT)
	{
		Init_WagonEvent();
	}

	if (m_iCurrentLevelIndex == (_uint)LEVEL_SNOWMOUNTAIN && m_tEnvironmentDesc.eInteractType == INTERACT_WAGONPUSH)
	{
		if (FAILED(Find_InteractGroupObject()))
			return E_FAIL;

		
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
		if (m_pGameInstance->Key_Down(DIK_F7))
		{
			Start_SplineEvent();
			m_pPlayer->SetState_InteractCartRideLoop();
		}
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


	if (true == m_tEnvironmentDesc.bAnimModel)// && true == m_bPlay)
	{
		m_pModelCom->Play_Animation(fTimeDelta, true);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if(m_bEnable == true)
		Interact();

	if (m_bSpline == true)
	{
		Spline_Loop(fTimeDelta);
		//Spline_LoopDoublePoint(fTimeDelta);
	}

	if (m_tEnvironmentDesc.bRotate == true)
	{
		if (RotationCheck(fTimeDelta))
		{
			StartGroupInteract();
			m_tEnvironmentDesc.bRotate = false;
		}
	}

	if (m_tEnvironmentDesc.bOffset == true)
	{
		Move_For_Offset();
	}

	if (m_tEnvironmentDesc.bRootTranslate == true)
	{
		Move_For_PlayerRootMotion();
	}

	if (m_tEnvironmentDesc.bOwner == false)
	{
		if (Check_OwnerEnablePosition())
			m_bEnable = false;
		else
			m_bEnable = true;
	}


}

void CEnvironment_Interact::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );

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
		if (m_tEnvironmentDesc.bAnimModel == true)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);
		}

		m_pModelCom->Bind_MaterialResource(m_pShaderCom, (_uint)i, &m_bORM_Available, &m_bEmissive_Available);
		m_pShaderCom->Bind_RawValue("g_bORM_Available", &m_bORM_Available, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_bEmissive_Available", &m_bEmissive_Available, sizeof(_bool));

		m_pShaderCom->Begin(m_tEnvironmentDesc.iShaderPassIndex);

		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_Interact::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_SHADOW));
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

void CEnvironment_Interact::Set_AnimationIndex(_uint iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

void CEnvironment_Interact::StartGroupInteract()
{
	_int iInteractGroupSize = (_int)m_vecInteractGroup.size();

	for (_uint i = 0; i < (_uint)iInteractGroupSize; ++i)
	{
		m_vecInteractGroup[i]->Set_Interact(false);
	}
}

void CEnvironment_Interact::Reset_Interact()
{
	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	m_bInteractStart = false;
	m_bInteract = false;
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
			if (true == m_pColliderCom->Is_Collision(m_pPlayer->Get_Collider()) )
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

					case CEnvironment_Interact::INTERACT_WAGONPUSH:
					{
						if (true == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()) && m_pPlayer->Get_CurrentAnimIndex() != (_int)CPlayer::Player_State::Player_InteractionPush_Rock_Idle)
						{
							if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
								m_pPlayer->SetState_InteractionPush_Rock_Idle();
						}
						else if(m_pPlayer->Get_CurrentAnimIndex() != (_int)CPlayer::Player_State::Player_InteractionPull_Rock_Idle)
						{
							if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
								m_pPlayer->SetState_InteractionPull_Rock_Idle();
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_WAGONJUMP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractCartRideStart();

						break;
					}

					case CEnvironment_Interact::INTEARCT_WAGONROPEJUMP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractCartRideWagonJump();

						break;
					}
					
					case CEnvironment_Interact::INTERACT_CLIMB100:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb100();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB200:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb200();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB300:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb300();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB450:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb450();

						break;
					}

					case CEnvironment_Interact::INTERACT_SLIDE:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractSlide();

						break;
					}

					case CEnvironment_Interact::INTERACT_LEVER:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractSmallLever();

						break;
					}

					case CEnvironment_Interact::INTERACT_PLANK:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractPlankStart();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROPECLIMB:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimbRope();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROPEDOWN:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractRopeDown();

						break;
					}

					case CEnvironment_Interact::INTERACT_DOOROPEN:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractDoorOpen();

						break;
					}

					case CEnvironment_Interact::INTERACT_LADDERUP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractLadderUpStart();

						break;
					}

					case CEnvironment_Interact::INTERACT_WHIPSWING:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractWhipSwing();

						break;
					}

					case CEnvironment_Interact::INTERACT_WHIPPULL:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractWhipPull();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROTATIONVALVE:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractRotationValve();

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
			else 
				m_bInteract = false;
			
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
							
						}

						break;
					}

					case CEnvironment_Interact::INTERACT_WAGONPUSH:
					{
						if (true == m_pTransformCom->Calc_FrontCheck(m_pPlayer->Get_Position()))
						{
							if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
								m_pPlayer->SetState_InteractionPush_Rock_Idle();
						}
						else
						{
							if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
								m_pPlayer->SetState_InteractionPull_Rock_Idle();
						}

					}

					case CEnvironment_Interact::INTERACT_WAGONJUMP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractCartRideStart();

						break;
					}

					case CEnvironment_Interact::INTEARCT_WAGONROPEJUMP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractCartRideWagonJump();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB100:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb100();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB200:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb200();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB300:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb300();

						break;
					}

					case CEnvironment_Interact::INTERACT_CLIMB450:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimb450();

						break;
					}

					case CEnvironment_Interact::INTERACT_SLIDE:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractSlide();

						break;
					}

					case CEnvironment_Interact::INTERACT_LEVER:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractSmallLever();

						break;
					}

					case CEnvironment_Interact::INTERACT_PLANK:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractPlankStart();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROPECLIMB:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractClimbRope();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROPEDOWN:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractRopeDown();

						break;
					}

					case CEnvironment_Interact::INTERACT_DOOROPEN:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractDoorOpen();

						break;
					}

					case CEnvironment_Interact::INTERACT_LADDERUP:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractLadderUpStart();

						break;
					}

					case CEnvironment_Interact::INTERACT_WHIPSWING:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractWhipSwing();

						break;
					}

					case CEnvironment_Interact::INTERACT_WHIPPULL:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractWhipPull();

						break;
					}

					case CEnvironment_Interact::INTERACT_ROTATIONVALVE:
					{
						if (m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Run_F || m_pPlayer->Get_CurrentAnimIndex() == (_int)CPlayer::Player_State::Player_Walk_F)
							m_pPlayer->SetState_InteractRotationValve();

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

			
			if (true == m_tEnvironmentDesc.bLevelChange && m_bInteract == true)
			{
				if (m_pPlayer->Is_Inputable_Front(32) && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
				{
					m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
					m_bInteract = false;
				}
			}
		}	
}

void CEnvironment_Interact::Move_For_PlayerRootMotion()
{
	if(m_pPlayer == nullptr && m_bInteract == false)
		return;

	_float4 vMyPostion = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 vPlayerRootMotion = m_pPlayer->Get_AddRootMotion();
	vPlayerRootMotion.w = 1.f;



	_vector vCalcPosition = XMLoadFloat4(&vMyPostion) + XMLoadFloat4(&vPlayerRootMotion);
	vCalcPosition.m128_f32[3] = 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);
}

void CEnvironment_Interact::Move_For_Offset()
{
	if(m_pOwnerInteract == nullptr)
		return;

	_float4 vOwnerPosition = m_pOwnerInteract->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCalcPosition = XMLoadFloat4(&vOwnerPosition) + XMLoadFloat4(&m_tEnvironmentDesc.vOffset);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);
}

HRESULT CEnvironment_Interact::Find_InteractGroupObject()
{
	

	list<CGameObject*> BackGroundObjects = *m_pGameInstance->Get_GameObjects(m_iCurrentLevelIndex, L"Layer_BackGround");

	if (true == BackGroundObjects.empty())
		return E_FAIL;

	for (auto& iter : BackGroundObjects)
	{
		if (typeid(*iter) == typeid(CEnvironment_Interact))
		{
			CEnvironment_Interact* pInteractObject = dynamic_cast<CEnvironment_Interact*>(iter);

			if (pInteractObject == nullptr)
				return E_FAIL;
			else
			{
				_int iFindObjectGroupIndex = pInteractObject->Get_InteractGroupIndex();

				if (iFindObjectGroupIndex != -1 && iFindObjectGroupIndex == m_tEnvironmentDesc.iInteractGroupIndex)
				{
					m_vecInteractGroup.push_back(pInteractObject);
					pInteractObject->Set_Interact(true); //! 처음에 충돌 되더라도 활성화시키지 않기위함
					pInteractObject->Set_OwnerObject(this); //! 자신이 활성화의 주체가 됨을 알림
				}
				else
					continue;


			}
		}
	}

	return S_OK;
}


_bool CEnvironment_Interact::ArrivalCheck()
{
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector		vDir = m_tEnvironmentDesc.vEnablePosition - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= 0.25f)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

_bool CEnvironment_Interact::RotationCheck(const _float fTimeDelta)
{
	return m_pTransformCom->Rotation_Lerp(XMConvertToRadians(m_tEnvironmentDesc.fRotationAngle), fTimeDelta, 0.1f);
}

_bool CEnvironment_Interact::Check_OwnerEnablePosition()
{
	if (m_pOwnerInteract == nullptr)
		return false;

	return m_pOwnerInteract->ArrivalCheck();
}

HRESULT CEnvironment_Interact::Add_InteractGroupObject(CEnvironment_Interact* pInteractObject)
{
	if (pInteractObject == nullptr)
		return E_FAIL;

	m_vecInteractGroup.push_back(pInteractObject);

	string strModelTag;

	m_pGameInstance->WString_To_String(pInteractObject->Get_ModelTag(), strModelTag);

	m_vecInteractGroupTag.push_back(strModelTag);

	return S_OK;
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

	Set_SplineSpeed(30.f);

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
	_int iPointSize = _int(SplinePoints->size());
	_int iRoopCount = _int(SplinePoints->size() / 2);

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

void CEnvironment_Interact::Start_SplineDouble(vector<_float4>* SplinePoints)
{
	if (true == SplinePoints->empty() || m_bSpline == true)
		return;

	if (SplinePoints->size() % 4 != 0)
	{
		MSG_BOX("스플라인 보간은 4의배수여야됨~");
		return;
	}


	m_InitMatrix = m_pTransformCom->Get_WorldFloat4x4();

	_vector vFirstPos, vSecondPos, vResultPos;
	_int iPointSize = (_int)SplinePoints->size();
	_int iRoopCount = _int(SplinePoints->size() / 4);


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
		_vector vCalcPosition = {};

		m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;

		if (m_fSplineTimeAcc >= 1.f)
		{
			m_fSplineTimeAcc = 0.0;
			++m_iCurrentSplineIndex;
			
		}

		_float t = m_fSplineTimeAcc / m_fArrivalTime;

		t = max(0.0f, min(1.0f, t));

		if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 1)
		{
			vCalcPosition = m_vecSplinePoints[m_iCurrentSplineIndex];
			m_bSpline = false;
		}
		else
		{
			_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


			_vector vMovePoint1, vMovePoint2;
			vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
			vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
			//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, 0.f);

			
			//vCalcPosition = XMVectorCatmullRomV(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, XMVectorSet(0.1f, 0.1f, 0.1f, 1.f));
			vCalcPosition = CatmullRomInterpolation(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
			//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, t);

			
			
		}

		m_pTransformCom->Look_At_Lerp(vCalcPosition, fTimeDelta, 0.1f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);

		//if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus))
		//{
		//	m_iCurrentSplineIndex++;
		//	//m_fSplineTimeAcc = 0.f;
		//}
	}
	else
	{
		Change_WagonTrack(fTimeDelta);
	}

}

void CEnvironment_Interact::Spline_LoopDoublePoint(const _float fTimeDelta)
{
	
	if (m_iCurrentLevelIndex == (_uint)LEVEL_TOOL)
	{
		m_fSplineTimeAcc += fTimeDelta * m_fSplineSpeed;

		_double t = m_fSplineTimeAcc / m_fArrivalTime;

		t = max(0.0f, min(1.0f, t));

		if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 3)
		{
			m_bSpline = false;
			return;
		}

		_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		_vector vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, vCalcPosition;
		vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
		vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
		vMovePoint3 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 2]);
		vMovePoint4 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 3]);

		//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fSplineTimeAcc);
		vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, (_float)t);
		


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
	string strHardPath = "../Bin/DataFiles/Data_Map/Spline/SplineData_202403280100.json";

	
	if (FAILED(CJson_Utility::Load_Json(strHardPath.c_str()/*m_tEnvironmentDesc.strSplineJsonPath.c_str()*/, SplineJson)))
	{
		MSG_BOX("스플라인 데이터 불러오기 실패");
		return E_FAIL;
	}

	_int iSplineJsonSize = _int(SplineJson.size());

	for (_int i = 0; i < iSplineJsonSize; ++i)
	{
		string strSplineKey = SplineJson[i]["SplineKey"];//(string)SplineJson[i]["SplineKey"];
		wstring strTest;

		m_pGameInstance->String_To_WString(strSplineKey, strTest);


		//m_pGameInstance->WString_To_String(strTest, strSplineKey);

		

		//strSplineKey = strSplineKey.c_str();
		
		json   SplineVectorJson = SplineJson[i]["SplineVectorJson"];
		_float fSplineSpeed = SplineJson[i]["SplineSpeed"]; //! 해당 구간 속도

		_int iSplineVectorJsonSize = _int(SplineVectorJson.size());

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

	m_fSplineTimeAcc += m_fSplineSpeed * fTimeDelta;

	

	_double t = m_fSplineTimeAcc / m_fArrivalTime;
	//!
	t = max(0.0f, min(1.0f, t));


	if (m_strCurrentSplineTrack == L"분기1지점오답" && m_vecPointChecks[0] == true)
	{
		_float4 vChangePoint = m_vecSplinePoints[m_iCurrentSplineIndex];

		m_vecSplinePoints.clear();

		m_vecSplinePoints.push_back(vChangePoint);
		
		m_strCurrentSplineTrack = L"분기1지점정답";

		auto& iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

		_int iNewSplineVectorSize = _int(iter->second.size());

		for (_int i = 0; i < iNewSplineVectorSize; ++i)
		{
			m_vecSplinePoints.push_back(iter->second[i]);
		}
		
		auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);
		
		Set_SplineSpeed(Speediter->second);
		
		m_iCurrentSplineIndex = 0;
		m_bSpline = true;
		m_fSplineTimeAcc = 0.f;
	}

	

	if (m_vecSplinePoints.size() <= m_iCurrentSplineIndex + 3)
	{
		if (m_strCurrentSplineTrack == L"분기1지점정답")
		{
			if (m_vecPointChecks[1] == true)
			{	
				m_vecSplinePoints.clear();
				//m_strCurrentSplineTrack = L"분기2지점정답";
				m_strCurrentSplineTrack = L"분기2지점정답스피드존";
				
				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		//else if (m_strCurrentSplineTrack == L"분기2지점정답")
		else if (m_strCurrentSplineTrack == L"분기2지점정답스피드존")
		{
			
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기2지점정답스피드2";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = (_int)iter->second.size();

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
		}
		else if (m_strCurrentSplineTrack == L"분기2지점정답스피드2")
		{
			if (m_vecPointChecks[3] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기3지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기3지점정답")
		{
			if (m_vecPointChecks[3] == true)
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점정답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(130.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
			else
			{
				m_vecSplinePoints.clear();
				m_strCurrentSplineTrack = L"분기4지점오답";

				auto iter = m_mapSplineVectors.find(m_strCurrentSplineTrack);

				_int iNewSplineVectorSize = _int(iter->second.size());

				for (_int i = 0; i < iNewSplineVectorSize; ++i)
				{
					m_vecSplinePoints.push_back(iter->second[i]);
				}

				auto& Speediter = m_mapSplineSpeeds.find(m_strCurrentSplineTrack);

				Set_SplineSpeed(Speediter->second);
				m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(125.f));
				m_iCurrentSplineIndex = 0;
				m_bSpline = true;
				m_fSplineTimeAcc = 0.f;
			}
		}
		else if (m_strCurrentSplineTrack == L"분기1지점오답" || m_strCurrentSplineTrack == L"분기2지점오답" || m_strCurrentSplineTrack == L"분기3지점오답" || m_strCurrentSplineTrack == L"분기4지점오답" || m_strCurrentSplineTrack == L"분기4지점정답")
		{
			m_bSpline = false;
			m_fSplineTimeAcc = 0.f;
		}
		return;
	}

	_vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	_vector vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, vCalcPosition;
	vMovePoint1 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex]);
	vMovePoint2 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 1]);
	vMovePoint3 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 2]);
	vMovePoint4 = XMLoadFloat4(&m_vecSplinePoints[m_iCurrentSplineIndex + 3]);
	
	vCalcPosition = CatmullRomInterpolation(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, (_float)t);
	//vCalcPosition = XMVectorCatmullRom(vMovePoint1, vMovePoint2, vMovePoint3, vMovePoint4, t);

	if (false == m_pTransformCom->Go_TargetArrivalCheck(vCalcPosition, fTimeDelta, m_fSplineEndRaidus) )
	{
		
		m_fSplineTimeAcc = 0.0f;
		++m_iCurrentSplineIndex;
	}
	else if (m_fSplineTimeAcc >= 1.0f)
	{
		m_fSplineTimeAcc = 0.0f;
	}

	_float3 vOffsetPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vOffsetPosition.y += 1.5f;

	m_pPlayer->Set_Position(vOffsetPosition);
	

}

_vector CEnvironment_Interact::CatmullRomInterpolation(_fvector p0, _fvector p1, _fvector p2, _fvector p3, _float t)
{
	 float t2 = t * t;
    float t3 = t * t * t;
    XMFLOAT3 result;

	_float3 ps0, ps1, ps2, ps3;

	XMStoreFloat3(&ps0, p0);
	XMStoreFloat3(&ps1, p1);
	XMStoreFloat3(&ps2, p2);
	XMStoreFloat3(&ps3, p3);

    result.x = 0.5f * ((2.0f * ps1.x) +
                        (-ps0.x + ps2.x) * t +
                        (2.0f * ps0.x - 5.0f * ps1.x + 4.0f * ps2.x - ps3.x) * t2 +
                        (-ps0.x + 3.0f * ps1.x - 3.0f * ps2.x + ps3.x) * t3);

    result.y = 0.5f * ((2.0f * ps1.y) +
                        (-ps0.y + ps2.y) * t +
                        (2.0f * ps0.y - 5.0f * ps1.y + 4.0f * ps2.y - ps3.y) * t2 +
                        (-ps0.y + 3.0f * ps1.y - 3.0f * ps2.y + ps3.y) * t3);

    result.z = 0.5f * ((2.0f * ps1.z) +
                        (-ps0.z + ps2.z) * t +
                        (2.0f * ps0.z - 5.0f * ps1.z + 4.0f * ps2.z - ps3.z) * t2 +
                        (-ps0.z + 3.0f * ps1.z - 3.0f * ps2.z + ps3.z) * t3);

    return XMVectorSet(result.x, result.y, result.z, 1.f);
}

vector<_float4> CEnvironment_Interact::CreateSmoothSpline(vector<_float4>& points, _int segments)
{
	vector<_float4> splinePoints;

	for (int i = 0; i < points.size() - 3; ++i) {
		for (int j = 0; j <= segments; ++j) {
			float t = static_cast<float>(j) / segments;
			_float4 p = CatmullRomInterpolation(points[i], points[i + 1], points[i + 2], points[i + 3], t);
			splinePoints.push_back(p);
		}
	}

	return splinePoints;
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




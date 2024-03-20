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


	return S_OK;
}

void CEnvironment_Interact::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_Interact::Tick(_float fTimeDelta)
{

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
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", (_uint)i, aiTextureType_NORMALS);
		
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
			if (true == m_pPlayer->Is_Animation_End() && m_pGameInstance->Get_NextLevel() != (_uint)LEVEL_TOOL)
			{
				//m_pPlayer->Is_Inputable_Front()
				//m_pPlayer->Get_Body()->Get_Model()->Get_TrackPosition()
				//m_pPlayer->Get_Actor()->Free()
				m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_tEnvironmentDesc.eChangeLevel));
				m_bInteract = false;
			}
			
		}
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




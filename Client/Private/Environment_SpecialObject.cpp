#include "stdafx.h"
#include "..\Public\Environment_SpecialObject.h"

#include "GameInstance.h"
#include "Environment_LightObject.h"
#include "Environment_Interact.h"
#include "Data_Manager.h"
#include "UI.h"
#include "Cell.h"
#include "SMath.h"
#include "Player.h"
#include "UI_Weakness.h"

CEnvironment_SpecialObject::CEnvironment_SpecialObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{

}

CEnvironment_SpecialObject::CEnvironment_SpecialObject(const CEnvironment_SpecialObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_SpecialObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Initialize(void* pArg)
{
	m_tEnvironmentDesc = *(ENVIRONMENT_SPECIALOBJECT_DESC*)pArg;

	m_iCurrnetLevel = m_pGameInstance->Get_NextLevel();

	
	m_tEnvironmentDesc.strDiffuseTextureTag = L"Prototype_Component_Texture_SpecialSignalDiffuseTexture";


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (false == m_tEnvironmentDesc.bPreview)
		m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);


	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		m_pModelCom->Set_Animation(m_tEnvironmentDesc.iPlayAnimationIndex);
	}

	

	if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		SignalInit();
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
	{
		if(FAILED(TrackLeverInit()))
			return E_FAIL;
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
	{
		if(FAILED(ElevatorInit()))
			return E_FAIL;
	}

	
	m_pSnowMountainWagon = CData_Manager::GetInstance()->Get_SnowMountainWagon();

	m_tEnvironmentDesc.vArrivalPosition.w = 1.f;
	return S_OK;
}

void CEnvironment_SpecialObject::Priority_Tick(_float fTimeDelta)
{
}

void CEnvironment_SpecialObject::Tick(_float fTimeDelta)
{
	if (m_iCurrnetLevel == (_uint)LEVEL_TOOL && m_bFindPlayer == false)
	{
		CCharacter* pPlayer = m_pGameInstance->Get_Player();
		if (nullptr != pPlayer)
		{
			m_pPlayer = dynamic_cast<CPlayer*>(pPlayer);
			m_bFindPlayer = true;
		}
	}

	m_fTimeAcc += fTimeDelta * 0.5f;

	if(m_fTimeAcc >= 180.f)
		m_fTimeAcc = 0.f;

	if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		SignalFunction(fTimeDelta);
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
	{
		TrackLeverFunction();
	}
	else if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
	{
		if(m_pElevatorColliderCom == nullptr)
			return;

		m_pElevatorColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
		ElevatorFunction(fTimeDelta);
	}
	
}

void CEnvironment_SpecialObject::Late_Tick(_float fTimeDelta)
{
	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		_float3 vRootAnimPos = {};

		//m_pModelCom->Play_Animation(fTimeDelta, true);
		m_pModelCom->Play_Animation(fTimeDelta, &vRootAnimPos);

		m_pTransformCom->Add_RootBone_Position(vRootAnimPos);
	}

	FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), );

	
	if (m_pGameInstance->Get_CurrentLevel() == (_uint)LEVEL_TOOL && m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR && m_pElevatorColliderCom != nullptr)
	{
		m_pGameInstance->Add_DebugRender(m_pElevatorColliderCom);
	//	m_pGameInstance->Add_DebugRender(m_pPickingCollider);
	}
}

HRESULT CEnvironment_SpecialObject::Render()
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

		if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER)
		{
			if (m_bLeverOn == true)
				m_pShaderCom->Begin(m_iSignalMeshShaderPass);
			else
				m_pShaderCom->Begin(8);
		}
		else if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
		{
			m_pShaderCom->Begin(0);
		}
		else
		{
			m_pShaderCom->Begin(m_iSignalMeshShaderPass);
		}
		
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Render_Shadow()
{
	_float lightFarValue = m_pGameInstance->Get_ShadowLightFar(m_iCurrnetLevel);

	FAILED_CHECK(m_pShaderCom->Bind_RawValue("g_fLightFar", &lightFarValue, sizeof(_float)));
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_ShadowLightViewMatrix(m_pGameInstance->Get_NextLevel())));
	FAILED_CHECK(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_ShadowLightProjMatrix(m_pGameInstance->Get_NextLevel())));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", (_uint)i, aiTextureType_DIFFUSE);
		m_pShaderCom->Begin(ECast(MODEL_SHADER::MODEL_SHADOW));
		m_pModelCom->Render((_uint)i);
	}

	return S_OK;
}

_bool CEnvironment_SpecialObject::Write_Json(json& Out_Json)
{
	return __super::Write_Json(Out_Json);
}

void CEnvironment_SpecialObject::Load_FromJson(const json& In_Json)
{
	return __super::Load_FromJson(In_Json);
}

void CEnvironment_SpecialObject::Set_AnimationIndex(_int iAnimIndex)
{
	m_pModelCom->Set_Animation(iAnimIndex);
}

#ifdef _DEBUG

_bool CEnvironment_SpecialObject::Picking(_float3* vPickedPos)
{
	GRAPHIC_DESC GraphicDesc = *m_pGameInstance->Get_GraphicDesc();

	HWND hWnd = GraphicDesc.hWnd;

	_int iWinSizeX = GraphicDesc.iBackBufferSizeX;
	_int iWinSizeY = GraphicDesc.iBackBufferSizeY;

	RAY ray = m_pGameInstance->Get_MouseRayLocal(hWnd, iWinSizeX, iWinSizeY, m_pTransformCom->Get_WorldMatrix());
	vector<class CMesh*> meshes = m_pModelCom->Get_Meshes();

	return m_pGameInstance->Picking_Mesh(ray, vPickedPos, meshes);
}

_bool CEnvironment_SpecialObject::Picking_VerJSY(RAY* pRay, _float3* vPickedPos)
{
	_vector vOrigin = XMLoadFloat4(&pRay->vPosition);
	_vector vDir = XMLoadFloat3(&pRay->vDirection);


	if (true == m_pGameInstance->isIn_WorldPlanes(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		_float fDist;


		CBounding_Sphere* pBoundingSphere = dynamic_cast<CBounding_Sphere*>(m_pPickingCollider->Get_Bounding());

		if (pBoundingSphere == nullptr)
		{
			MSG_BOX("콜라이더 없대");
			return false;
		}

		const BoundingSphere* pBoundingSphereBox = pBoundingSphere->Get_Bounding();

		if (true == pBoundingSphereBox->Intersects(vOrigin, vDir, fDist))
		{
			*vPickedPos = vOrigin + fDist * vDir;
			return true;
		}
	}
	else
		return false;


	return false;
}



#endif

void CEnvironment_SpecialObject::SignalInit()
{
	m_vSignalColor = m_vRedSignal;
	m_bSignalChange = false;
	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);
	m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(90.f));
}

void CEnvironment_SpecialObject::SignalFunction(const _float fTimeDelta)
{
	if (m_bSignalChange == true && m_bChangeLerp == false)
	{
		m_bChangeLerp = m_pTransformCom->Rotation_Lerp(XMConvertToRadians(90.f), fTimeDelta, 1.f);
	}
}

void CEnvironment_SpecialObject::Set_SignalChange(_bool bChange)
{
	#ifdef _DEBUG
		if (m_tEnvironmentDesc.eSpecialType != CEnvironment_SpecialObject::SPECIAL_SIGNAL)
		{
			MSG_BOX("CEnvironment_SpecialObject : 스페셜타입이 시그널이 아닙니다.");
			return;
		}
	#endif // _DEBUG

	m_bSignalChange = bChange; 
	
	if (m_bSignalChange == true)
		m_vSignalColor = m_vGreenSignal;
	else
		m_vSignalColor = m_vRedSignal;

	
}

HRESULT CEnvironment_SpecialObject::TrackLeverInit()
{
	m_pLeverWeaknessUI = dynamic_cast<CUI*>(m_pGameInstance->Add_CloneObject_And_Get(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Weakness")));

	m_pLeverWeaknessUI->Set_Active(true);

	dynamic_cast<CUI_Weakness*>(m_pLeverWeaknessUI)->Set_ColliderRadius(3.f);

	if(nullptr == m_pLeverWeaknessUI)
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(m_tEnvironmentDesc.WorldMatrix);

	m_pLeverWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 0.f, 0.f));

	if (nullptr != m_pLightObject)
		m_pLightObject->Set_Enable(false);

	

	m_bLeverOn = false;
	m_bSignalChange = false;

	if (m_iCurrnetLevel == (_uint)LEVEL_SNOWMOUNTAIN)
	{
		if (m_tEnvironmentDesc.bLeverForElevator == true)
		{
			return Find_ElevatorObject();
		}
		else
		{
			return Find_SignalBox_AndLightObject();
		}
	}
	

	

	


	return S_OK;

	
}

HRESULT CEnvironment_SpecialObject::Find_SignalBox_AndLightObject()
{
	list<CGameObject*> BackGroundObjects = *m_pGameInstance->Get_GameObjects(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround");
	
	if(true == BackGroundObjects.empty())
		return E_FAIL;

	for (auto& iter : BackGroundObjects)
	{
		if (typeid(*iter) == typeid(CEnvironment_SpecialObject))
		{
			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(iter);

			if(pSpecialObject == nullptr)
				return E_FAIL;
			else
			{
				if (pSpecialObject->Get_SpecialType() == CEnvironment_SpecialObject::SPECIAL_SIGNAL && pSpecialObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pTargetObject = pSpecialObject;
				}
				else 
					continue;
			}
		}
		else if (typeid(*iter) == typeid(CEnvironment_LightObject))
		{
			CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(iter);

			if (pLightObject == nullptr)
				return E_FAIL;
			else
			{
				if (pLightObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pLightObject = pLightObject;
				}
				else
					continue;
			}
		}
	}


	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Find_ElevatorObject()
{
	list<CGameObject*> BackGroundObjects = *m_pGameInstance->Get_GameObjects(LEVEL_SNOWMOUNTAIN, L"Layer_BackGround");

	if (true == BackGroundObjects.empty())
		return E_FAIL;

	for (auto& iter : BackGroundObjects)
	{
		if (typeid(*iter) == typeid(CEnvironment_SpecialObject))
		{
			CEnvironment_SpecialObject* pSpecialObject = dynamic_cast<CEnvironment_SpecialObject*>(iter);

			if (pSpecialObject == nullptr)
				return E_FAIL;
			else
			{
				if (pSpecialObject->Get_SpecialType() == CEnvironment_SpecialObject::SPECIAL_ELEVATOR && pSpecialObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pTargetObject = pSpecialObject;
				}
				else
					continue;
			}
		}
		else if (typeid(*iter) == typeid(CEnvironment_LightObject))
		{
			CEnvironment_LightObject* pLightObject = dynamic_cast<CEnvironment_LightObject*>(iter);

			if (pLightObject == nullptr)
				return E_FAIL;
			else
			{
				if (pLightObject->Get_SpecialGroupIndex() == m_tEnvironmentDesc.iSpecialGroupIndex)
				{
					m_pLightObject = pLightObject;
				}
				else
					continue;
			}
		}
	}

	return S_OK;
}

void CEnvironment_SpecialObject::TrackLeverFunction()
{
	if (m_pLeverWeaknessUI != nullptr)
	{
		if (m_pLeverWeaknessUI->Get_Enable() == false) 
		{
			if (m_pTargetObject != nullptr)
			{
				if (m_tEnvironmentDesc.bLeverForElevator == true)
				{	
					if (m_pTargetObject->Get_ElevatorOn() == false)
					{
						m_pTargetObject->Set_ElevatorOn(true);
						m_bLeverOn = true;

						if(m_pLightObject != nullptr)
							m_pLightObject->Set_Enable(true);
					}
					else
					{
						m_pLeverWeaknessUI->Set_Enable(true);

						if (m_pLightObject != nullptr)
							m_pLightObject->Set_Enable(false);

					}
					
				}
				else if(m_pSnowMountainWagon != nullptr)
				{
					m_pTargetObject->Set_SignalChange(true);
					m_pLeverWeaknessUI->Set_Dead(true);
					m_pLeverWeaknessUI = nullptr;

					if (nullptr != m_pLightObject)
						m_pLightObject->Set_Enable(true);

					m_pSnowMountainWagon->Set_SplineCheck(m_tEnvironmentDesc.iSpecialGroupIndex, true);
					m_bLeverOn = true;
				}
			}

			
		}
		else
		{
			m_pLeverWeaknessUI->SetUp_WorldToScreen(m_pTransformCom->Get_WorldFloat4x4(), _float3(0.f, 1.f, 0.f));
		}
	}

	if (m_tEnvironmentDesc.bOffset == true)
	{
		Move_ForOffset();
	}
}

void CEnvironment_SpecialObject::Move_ForOffset()
{
		if (m_pTargetObject == nullptr)
			return;

		_float4 vOwnerPosition = m_pTargetObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vCalcPosition = XMLoadFloat4(&vOwnerPosition) + XMLoadFloat4(&m_tEnvironmentDesc.vOffset);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCalcPosition);

}


HRESULT CEnvironment_SpecialObject::ElevatorInit()
{
	if (m_iCurrnetLevel != (_uint)LEVEL_TOOL)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_Player());
		Safe_AddRef(m_pPlayer);
		m_bFindPlayer = true;
		m_bElevatorOn = false;
	}
	
	
	
	Set_Speed(m_tEnvironmentDesc.fElevatorSpeed);
	Set_RotationSpeed(m_tEnvironmentDesc.fElevatorRotationSpeed);
	
	m_vInitPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_iInitCellIndex = m_tEnvironmentDesc.iArrivalCellIndex;
	return S_OK;
}

void CEnvironment_SpecialObject::ElevatorFunction(const _float fTimeDelta)
{

	_float4 vPosition = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);

	if (m_bArrival == false && m_bElevatorOn == true)
	{
		if (m_pElevatorColliderCom == nullptr || m_pPlayer == nullptr)
			return;

		_float3 vPlayerPos = m_pPlayer->Get_Position();

		if (true == m_pElevatorColliderCom->Is_Collision(m_pPlayer->Get_Collider())) //! 충돌 중이라면 플레이어  y값 고정
		{
			m_pPlayer->Get_Navigation()->Set_InteractMoveMode(true);
			
			if (m_bFirstCollision == false)
			{
				m_bFirstCollision = true;
				m_iInitCellIndex = m_pPlayer->Get_Navigation()->Get_CurrentCellIndex();
			}
			

			switch (m_tEnvironmentDesc.eElevatorType)
			{
				case CEnvironment_SpecialObject::ELEVATOR_UP:
				{
					if (m_tEnvironmentDesc.fElevatorMaxHeight > vPosition.y)
					{
						m_pTransformCom->Go_Up(fTimeDelta, nullptr);
					}
					else
					{
						m_bArrival = true;
						m_bElevatorOn = false;
						m_pPlayer->Get_Navigation()->Set_CurrentIndex(m_tEnvironmentDesc.iArrivalCellIndex);
						m_pPlayer->Get_Navigation()->Set_InteractMoveMode(false);
						m_pPlayer->Set_UseGravity(true);
						m_bFirstCollision = false;
						
					}

					break;
				}

				case CEnvironment_SpecialObject::ELEVATOR_DOWN:
				{
					if (m_tEnvironmentDesc.fElevatorMinHeight < vPosition.y)
					{
						m_pTransformCom->Go_Down(fTimeDelta, nullptr);
					}
					else
					{
						m_bArrival = true;
						m_bElevatorOn = false;
						m_pPlayer->Get_Navigation()->Set_CurrentIndex(m_tEnvironmentDesc.iArrivalCellIndex);
						m_pPlayer->Get_Navigation()->Set_InteractMoveMode(false);
						m_pPlayer->Set_UseGravity(true);
						m_bFirstCollision = false;
					}

					break;
				}

				case CEnvironment_SpecialObject::ELEVATOR_TARGET:
				{
					if (false == SMath::Is_InRange(m_tEnvironmentDesc.vArrivalPosition, vPosition, 0.5f))
					{
						m_pTransformCom->Go_Target(m_tEnvironmentDesc.vArrivalPosition, fTimeDelta, 0.5f);
					}
					else
					{
						
						m_bArrival = true;
						
						m_bElevatorOn = false;
						m_pPlayer->Get_Navigation()->Set_CurrentIndex(m_tEnvironmentDesc.iArrivalCellIndex);
						m_pPlayer->Get_Navigation()->Set_InteractMoveMode(false);
						m_pPlayer->Set_UseGravity(true);
						m_bFirstCollision = false;
					}

					break;
				}
			}

			_float3 vMinCorner = dynamic_cast<CBounding_AABB*>(m_pElevatorColliderCom->Get_Bounding())->Get_MinCorner();
			_float3 vMaxCorner = dynamic_cast<CBounding_AABB*>(m_pElevatorColliderCom->Get_Bounding())->Get_MaxCorner();



			vPlayerPos.y = vMinCorner.y;



			if (vPlayerPos.x < vMinCorner.x)
			{
				vPlayerPos.x = vMinCorner.x;
			}
			else if (vPlayerPos.x > vMaxCorner.x)
			{
				vPlayerPos.x = vMaxCorner.x;
			}


			if (vPlayerPos.z < vMinCorner.z)
			{
				vPlayerPos.z = vMinCorner.z;
			}
			else if (vPlayerPos.z > vMaxCorner.z)
			{
				vPlayerPos.z = vMaxCorner.z;
			}

			m_pPlayer->Set_Position(vPlayerPos);
		}
		else
		{
			m_bElevatorOn = false;
		}
	}

}

void CEnvironment_SpecialObject::Set_ElevatorOn(_bool bElevatorOn)
{
	if (bElevatorOn == true)
	{
		switch (m_tEnvironmentDesc.eElevatorType)
		{
			case CEnvironment_SpecialObject::ELEVATOR_UP:
			{
				if (m_bArrival == true)
				{
					m_tEnvironmentDesc.eElevatorType = CEnvironment_SpecialObject::ELEVATOR_DOWN;
					m_bArrival = false;
					_int iSwapCellIndex = m_tEnvironmentDesc.iArrivalCellIndex;
					m_tEnvironmentDesc.iArrivalCellIndex = m_iInitCellIndex;
					m_iInitCellIndex = iSwapCellIndex;
				}
				

				break;
			}

			case CEnvironment_SpecialObject::ELEVATOR_DOWN:
			{
				if (m_bArrival == true)
				{
					m_tEnvironmentDesc.eElevatorType = CEnvironment_SpecialObject::ELEVATOR_UP;
					m_bArrival = false;
					_int iSwapCellIndex = m_tEnvironmentDesc.iArrivalCellIndex;
					m_tEnvironmentDesc.iArrivalCellIndex = m_iInitCellIndex;
					m_iInitCellIndex = iSwapCellIndex;
				}

				break;
			}

			case CEnvironment_SpecialObject::ELEVATOR_TARGET:
			{
				if (m_bArrival == true) //! 이미 도착해있었다면
				{
					_float4 vSwapPosition = m_tEnvironmentDesc.vArrivalPosition;
					m_tEnvironmentDesc.vArrivalPosition = m_vInitPosition;
					m_vInitPosition = vSwapPosition;
					m_bArrival = false;

					_int iSwapCellIndex = m_tEnvironmentDesc.iArrivalCellIndex;
					m_tEnvironmentDesc.iArrivalCellIndex = m_iInitCellIndex;
					m_iInitCellIndex = iSwapCellIndex;
				}
				
				
				
			}
		
		}
	}

	m_bElevatorOn = bElevatorOn;

	
}

void CEnvironment_SpecialObject::Set_Speed(_float fSpeed)
{
	m_tEnvironmentDesc.fElevatorSpeed = fSpeed;
	m_pTransformCom->Set_Speed(fSpeed);
}

void CEnvironment_SpecialObject::Set_RotationSpeed(_float fRotationSpeed)
{
	m_tEnvironmentDesc.fElevatorRotationSpeed = XMConvertToRadians(fRotationSpeed);
	m_pTransformCom->Set_RotationSpeed(XMConvertToRadians(fRotationSpeed));
}

void CEnvironment_SpecialObject::Set_ElevatorColliderSize(_float3 vElevatorColliderSize)
{
	{
		CBounding* pBounding = m_pElevatorColliderCom->Get_Bounding();

		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Extents = vElevatorColliderSize;
		m_tEnvironmentDesc.vElevatorColliderSize = vElevatorColliderSize;
	}
}

void CEnvironment_SpecialObject::Set_ElevatorColliderCenter(_float3 vElevatorColliderCenter)
{
	{
		CBounding* pBounding = m_pElevatorColliderCom->Get_Bounding();


		CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(pBounding);

		if (pAABB == nullptr)
			return;

		BoundingBox* pBox = pAABB->Get_OriginBounding();

		pBox->Center = vElevatorColliderCenter;
		m_tEnvironmentDesc.vElevatorColliderCenter = vElevatorColliderCenter;
	}
}

void CEnvironment_SpecialObject::Set_ElevatorInit()
{
	m_pTransformCom->Set_Position(_float3(m_vInitPosition.x, m_vInitPosition.y, m_vInitPosition.z));
	m_bArrival = false;
	m_bElevatorOn = false;
}


void CEnvironment_SpecialObject::Add_UpdateCellIndex(_int iCellIndex)
{
	m_tEnvironmentDesc.vecUpdateCellIndexs.push_back(iCellIndex);
}

void CEnvironment_SpecialObject::Enable_UpdateCells()
{
	if (m_pPlayer == nullptr)
		return;

	CNavigation* pNavigation = m_pPlayer->Get_Navigation();

	if (pNavigation == nullptr)
		return;


	_int iUpdateCellCount = m_tEnvironmentDesc.vecUpdateCellIndexs.size();

	for (_int i = 0; i < iUpdateCellCount; ++i)
	{
		pNavigation->Set_MoveEnableForCellIndex(m_tEnvironmentDesc.vecUpdateCellIndexs[i]);
	}
}

void CEnvironment_SpecialObject::UnEnable_UpdateCells()
{
	if (m_pPlayer == nullptr)
		return;

	CNavigation* pNavigation = m_pPlayer->Get_Navigation();

	if (pNavigation == nullptr)
		return;

	_int iUpdateCellCount = m_tEnvironmentDesc.vecUpdateCellIndexs.size();

	for (_int i = 0; i < iUpdateCellCount; ++i)
	{
		pNavigation->Set_MoveUnEnableForCellIndex(m_tEnvironmentDesc.vecUpdateCellIndexs[i]);
	}
}



HRESULT CEnvironment_SpecialObject::Ready_Components()
{

	if (true == m_tEnvironmentDesc.bAnimModel)
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}
	else
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom)));
	}

	if (m_tEnvironmentDesc.strDiffuseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strDiffuseTextureTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTexture)));
	}

	if (m_tEnvironmentDesc.strMaskTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strMaskTextureTag, TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexture)));
	}

	if (m_tEnvironmentDesc.strNoiseTextureTag != L"")
	{
		FAILED_CHECK(__super::Add_Component(m_iCurrnetLevel, m_tEnvironmentDesc.strNoiseTextureTag, TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTexture)));
	}


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_NextLevel(), m_tEnvironmentDesc.strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
	{
		CBounding_AABB::BOUNDING_AABB_DESC		BoundingDesc = {};

		/* For.Com_Collider */
		BoundingDesc.iLayer = ECast(COLLISION_LAYER::NONE);
		BoundingDesc.vExtents = m_tEnvironmentDesc.vElevatorColliderSize;
		BoundingDesc.vCenter = m_tEnvironmentDesc.vElevatorColliderCenter;


		if (FAILED(__super::Add_Component(m_iCurrnetLevel, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_ElevatorCollider"), reinterpret_cast<CComponent**>(&m_pElevatorColliderCom), &BoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEnvironment_SpecialObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	
	_float fCamFar = m_pGameInstance->Get_CamFar();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	
	if (nullptr != m_pDiffuseTexture)
	{
		if (FAILED(m_pDiffuseTexture->Bind_ShaderResource(m_pShaderCom, "g_ColorDiffuse")))
			return E_FAIL;
	}

	if (nullptr != m_pMaskTexture)
	{
		if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (nullptr != m_pNoiseTexture)
	{
		if (FAILED(m_pNoiseTexture->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
			return E_FAIL;
	}
	

	if (m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_SIGNAL)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &m_vSignalColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_fSignalBloomPower, sizeof(_float))))
			return E_FAIL;
	}
	else if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_TRACKLEVER && m_bLeverOn == true)
	{
		_float4 GreenColor = { 0.f, 1.f, 0.f, 1.f};
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &GreenColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBloomPower", &m_fSignalBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

CEnvironment_SpecialObject* CEnvironment_SpecialObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CEnvironment_SpecialObject* pInstance = new CEnvironment_SpecialObject(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironment_SpecialObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_SpecialObject::Clone(void* pArg)
{
	CEnvironment_SpecialObject* pInstance = new CEnvironment_SpecialObject(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironment_SpecialObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEnvironment_SpecialObject::Pool()
{
	return new CEnvironment_SpecialObject(*this);
}

void CEnvironment_SpecialObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);


	if (m_iCurrnetLevel == (_uint)LEVEL_TOOL)
		Safe_Release(m_pPickingCollider);

	if(m_tEnvironmentDesc.eSpecialType == CEnvironment_SpecialObject::SPECIAL_ELEVATOR)
		Safe_Release(m_pPlayer);
}




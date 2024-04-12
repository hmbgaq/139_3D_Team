#include "Character_Client.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "SpringCamera.h"
#include "Data_Manager.h"
#include "UI_Manager.h"
#include "MasterCamera.h"

CCharacter_Client::CCharacter_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CCharacter(pDevice, pContext, strPrototypeTag)
	, m_pUIManager(CUI_Manager::GetInstance())
	, m_pDataManager(CData_Manager::GetInstance())
{
	/* UIManager Add */
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pDataManager);
}

CCharacter_Client::CCharacter_Client(const CCharacter_Client& rhs)
	: CCharacter(rhs)
	, m_pUIManager(rhs.m_pUIManager)
	, m_pDataManager(rhs.m_pDataManager)
{
	/* UIManager Add */
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pDataManager);
}

HRESULT CCharacter_Client::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CCharacter_Client::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}

void CCharacter_Client::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CCharacter_Client::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCharacter_Client::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (GAME_STATE::GAMEPLAY != m_pDataManager->Get_GameState())
		return;

	Check_Frustum();
	if (true == m_bIsInFrustum)
	{
		FAILED_CHECK_RETURN(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this), ); //m_bIsInFrustum

		if (false == m_bRootMoveEnd)
		{
			_float3 vBodyMovePos = m_pBody->Get_MovePos();

			_float fDiff = abs(vBodyMovePos.x) + abs(vBodyMovePos.y) + abs(vBodyMovePos.z);

			if (0.0001f < fDiff)
			{
				m_vAddRootPosition = vBodyMovePos;

				//_float3 vResult = vBodyMovePos;
				m_vAddRootPosition.x *= m_vRootMoveRate.x;
				m_vAddRootPosition.y *= m_vRootMoveRate.y;
				m_vAddRootPosition.z *= m_vRootMoveRate.z;
				//m_pTransformCom->Add_RootBone_Position(vResult, m_pNavigationCom);
				m_pTransformCom->Add_RootBone_Position(m_vAddRootPosition, fTimeDelta, m_pNavigationCom);
			}
			else
			{
				m_vAddRootPosition = {};
			}
		}

	}
	else
	{
		_uint i = 0;
	}

	m_pRigidBody->Late_Tick(fTimeDelta);

#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRender(m_pNavigationCom);
#endif	

	Set_WeaknessPos();
}

HRESULT CCharacter_Client::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

//CEffect* CCharacter_Client::Create_Effect(CGameObject* pOwner)
//{
//	string strEffectFileName = Get_CharcterDesc().EffectFileName;
//	if ("" == strEffectFileName)
//	{
//		strEffectFileName = "Hit_Normal";
//	}
//	//return EFFECT_MANAGER->Create_Effect("Hit/", strEffectFileName + ".json", pOwner);
//	return EFFECT_MANAGER->Play_Effect("Hit/", strEffectFileName + ".json", pOwner);
//}
//
//
//CEffect* CCharacter_Client::Create_Effect(_float3 vPos, CGameObject* pOwner)
//{
//	CEffect* pEffect = Create_Effect(pOwner);
//	if (pEffect) 
//		pEffect->Set_Position(vPos);
//
//	return pEffect;
//}

//CEffect* CCharacter_Client::Create_Effect(const wstring& strPartTag)
//{
//	CGameObject* pPartObject = Find_PartObject(strPartTag);
//	
//
//	//CWeapon* pWeapon = pActor->Get_Weapon(TEXT("Weapon_Punch_R"));
//	//_float3 vPos = pWeapon->Get_WorldPosition();
//
//	//pActor->Create_Effect(vPos);
//
//	return nullptr;
//}

void CCharacter_Client::Reset_UpperAngle()
{
	if (nullptr == m_pDataManager) m_pDataManager = CData_Manager::GetInstance();
	//_float fPitch = m_pDataManager->Get_MasterCamera()->Get_SpringCamera()->Get_Pitch();
	m_pDataManager->Get_MasterCamera()->Get_SpringCamera()->Reset_Angle();

	//m_pBody->Reset_UpperAngle(fPitch);
	m_pBody->Reset_UpperAngle();
}

void CCharacter_Client::Apply_Shake_And_Blur(Power ePower)
{
	if (nullptr == m_pDataManager)
	{
		m_pDataManager = CData_Manager::GetInstance();
	}

	CSpringCamera* pSpringCam = m_pDataManager->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_ShakeCamera(true);
	}

	m_pGameInstance->Set_RadialBlurTime(0.2f * ECast(ePower));
}

void CCharacter_Client::Apply_Shake_And_Chroma(Power ePower)
{
	if (nullptr == m_pDataManager)
	{
		m_pDataManager = CData_Manager::GetInstance();
	}

	CSpringCamera* pSpringCam = m_pDataManager->Get_MasterCamera()->Get_SpringCamera();
	if (pSpringCam)
	{
		pSpringCam->Set_ShakeCamera(true);
	}

	m_pGameInstance->Set_RadialBlurTime(0.2f * ECast(ePower));
}

void CCharacter_Client::Create_Hitting_Effect(_float3 vPos, Power ePower, string strEffectName, CGameObject* pOwner)
{
	//string strEffectFileName;
	//strEffectFileName = strEffectName != "" ? strEffectName : Get_CharcterDesc().EffectFileName;
	//strEffectFileName = strEffectFileName != "" ? strEffectFileName : "Hit_Normal";


	
	//if (pEffect)
	//	pEffect->Set_Position(vPos);

	EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", vPos);


	Apply_Shake_And_Blur(ePower);
}

_bool CCharacter_Client::Check_EffectOnTrackPosition()
{
	return Compare_TrackPosition_Is_Over(Get_CharcterDesc().EffectOnTrackPosition);
}

void CCharacter_Client::Free()
{
	__super::Free();

	/* UIManager Delete */
	if(m_pUIManager)
		Safe_Release(m_pUIManager);

	if (m_pDataManager)
		Safe_Release(m_pDataManager);

}

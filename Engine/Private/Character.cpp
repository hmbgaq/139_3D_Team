#include "..\Public\Character.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "PhysXCharacterController.h"
#include "Bone.h"


CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	: CGameObject(pDevice, pContext, strPrototypeTag)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_pBody(rhs.m_pBody)
	, m_Weapons(rhs.m_Weapons)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pRigidBody = CRigidBody::Create(m_pDevice, m_pContext);
	if (nullptr == m_pRigidBody)
		return E_FAIL;
	if (nullptr != Find_Component(g_pRigidBodyTag))
		return E_FAIL;
	m_Components.emplace(g_pRigidBodyTag, m_pRigidBody);
	Safe_AddRef(m_pRigidBody);
	m_pRigidBody->Set_Owner(this);
	m_pRigidBody->Set_Transform(m_pTransformCom);


	return S_OK;
}

void CCharacter::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CCharacter::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Tick(fTimeDelta);
	}
}

void CCharacter::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& Pair : m_PartObjects)
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(fTimeDelta);
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	m_pTransformCom->Add_RootBone_Position(m_pBody->Get_MovePos(), m_pNavigationCom);

	m_pRigidBody->Late_Tick(fTimeDelta);
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugRender(m_pNavigationCom);
#endif	
}

HRESULT CCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CBody* CCharacter::Get_Body()
{
	return dynamic_cast<CBody*>(Find_PartObject(TEXT("Part_Body")));
}

CWeapon* CCharacter::Get_Weapon(const wstring& strWeaponTag)
{
	return dynamic_cast<CWeapon*>(Find_PartObject(strWeaponTag));
}

CCollider* CCharacter::Get_Collider()
{
	return m_pBody->Get_Collider();
}

CNavigation* CCharacter::Get_Navigation()
{
	return m_pNavigationCom;
}

CGameObject* CCharacter::Find_PartObject(const wstring& strPartTag)
{
	auto	iter = m_PartObjects.find(strPartTag);

	if (iter == m_PartObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CCharacter::Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg)
{
	if (nullptr != Find_PartObject(strPrototypeTag))
		return E_FAIL;

	CGameObject* pPartObject = m_pGameInstance->Clone_Prototype(strPrototypeTag, pArg);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PartObjects.emplace(strPartTag, pPartObject);

	pPartObject->Set_Object_Owner(this);

	return S_OK;
}

HRESULT CCharacter::Add_Body(const wstring& strPrototypeTag, CBody::BODY_DESC pArg)
{
	pArg.m_pParentTransform = m_pTransformCom;
	if (FAILED(Add_PartObject(strPrototypeTag, TEXT("Part_Body"), &pArg)))
		return E_FAIL;

	m_pBody = Get_Body();
	if (nullptr == m_pBody)
		return E_FAIL;

	Safe_AddRef(m_pBody);

	return S_OK;
}

HRESULT CCharacter::Add_Weapon(const wstring& strPrototypeTag, string strBoneName, CWeapon::WEAPON_DESC pArg, const wstring& strWeaponTag)
{
	if (nullptr == m_pBody)
		return E_FAIL;

	pArg.m_pSocketBone = m_pBody->Get_BonePtr(strBoneName.c_str());
	pArg.m_pParentTransform = m_pTransformCom;

	/* For.Part_Weapon*/
	if (FAILED(Add_PartObject(strPrototypeTag, strWeaponTag, &pArg)))
		return E_FAIL;

	CWeapon* pWeapon = Get_Weapon(strWeaponTag);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.push_back(pWeapon);
	Safe_AddRef(pWeapon);

	return S_OK;
}

void CCharacter::Set_EventNotify(string strPath, string JsonFileName)
{
	string strpath = "../../Client/Bin/DataFiles/"+strPath;
	LoadAnimJson(strpath, JsonFileName);
}

HRESULT CCharacter::LoadAnimJson(string strPath, string strFileName)
{
	json LoadJson = {};

	string strFullPath = strPath + "/" + strFileName;

	if (FAILED(CJson_Utility::Load_Json(strFullPath.c_str(), LoadJson)))
	{
		MSG_BOX("애니메이션툴 로드 실패");
		return E_FAIL;
	}

	json BodyJson = LoadJson["Body"];
	json WeaponJson = LoadJson["Weapon"];
	json EffectJson = LoadJson["Effect"];
	
	//CharAnimDesc.Body = LoadJson["Body"];
	CharAnimDesc.AnimationIndex = BodyJson["AnimIndex"];
	CharAnimDesc.Duration = BodyJson["AnimDuration"];
	CharAnimDesc.EventTrackposition = BodyJson["CurrentTrackPosition"];
	CharAnimDesc.AnimationSpeed = BodyJson["AnimationSpeed"];
	CharAnimDesc.BoneIndex = BodyJson["BoneIndex"];
	CharAnimDesc.ColliderSize = BodyJson["ColliderSize"];
	CharAnimDesc.ColliderTrackPositionOff = BodyJson["ColliderTrackPositionOff"];
	CharAnimDesc.ColliderTrackPositionOn = BodyJson["ColliderTrackPositionOn"];
	//Weapon

	if (WeaponJson != nullptr)
	{
		CharAnimDesc.BoneName = WeaponJson["BoneName"];
		CharAnimDesc.EventWeaponTrackPosition = BodyJson["CurrentTrackPosition"];
		CharAnimDesc.ColliderWeaponSize = WeaponJson["WeaponColliderSize"];
		CharAnimDesc.ColliderWeaponTrackPositionOn = WeaponJson["WeaponColliderTrackPositionOn"];
		CharAnimDesc.ColliderWeaponTrackPositionOff= WeaponJson["WeaponColliderTrackPositionOff"];

		CJson_Utility::Load_Float3(WeaponJson["GuizmoTranslation"], CharAnimDesc.GuizmoTranslatrion);
		CJson_Utility::Load_Float3(WeaponJson["GuizmoRotation"], CharAnimDesc.GuizmoRotation);
		CJson_Utility::Load_Float3(WeaponJson["GuizmoScale"], CharAnimDesc.GuizmoScale);
	}
	//Effect
	{
		CharAnimDesc.EffectFileName = EffectJson["EffectFileName"];
		CJson_Utility::Load_Float3(EffectJson["EffectPosition"], CharAnimDesc.EffectPosition);
		CharAnimDesc.EffectOnTrackPosition = EffectJson["EffectTrackPosition"];
	}

	string	EffectFileName = "";
	_float	EffectOnTrackPosition = { 0.f };
	_float3	EffectPosition = {};
	
	return S_OK;
}

void CCharacter::Set_Animation(_uint _iNextAnimation, CModel::ANIM_STATE _eAnimState, _bool _bIsTransition, _bool _bUseAnimationPos, _uint iTargetKeyFrameIndex)
{
	m_pBody->Set_Animation(_iNextAnimation, _eAnimState, _bIsTransition, _bUseAnimationPos, iTargetKeyFrameIndex);
}

_bool CCharacter::Is_Animation_End()
{
	return m_pBody->Is_Animation_End();
}

_bool CCharacter::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_pBody->Is_Inputable_Front(_iIndexFront);
}

_float CCharacter::Get_TrackPosition()
{
	return m_pBody->Get_TrackPosition();
}

void CCharacter::Go_Straight(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Straight_L45(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight_L45(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Straight_R45(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight_R45(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Backward(_float fTimeDelta)
{
	m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Backward_L45(_float fTimeDelta)
{
	m_pTransformCom->Go_Backward_L45(fTimeDelta, m_pNavigationCom);
}
void CCharacter::Go_Backward_R45(_float fTimeDelta)
{
	m_pTransformCom->Go_Backward_R45(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Left(_float fTimeDelta)
{
	m_pTransformCom->Go_Left(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Go_Right(_float fTimeDelta)
{
	m_pTransformCom->Go_Right(fTimeDelta, m_pNavigationCom);
}

void CCharacter::Set_Enable(_bool _Enable)
{
	__super::Set_Enable(_Enable);

	if (false == _Enable && true == m_bIsPoolObject)
	{
		Safe_Release(m_pBody);

		for (CWeapon* pWeapon : m_Weapons)
		{
			Safe_Release(pWeapon);
		}
		m_Weapons.clear();

		for (auto& Pair : m_PartObjects)
			Safe_Release(Pair.second);

		m_PartObjects.clear();
	}


}

Hit_Type CCharacter::Set_Hitted(_uint iDamage, _vector vDir, _float fForce, _float fStiffnessRate, Direction eHitDirection, Power eHitPower)
{
	Hit_Type eHitType = Hit_Type::None;

	if (Power::Absolute == m_eStrength)
	{
		return Hit_Type::None;
	}

	//Get_Damaged(iDamage);
	//Set_InvincibleTime(fInvincibleTime);
	Add_Force(vDir, fForce);
	m_pTransformCom->Look_At_Direction(vDir * -1);

	if (m_iHp <= 0)
	{
		Hitted_Dead(eHitPower);
		//eHitType = Hit_Type::Hit_Finish;
	}
	else if (eHitPower >= m_eStrength)
	{
		eHitType = Hit_Type::Hit;

		switch (eHitDirection)
		{
		case Engine::Left:
			Hitted_Left(eHitPower);
			break;
		case Engine::Right:
			Hitted_Right(eHitPower);
			break;
		default:
			Hitted_Front(eHitPower);
			break;
		}
		//Set_StiffnessRate(fStiffnessRate);
	}

	return eHitType;
}

//Hit_Type CCharacter::Set_Hitted(_uint iDamage, _float3 vForce, _float fStiffnessRate, Direction eHitDirection, Power eHitPower)
//{
//	Hit_Type eHitType = Hit_Type::None;
//
//	if (Power::Absolute == m_eStrength)
//	{
//		return Hit_Type::None;
//	}
//
//	//Get_Damaged(iDamage);
//	//Set_InvincibleTime(fInvincibleTime);
//
//	if (m_iHp <= 0)
//	{
//		Add_Force(,vForce);
//		Hitted_Dead();
//		//eHitType = Hit_Type::Hit_Finish;
//	}
//	else if (eHitPower >= m_eStrength)
//	{
//		eHitType = Hit_Type::Hit;
//
//		//Add_Force(vForce);
//
//		switch (eHitDirection)
//		{
//		case Engine::Left:
//			Hitted_Right();
//			break;
//		case Engine::Right:
//			Hitted_Left();
//			break;
//		default:
//			Hitted_Front();
//			break;
//		}
//		//Set_StiffnessRate(fStiffnessRate);
//	}
//
//	return eHitType;
//}


void CCharacter::Add_Force(_vector In_vDir, _float In_fPower)
{
	m_pRigidBody->Add_Force(In_vDir, In_fPower);
}




_bool CCharacter::Picking(_Out_ _float3* vPickedPos)
{
	return m_pBody->Picking(vPickedPos);
}

void CCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pBody);

	for (CWeapon* pWeapon : m_Weapons)
	{
		Safe_Release(pWeapon);
	}
	m_Weapons.clear();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();

	if (m_pNavigationCom)
		Safe_Release(m_pNavigationCom);

	
}

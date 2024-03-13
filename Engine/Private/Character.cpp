#include "Character.h"
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
	FAILED_CHECK(__super::Initialize(pArg));

	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	_int iCurrentLevel = m_pGameInstance->Get_NextLevel();
	
	if (FAILED(__super::Add_Component(iCurrentLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	m_pNavigationCom->Set_CurrentIndex(m_pNavigationCom->Get_SelectRangeCellIndex(this));
	

	FAILED_CHECK(Ready_Components());

	FAILED_CHECK(Ready_PartObjects());

	m_pRigidBody = CRigidBody::Create(m_pDevice, m_pContext);

	NULL_CHECK_RETURN(m_pRigidBody, E_FAIL);

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
	FAILED_CHECK(__super::Render());

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

_int CCharacter::Get_CurrentAnimIndex()
{
	return m_pBody->Get_CurrentAnimIndex();
}

void CCharacter::Set_Animation(_uint _iNextAnimation, CModel::ANIM_STATE _eAnimState, _bool _bIsTransition, _bool _bUseAnimationPos, _uint iTargetKeyFrameIndex)
{
	m_pBody->Set_Animation(_iNextAnimation, _eAnimState, _bIsTransition, _bUseAnimationPos, iTargetKeyFrameIndex);
}

_bool CCharacter::Is_Animation_End()
{
	return m_pBody->Is_Animation_End();
}

_bool CCharacter::Is_UpperAnimation_End()
{
	return m_pBody->Is_UpperAnimation_End();
}

_bool CCharacter::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_pBody->Is_Inputable_Front(_iIndexFront);
}

_float CCharacter::Get_TrackPosition()
{
	return m_pBody->Get_TrackPosition();
}

_bool CCharacter::Compare_TrackPosition_Is_Over(_float fTrackPosition)
{
	return m_pBody->Compare_TrackPosition_Is_Over(fTrackPosition);
}
void CCharacter::Set_TrackPosition(_int iNewTrackPostion)
{
	return m_pBody->Set_TrackPosition(iNewTrackPostion);
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

	//if (Power::Absolute == m_eStrength)
	//{
	//	return Hit_Type::None;
	//}

	//Get_Damaged(iDamage);	
	//Set_InvincibleTime(fInvincibleTime);
	Add_Force(vDir, fForce);
	m_pTransformCom->Look_At_Direction(vDir * -1);

	if (m_iHp <= 0)
	{
		Hitted_Dead(eHitPower);
		//eHitType = Hit_Type::Hit_Finish;
	}
	else if (m_bTrigger == true)
	{

	}
	else //if (eHitPower >= m_eStrength)
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

void CCharacter::Add_Force(_vector In_vDir, _float In_fPower)
{
	m_pRigidBody->Add_Force(In_vDir, In_fPower);
}

void CCharacter::Look_At_Target()
{
	if (nullptr == m_pTarget || false == m_pTarget->Get_Enable())
		return;

	_fvector vTargetPos = m_pTarget->Get_Position_Vector();
	m_pTransformCom->Look_At_OnLand(vTargetPos);
}

void CCharacter::Look_At_Target_Lerp(_float fTimeDelta)
{
	if (nullptr == m_pTarget || false == m_pTarget->Get_Enable())
		return;

	_fvector vTargetPos = m_pTarget->Get_Position_Vector();
	m_pTransformCom->Look_At_Lerp(vTargetPos, fTimeDelta);
}

void CCharacter::Search_Target(const wstring& strLayerTag, const _float fSearchDistance)
{
	if (nullptr != m_pTarget)
		return;

	m_pTarget = Select_The_Nearest_Enemy(strLayerTag, fSearchDistance);
}

_float CCharacter::Target_Contained_Angle(_float4 vStandard, _float4 vTargetPos)
{
	/* ---------- 소영 추가 ---------- */
	// 함수설명 : Look 기준으로 우측에 있을경우 +사이각 , 좌측에 있을경우 - 사이각으로 값이 리턴된다. 
	/* ------------------------------- */
	_vector vLook = XMVector3Normalize(vTargetPos - m_pTransformCom->Get_Pos());

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	_float fAngle = acos(XMVectorGetX(XMVector3Dot(vStandard, vLook)));

	fAngle = XMConvertToDegrees(fAngle);
	
	_vector vJudge = XMVector3Cross(vStandard, vLook);

	_float fRotationDirection = XMVectorGetY(vJudge) < 0 ? -1.0f : 1.0f;

	return fAngle * fRotationDirection;

}

_bool CCharacter::Lerp_ToOrigin_Look(_float4 vOriginLook, _float fSpeed, _float fTimeDelta)
{
	_vector currentLook = m_pTransformCom->Get_Look();
	_vector originLook = XMLoadFloat4(&vOriginLook);

	_float angle = acos(XMVectorGetX(XMVector3Dot(currentLook, originLook)));

	if (angle < 0.01f)
		return true;

	_vector lerpedLook = XMVectorLerp(currentLook, originLook, fSpeed * fTimeDelta);

	lerpedLook = XMVector3Normalize(lerpedLook);

	m_pTransformCom->Set_Look(lerpedLook);

	return false;
}

CCharacter* CCharacter::Select_The_Nearest_Enemy(const wstring& strLayerTag, _float fMaxDistance)
{
	CCharacter* pResult = { nullptr };

	_float fMinDistance = fMaxDistance;

	_uint iCurrentLevel = m_pGameInstance->Get_NextLevel();

	list<CGameObject*>* pTargetLayer = m_pGameInstance->Get_GameObjects(iCurrentLevel, strLayerTag);

	if (nullptr == pTargetLayer)
		return nullptr;

	for (CGameObject* pTarget : *pTargetLayer) 
	{

		if (nullptr == pTarget || false == pTarget->Get_Enable())
			continue;

		CCharacter* pTargetCharacter = dynamic_cast<CCharacter*>(pTarget);

		if (nullptr == pTargetCharacter)
			continue;

		_float fDistance = Calc_Distance(pTarget);
		if (fMinDistance > fDistance) 
		{
			fMinDistance = fDistance;
			pResult = pTargetCharacter;
		}
	}

	return pResult;
}


_float CCharacter::Calc_Distance(_float3 vTargetPos)
{
	_float3 vPos = Get_Position();

	_float3 vDiff = vTargetPos - vPos;

	return sqrt(vDiff.x * vDiff.x + vDiff.y * vDiff.y + vDiff.z * vDiff.z);
}

_float CCharacter::Calc_Distance(CGameObject* pTarget)
{
	if (nullptr == pTarget || false == pTarget->Get_Enable())
		return 1000000.f;

	return Calc_Distance(pTarget->Get_Position());
}

_float CCharacter::Calc_Distance()
{
	return Calc_Distance(m_pTarget);
}

_float CCharacter::Calc_The_Nearest_Enemy_Distance(const wstring& strLayerTag)
{
	CCharacter* pCharacter = Select_The_Nearest_Enemy(strLayerTag);

	if (nullptr == pCharacter)
		return 1000000.f;

	return Calc_Distance(pCharacter);
}

void CCharacter::Move_In_Proportion_To_Enemy(_float fTimeDelta, _float fSpeedCap)
{
	if (nullptr == m_pTarget)
		return;

	_matrix _WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float fDistance = Calc_Distance();
	_float3 vPos = { 0.f, 0.f, min(fDistance * fTimeDelta, fSpeedCap) };

	_vector vResult = XMVector3TransformNormal(XMLoadFloat3(&vPos), _WorldMatrix);
	m_pTransformCom->Move_On_Navigation(vResult);
}


void CCharacter::Set_Animation_Upper(_uint _iAnimationIndex, CModel::ANIM_STATE _eAnimState, _uint iTargetKeyFrameIndex)
{
	m_pBody->Set_Animation_Upper(_iAnimationIndex, _eAnimState, iTargetKeyFrameIndex);
}

void CCharacter::Set_StiffnessRate(_float fStiffnessRate)
{
	m_pBody->Set_StiffnessRate(fStiffnessRate);
}

void CCharacter::Set_StiffnessRate_Upper(_float fStiffnessRate)
{
	m_pBody->Set_StiffnessRate_Upper(fStiffnessRate);
}

void CCharacter::Set_Weapons_Enable_False()
{
	for (CWeapon* pWeapon : m_Weapons)
	{
		pWeapon->Set_Enable(false);
	}

}

CWeapon* CCharacter::Set_Weapon_Enable(const wstring& strWeaponTag, _bool bActivate)
{
	CWeapon* pWeapon = Get_Weapon(strWeaponTag);
	if (pWeapon)
		pWeapon->Set_Enable(bActivate);

	return pWeapon;
}

CWeapon* CCharacter::Set_Weapon_Collisions_Enable(const wstring& strWeaponTag, _bool bActivate)
{
	CWeapon* pWeapon = Get_Weapon(strWeaponTag);
	if (pWeapon)
		pWeapon->Set_Enable_Collisions(bActivate);

	return pWeapon;
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

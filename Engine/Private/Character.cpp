#include "..\Public\Character.h"
#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
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

	//if (m_bIsDissolve)
	//	m_fDissolveTime += fTimeDelta;

	//if (1.3f <= m_fDissolveTime)
	//	m_bisdead = true;
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

	//if (m_fInvincibleTime > 0.f)
	//{
	//	m_fInvincibleTime -= fTimeDelta;
	//}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugRender(m_pNavigationCom);
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

void CCharacter::Free()
{
	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();

	Safe_Release(m_pBody);

	//for (CWeapon* pWeapon : m_Weapons)
	//{
	//	Safe_Release(pWeapon);
	//}
	//m_Weapons.clear();

	Safe_Release(m_pNavigationCom);

	__super::Free();
}

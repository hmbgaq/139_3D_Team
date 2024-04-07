#include "Bullet_Revolver.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "Effect_Trail.h"
#include "Mother.h"

CBullet_Revolver::CBullet_Revolver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CBullet_Revolver::CBullet_Revolver(const CBullet_Revolver& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_Revolver::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Revolver::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 60.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 4.f;

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect(m_iCurrnetLevel, LAYER_EFFECT, "Test_Skull.json", this);

	return S_OK;
}

void CBullet_Revolver::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Revolver::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);
}

void CBullet_Revolver::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	////! 유정: 트레일 테스트
	//if (nullptr != m_pTrail)
	//{
	//	m_pTrail->Tick_Trail(fTimeDelta, m_pTransformCom->Get_WorldFloat4x4());	//! 제대로 된 위치에 생성되는게 아닌 것 같다. 나중에 물어보기
	//}

}

HRESULT CBullet_Revolver::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Revolver::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CBullet_Revolver::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.f;

		_vector vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector();
		_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(vPlayerPos);
		//_vector vDir = pTarget_Character->Calc_Look_Dir(m_pTransformCom->Get_Position());
		pTarget_Character->Set_Hitted(m_fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);


		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());
		EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());

	}
	if (CData_Manager::GetInstance()->Get_Mother() != nullptr)
	{
		CData_Manager::GetInstance()->Get_Mother()->Get_Damaged(Get_Damage());
	}
	Set_Dead(true);


	//m_pEffect->Set_Dead(true);
}

void CBullet_Revolver::OnCollisionStay(CCollider* other)
{
}

void CBullet_Revolver::OnCollisionExit(CCollider* other)
{
}

HRESULT CBullet_Revolver::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.1f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

CBullet_Revolver* CBullet_Revolver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_Revolver* pInstance = new CBullet_Revolver(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_Revolver");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Revolver::Clone(void* pArg)
{
	CBullet_Revolver* pInstance = new CBullet_Revolver(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet_Revolver");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Revolver::Pool()
{
	return new CBullet_Revolver(*this);
}

void CBullet_Revolver::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		Safe_Release(m_pEffect);
}

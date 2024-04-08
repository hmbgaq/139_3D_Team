#include "Bullet_Winchester.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CBullet_Winchester::CBullet_Winchester(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CBullet_Winchester::CBullet_Winchester(const CBullet_Winchester& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_Winchester::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Winchester::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 90.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 10.f;

	EFFECT_MANAGER->Play_Effect_StaticPivot("Revolver_13_Tail_01.json", this, Get_Transform()->Get_WorldFloat4x4());

	return S_OK;
}

void CBullet_Winchester::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Winchester::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//m_pTransformCom->Go_Straight(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);

	//_float fDistance = m_pTransformCom->Get_Speed();// / 2.f;
	//Search_Target(LAYER_MONSTER, fDistance);
	//Search_Target(LAYER_BOSS, fDistance);
	//
	//Look_At_Target();
	//
	//if (m_pTarget)
	//{
	//	//_float fDiff = Calc_Distance(m_pTarget->Get_Position());
	//	//m_pTransformCom->Set_Speed(fDiff);
	//	Set_Position(m_pTarget->Get_WeaknessPos());
	//}
	//else 
	//{
	//	m_pTransformCom->Go_Straight(fTimeDelta);
	//}
	
	
}

void CBullet_Winchester::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	//! 유정: 트레일 테스트
	//if (nullptr != m_pTrail)
	//{
	//	m_pTrail->Tick_Trail(fTimeDelta, m_pTransformCom->Get_WorldFloat4x4());	//! 제대로 된 위치에 생성되는게 아닌 것 같다. 나중에 물어보기
	//}

}

HRESULT CBullet_Winchester::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Winchester::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CBullet_Winchester::OnCollisionEnter(CCollider* other)
{

	if (other->m_iLayer == (_uint)COLLISION_LAYER::WEAKNESS)
	{
		other->Get_Owner()->Set_Enable(false);
	}

	CCharacter* pTarget_Character = Get_Target_Character(other);

	//if (nullptr != pTarget_Character)// 일반 타격 
	{
		m_eHitDirection = Direction::Front;
		m_eHitPower = Power::Medium;
		m_fForce = 0.f;
		if (nullptr != pTarget_Character)
		{
			_vector vPlayerPos = CData_Manager::GetInstance()->Get_Player()->Get_Position_Vector();
			_vector vDir = pTarget_Character->Calc_Look_Dir_XZ(vPlayerPos);
			//_vector vDir = pTarget_Character->Calc_Look_Dir(m_pTransformCom->Get_Position());
			pTarget_Character->Set_Hitted(m_fDamage, vDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower);
		}
		

		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position());
		//EFFECT_MANAGER->Play_Effect("Hit_Distortion.json", m_pTransformCom->Get_Position());

	}

	Set_Dead(true);

}

void CBullet_Winchester::OnCollisionStay(CCollider* other)
{
	Set_Dead(true);
}

void CBullet_Winchester::OnCollisionExit(CCollider* other)
{
}

HRESULT CBullet_Winchester::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.4f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;



	//! 유정: 트레일 테스트
	m_pTrail = EFFECT_MANAGER->Ready_Trail(iNextLevel, LAYER_EFFECT, "Test_Trail.json", this);


	return S_OK;
}

CBullet_Winchester* CBullet_Winchester::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_Winchester* pInstance = new CBullet_Winchester(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_Winchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Winchester::Clone(void* pArg)
{
	CBullet_Winchester* pInstance = new CBullet_Winchester(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet_Winchester");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Winchester::Pool()
{
	return new CBullet_Winchester(*this);
}

void CBullet_Winchester::Free()
{
	__super::Free();

	if (nullptr != m_pTrail)
		m_pTrail->Set_Dead(TRUE);

}

#include "Bullet_Arrow_Electric.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CBullet_Arrow_Electric::CBullet_Arrow_Electric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CBullet_Arrow_Electric::CBullet_Arrow_Electric(const CBullet_Arrow_Electric& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_Arrow_Electric::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Arrow_Electric::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 30.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 0.f;
	m_fLifeTime = 3.f;

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect(m_iCurrnetLevel, LAYER_EFFECT, "Test_Skull.json", this);

	return S_OK;
}

void CBullet_Arrow_Electric::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Arrow_Electric::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta);

}

void CBullet_Arrow_Electric::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CBullet_Arrow_Electric::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Arrow_Electric::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CBullet_Arrow_Electric::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{
		//CGameObject* pDynamite = m_pGameInstance->Add_CloneObject_And_Get(m_iCurrnetLevel, LAYER_PLAYER_BULLET, L"Prototype_GameObject_Explosion_Dynamite");
		//pDynamite->Set_Position(Get_Position());



		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());
		//Set_Dead(true);
	}


	//m_pEffect->Set_Dead(true);
}

void CBullet_Arrow_Electric::OnCollisionStay(CCollider* other)
{
}

void CBullet_Arrow_Electric::OnCollisionExit(CCollider* other)
{
}

HRESULT CBullet_Arrow_Electric::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Model_Bullet_Arrow_Electric"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 1.0f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

CBullet_Arrow_Electric* CBullet_Arrow_Electric::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_Arrow_Electric* pInstance = new CBullet_Arrow_Electric(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_Arrow_Electric");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Arrow_Electric::Clone(void* pArg)
{
	CBullet_Arrow_Electric* pInstance = new CBullet_Arrow_Electric(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet_Arrow_Electric");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Arrow_Electric::Pool()
{
	return new CBullet_Arrow_Electric(*this);
}

void CBullet_Arrow_Electric::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		m_pEffect->Set_Dead(true);
}

#include "Bullet_Teleport.h"
#include "GameInstance.h"
#include "Character.h"
#include "Data_Manager.h"
#include "Effect.h"
#include "Effect_Manager.h"
#include "Player.h"

#include "Effect_Manager.h"
#include "Effect_Trail.h"


CBullet_Teleport::CBullet_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CProjectile(pDevice, pContext, strPrototypeTag)
{
}

CBullet_Teleport::CBullet_Teleport(const CBullet_Teleport& rhs)
	: CProjectile(rhs)
{
}

HRESULT CBullet_Teleport::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Teleport::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		GameObjectDesc = {};

	GameObjectDesc.fSpeedPerSec = 30.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	//m_pTransformCom->Look_At(m_vPlayerPos);

	m_fDamage = 0.f;
	m_fLifeTime = 3.f;

	m_vLastPotision = CData_Manager::GetInstance()->Get_Player()->Get_Position();

	// 이펙트 생성
	//m_pEffect = EFFECT_MANAGER->Create_Effect(m_iCurrnetLevel, LAYER_EFFECT, "Test_Skull.json", this);

	

	return S_OK;
}

void CBullet_Teleport::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBullet_Teleport::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_int iCheckIndex = m_pNavigationCom->Get_SelectRangeCellIndex(this);
	m_pNavigationCom->Set_CurrentIndex(iCheckIndex);

	_bool IsMove = m_pNavigationCom->isMove(Get_Position_Vector());
	if (true == IsMove)
	{
		m_vLastPotision = Get_Position();
		_float fHeight = m_pNavigationCom->Compute_Height(m_vLastPotision, nullptr);
		m_vLastPotision.y = fHeight;
	}


	if (m_pGameInstance->Key_Up(DIK_T))
	{
		CPlayer* pPlayer = CData_Manager::GetInstance()->Get_Player();
		pPlayer->Set_InitPosition(m_vLastPotision);
		Set_Dead(true);
	}

	m_pTransformCom->Go_Straight(fTimeDelta);

}

void CBullet_Teleport::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CBullet_Teleport::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBullet_Teleport::Render_Shadow()
{
	//이거 필요한거? 딱히 필요는 없어 보임 
	return S_OK;
}

void CBullet_Teleport::OnCollisionEnter(CCollider* other)
{

	CCharacter* pTarget_Character = Get_Target_Character(other);

	if (nullptr != pTarget_Character)// 일반 타격 
	{




		//CEffect* pEffect = EFFECT_MANAGER->Create_Effect("Hit/", "Hit_Distortion.json", m_pTransformCom->Get_Position(), TRUE, m_pGameInstance->Get_Player()->Get_Position());
		//Set_Dead(true);
	}

}

void CBullet_Teleport::OnCollisionStay(CCollider* other)
{
}

void CBullet_Teleport::OnCollisionExit(CCollider* other)
{
}

HRESULT CBullet_Teleport::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Shader_Model"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.5f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pCollider), &BoundingDesc)))
		return E_FAIL;


	CNavigation::NAVI_DESC		NaviDesc = {};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;





	//! 유정: 트레일 추가 테스트
	//m_pTrail = EFFECT_MANAGER->Ready_Trail("Monster_Bullet_Trail.json", this);


	return S_OK;
}

HRESULT CBullet_Teleport::Set_InitPosition(const _float3& vPos)
{
	Set_Position(vPos);

	NULL_CHECK_RETURN(m_pNavigationCom, E_FAIL);

	_int iCheckIndex = m_pNavigationCom->Get_SelectRangeCellIndex(this);
	if (iCheckIndex == -1)
		Set_Dead(true);

	m_pNavigationCom->Set_CurrentIndex(iCheckIndex);

	return S_OK;
}


CBullet_Teleport* CBullet_Teleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CBullet_Teleport* pInstance = new CBullet_Teleport(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_Teleport");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Teleport::Clone(void* pArg)
{
	CBullet_Teleport* pInstance = new CBullet_Teleport(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBullet_Teleport");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Teleport::Pool()
{
	return new CBullet_Teleport(*this);
}

void CBullet_Teleport::Free()
{
	__super::Free();

	if (nullptr != m_pEffect)
		m_pEffect->Set_Dead(true);

	if (m_pNavigationCom)
		Safe_Release(m_pNavigationCom);
}

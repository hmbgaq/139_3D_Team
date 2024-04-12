#include "stdafx.h"
#include "Player_Weapon_Punch.h"
#include "GameInstance.h"
#include "Character_Client.h"
#include "Player.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "Effect_Manager.h"
#include "SMath.h"

CPlayer_Weapon_Punch::CPlayer_Weapon_Punch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
	:CWeapon_Player(pDevice, pContext, strPrototypeTag)
{

}

CPlayer_Weapon_Punch::CPlayer_Weapon_Punch(const CPlayer_Weapon_Punch& rhs)
	: CWeapon_Player(rhs)
{
}

HRESULT CPlayer_Weapon_Punch::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Punch::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	m_bIsMelee = true;

	return S_OK;
}

void CPlayer_Weapon_Punch::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CPlayer_Weapon_Punch::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayer_Weapon_Punch::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Weapon_Punch::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon_Punch::Ready_Components()
{
	_uint iNextLevel = m_pGameInstance->Get_NextLevel();

	//if (FAILED(__super::Ready_Components()))
	//	return E_FAIL;

	m_iColliderSize = 1;
	m_pColliders.resize(m_iColliderSize);

	///* For.Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.iLayer = ECast(COLLISION_LAYER::PLAYER_ATTACK);
	BoundingDesc.fRadius = { 0.6f };
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(iNextLevel, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliders[0]), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon_Punch::Attack(CCollider* other)
{
	CCharacter* pTarget_Character = Get_Target_Character(other);
	if (nullptr != pTarget_Character)
	{
		_vector vTargetPos = pTarget_Character->Get_Position_Vector();
		_vector vHitDir = Get_Object_Owner()->Calc_Look_Dir_XZ(vTargetPos) * -1;

		_float fDamage = m_fDamage + CData_Manager::GetInstance()->Get_Additional_MeleeDamage();

		pTarget_Character->Set_Hitted(fDamage, vHitDir, m_fForce, 1.f, m_eHitDirection, m_eHitPower, m_bIsMelee, m_bKnockUp);
		//pTarget_Character->Set_Hitted(0, Get_Object_Owner()->Calc_Look_Dir(vTargetPos) * -1, 0.5f, 1.f, Direction::Front, Power::Light);


		//CCharacter_Client* pOwnerCharacter = dynamic_cast<CCharacter_Client*>(Get_Object_Owner());
		//if (pOwnerCharacter)
		//{
		//	pOwnerCharacter->Create_Hitting_Effect(Get_WorldPosition(), m_eHitPower);
		//}

		{
			_float3 vPos = Get_WorldPosition();
			_float3 vTargetPos = pTarget_Character->Get_Position();
			vTargetPos.y = vPos.y;
			EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", vPos, true, vTargetPos);

			//EFFECT_MANAGER->Play_Effect("Hit/", "Hit_Distortion.json", Get_WorldPosition(), true, pTarget_Character->Get_Position());
			CData_Manager::GetInstance()->Apply_Shake_And_Blur(m_eHitPower);

			Play_Hit_Sound(m_eHitPower);


		}

		Set_Enable_Collisions(false);
	}
}


void CPlayer_Weapon_Punch::Play_Hit_Sound_Normal()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"Player_Punch_Regular_Impact_01.wav";
		break;
	case 1:
		strFileName = L"Player_Punch_Regular_Impact_02.wav";
		break;
	case 2:
		strFileName = L"Player_Punch_Regular_Impact_03.wav";
		break;
	case 3:
		strFileName = L"Player_Punch_Regular_Impact_04.wav";
		break;
	case 4:
		strFileName = L"Player_Punch_Regular_Impact_05.wav";
		break;
	case 5:
		strFileName = L"Player_Punch_Regular_Impact_06.wav";
		break;
	default:
		strFileName = L"Player_Punch_Regular_Impact_01.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", strFileName, CHANNELID::SOUND_PLAYER_IMPACT, 10.f);
}

void CPlayer_Weapon_Punch::Play_Hit_Sound_Heavy()
{
	wstring strFileName = L"";

	_uint iRand = SMath::Random(0, 6);
	switch (iRand)
	{
	case 0:
		strFileName = L"Player_Punch_Heavy_Impact_01.wav";
		break;
	case 1:
		strFileName = L"Player_Punch_Heavy_Impact_02.wav";
		break;
	case 2:
		strFileName = L"Player_Punch_Heavy_Impact_03.wav";
		break;
	case 3:
		strFileName = L"Player_Punch_Heavy_Impact_04.wav";
		break;
	case 4:
		strFileName = L"Player_Punch_Heavy_Impact_05.wav";
		break;
	case 5:
		strFileName = L"Player_Punch_Heavy_Impact_06.wav";
		break;
	default:
		strFileName = L"Player_Punch_Heavy_Impact_01.wav";
		break;
	}

	m_pGameInstance->Play_Sound(L"PLAYER_IMPACT", strFileName, CHANNELID::SOUND_PLAYER_IMPACT, 10.f);
}

void CPlayer_Weapon_Punch::OnCollisionEnter(CCollider* other)
{
	Attack(other);
}

void CPlayer_Weapon_Punch::OnCollisionStay(CCollider* other)
{
	Attack(other);
}

void CPlayer_Weapon_Punch::OnCollisionExit(CCollider* other)
{
	_int a = 0;
}

CPlayer_Weapon_Punch* CPlayer_Weapon_Punch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag)
{
	CPlayer_Weapon_Punch* pInstance = new CPlayer_Weapon_Punch(pDevice, pContext, strPrototypeTag);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Weapon_Punch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Punch::Clone(void* pArg)
{
	CPlayer_Weapon_Punch* pInstance = new CPlayer_Weapon_Punch(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Weapon_Punch");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Weapon_Punch::Pool()
{
	return new CPlayer_Weapon_Punch(*this);
}

void CPlayer_Weapon_Punch::Free()
{
	__super::Free();
}

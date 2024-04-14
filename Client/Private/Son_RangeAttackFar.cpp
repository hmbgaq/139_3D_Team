#include "Son_RangeAttackFar.h"
#include "Son_Idle.h"
#include "GameInstance.h"
#include "Bone.h"

void CSon_RangeAttackFar::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_MonsterAttackState(true);

	
}

CState<CSon>* CSon_RangeAttackFar::Update(CSon* pActor, _float fTimeDelta)
{

	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(26))
	{
		_float4x4 BoneMatrix = pActor->Get_Body()->Get_BonePtr("Bone020")->Get_CombinedTransformationMatrix();
		_float4x4 SonMatrix = pActor->Get_Transform()->Get_WorldMatrix();
		

		BoneMatrix = BoneMatrix * SonMatrix;
		CGameObject* pProjectile = { nullptr };

		_int iRandomSound = m_pGameInstance->Random_Int(1, 5);
		wstring strSoundKey = L"";

		switch (iRandomSound)
		{
		case 1:
			strSoundKey = L"grand_parasiter_vo_spit001.wav";
			break;
		case 2:
			strSoundKey = L"grand_parasiter_vo_spit002.wav";
			break;
		case 3:
			strSoundKey = L"grand_parasiter_vo_spit003.wav";
			break;
		case 4:
			strSoundKey = L"grand_parasiter_vo_spit004.wav";
			break;
		case 5:
			strSoundKey = L"grand_parasiter_vo_spit005.wav";
			break;
		}

		m_pGameInstance->Play_Sound(L"SON_ATTACK", strSoundKey, SOUND_ENEMY_WHOOSH, 7.f);

		
		pProjectile = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_BossProjectile", L"Prototype_GameObject_Son_Projectile");
		pProjectile->Set_Object_Owner(pActor);



		//pProjectile->Set_WorldMatrix(BoneMatrix);
		//_float3 Temp = _float3(BoneMatrix._41, BoneMatrix._42, BoneMatrix._43);
		//pProjectile->Set_Position(Temp);
		pActor->m_bLookAt = false;
		m_bFlags[0] = true;
	}

	if (false == m_bFlags[1])
	{
		m_bFlags[1] = pActor->Is_Inputable_Front(39);
		if (true == m_bFlags[1])
		{
			m_pGameInstance->Play_Sound(L"SON_BODYFALL", L"boohag_mvm_bodyfall006.wav", SOUND_ENEMY_BODYMOVE2, 7.f);
		}
	}

	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_RangeAttackFar::Release(CSon* pActor)
{
	__super::Release(pActor);
	pActor->m_bLookAt = true;
}

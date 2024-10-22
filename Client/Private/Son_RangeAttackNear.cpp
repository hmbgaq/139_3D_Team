#include "Son_RangeAttackNear.h"
#include "Son_Idle.h"
#include "GameInstance.h"
#include "Bone.h"

void CSon_RangeAttackNear::Initialize(CSon* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	pActor->Set_MonsterAttackState(true);
}

CState<CSon>* CSon_RangeAttackNear::Update(CSon* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(26))
	{
		_float4x4 BoneMatrix = pActor->Get_Body()->Get_BonePtr("Bone020")->Get_CombinedTransformationMatrix();
		_float4x4 SonMatrix = pActor->Get_Transform()->Get_WorldMatrix();

		BoneMatrix = BoneMatrix * SonMatrix;
		CGameObject* pProjectile = { nullptr };
		pProjectile = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Boss", L"Prototype_GameObject_Son_Projectile");
		pProjectile->Set_WorldMatrix(BoneMatrix);
		pActor->m_bLookAt = false;
		m_bFlags[0] = true;
	}

	if (pActor->Is_Animation_End())
	{
		return new CSon_Idle;
	}

	return nullptr;
}

void CSon_RangeAttackNear::Release(CSon* pActor)
{
	__super::Release(pActor);
	pActor->m_bLookAt = true;
}

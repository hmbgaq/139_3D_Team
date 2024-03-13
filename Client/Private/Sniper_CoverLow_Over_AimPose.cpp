#include "stdafx.h"
#include "GameInstance.h"
#include "Sniper_CoverLow_Over_Stop.h"
#include "Sniper_CoverLow_Over_AimPose.h"
#include "Bullet_Bandit_Sniper.h"

void CSniper_CoverLow_Over_AimPose::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverLow_Over_AimPose::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	/* 에임중에는 계속쳐다보는에 lookat처럼 처다봄 어.. 맞는듯 비슷하네 */

	fTimeAcc += fTimeDelta;
	pActor->Look_At_Target();

	if (fTimeAcc > 2.5f)
	{
		/* 1초 전 위치로 공격할거임 */

		if (!bReady)
		{
			_float3 fDir = pActor->Get_Target()->Get_Position() - pActor->Get_Position();
			_vector vDir = DirectX::XMVectorSet(fDir.x, fDir.y, fDir.z, 1.f);
			vBulletDir = DirectX::XMVector4Normalize(vDir);
			bReady = true;
		}

		if (fTimeAcc >= 3.5f)
		{
			cout << "총알발사 " << endl;
			fTimeAcc = 0.f;
			
			/* 위에서 만든 방향으로 총알 만들기 */
			CBullet_Bandit_Sniper::BULLET_DESC desc = {};
			desc.fBullet_InitPos = pActor->Get_BonePos();
			desc.vBullet_MoveDir = vBulletDir;

			m_pGameInstance->Add_CloneObject_And_Get(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Bullet_Bandit_Sniper"), &desc);
			
			return new CSniper_CoverLow_Over_Stop();
		}
	}

	return nullptr;
}

void CSniper_CoverLow_Over_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

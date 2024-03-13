#include "stdafx.h"
#include "GameInstance.h"
#include "Sniper_CoverLow_Over_Stop.h"
#include "Sniper_CoverLow_Over_AimPose.h"
#include "Bullet_Bandit_Sniper.h"
#include "Weapon_Bandit_Sniper.h"

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

	if (fTimeAcc > 3.5f)
	{
		fTimeAcc = 0.f;
		pActor->Sniping_Target(pActor->Get_Target()->Get_Transform()->Get_Pos());

		return new CSniper_CoverLow_Over_Stop();
	}
	//if (fTimeAcc > 3.f)
	//{
	//	/* 1초 전 위치로 공격할거임 */
	//	if (!bReady)
	//	{
	//		CWeapon_Bandit_Sniper* pWeapon = dynamic_cast<CWeapon_Bandit_Sniper*>(pActor->Get_Weapon(TEXT("Weapon_Sniper")));
	//		if (nullptr == pWeapon)
	//			cout << "CSniper_CoverLow_Over_AimPose" << endl;
	//		
	//		_float4 TargetPos = {};
	//		XMStoreFloat4(&TargetPos, pActor->Get_Target()->Get_Position_Vector());

	//		pWeapon->Set_TargetPos(TargetPos);

	//		pActor->
	//		//_float4 Result = {};
	//		//_float3 fRay_Dir = {};
	//		//_float4 vRay_Pos = {};
	//		//_float3 Weapon_Pos = pActor->Get_Weapon(TEXT("Weapon_Sniper"))->Get_Weapon_Pos();
	//		//XMStoreFloat3(&fRay_Dir, (XMVector3Normalize(XMLoadFloat3(&(pActor->Get_Target()->Get_Position() - Weapon_Pos)))));
	//		//
	//		//RAY ray = {};
	//		//ray.vDirection = fRay_Dir;
	//		//ray.vPosition = vRay_Pos;
	//		//ray.fLength = 100.f;
	//		//pActor->Get_Target()->Get_Body()->Get_Model()->IsModelPicking(ray, pActor->Get_Transform()->Get_WorldMatrix(), &Result);
	//		
	//		bReady = true;
	//	}

	//	if (fTimeAcc >= 3.5f)
	//	{
	//		cout << "총알발사 " << endl;

	//		fTimeAcc = 0.f; 
	//		pActor->Sniping_Target();
	//		//CWeapon_Bandit_Sniper* pWeapon = dynamic_cast<CWeapon_Bandit_Sniper*>(pActor->Get_Weapon(TEXT("Weapon_Sniper")));
	//		//if (nullptr == pWeapon)
	//		//	cout << "CSniper_CoverLow_Over_AimPose" << endl;
	//		//
	//		//pWeapon->Sniping();
	//		
	//		return new CSniper_CoverLow_Over_Stop();
	//	}
	//}

	return nullptr;
}

void CSniper_CoverLow_Over_AimPose::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

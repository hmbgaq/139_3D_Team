#include "Mother_ShakeTreeStart.h"
#include "Mother_ShakeTreeLoop.h"
#include "Data_Manager.h"
#include "MasterCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "SMath.h"

void CMother_ShakeTreeStart::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CMother>* CMother_ShakeTreeStart::Update(CMother* pActor, _float fTimeDelta)
{
	if (m_bFlags[0] == false && pActor->Is_Inputable_Front(29))
	{
		CSpringCamera* pSpringCam = CData_Manager::GetInstance()->Get_MasterCamera()->Get_SpringCamera();
		pSpringCam->Set_ShakeCameraTime(0.3f);
		pSpringCam->Set_ShakeCameraMinMax(_float2(0.f, 0.2f));
		pActor->Apply_Shake_And_Blur(Power::Medium);

		CGameObject* pObjcet = { nullptr };
		for (int i = 0; i <2; ++i)
		{
			for (int j = 0; j <2; ++j)
			{
				_float fRandom = SMath::fRandom(-5.f, 5.f);
				_float fRandom2 = SMath::fRandom(-5.f, 5.f);
				_float fRandom3 = SMath::fRandom(-5.f, 5.f);
				_float fRandom4 = SMath::fRandom(-5.f, 5.f);
				//++방향 1사분면
				pObjcet = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Monster", L"Prototype_GameObject_MotherShakeTreeProjectile");
				pObjcet->Set_Position(CData_Manager::GetInstance()->Get_Player()->Get_Position() + _float3((j + 1) * 10.f, (20.f + fRandom), (i + 1) * 10.f));
				//-+방향 2사분면
				pObjcet = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Monster", L"Prototype_GameObject_MotherShakeTreeProjectile");
				pObjcet->Set_Position(CData_Manager::GetInstance()->Get_Player()->Get_Position() + _float3(-j * 10.f, (20.f + fRandom2), i * 10.f));
				//--방향 3사분면
				pObjcet = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Monster", L"Prototype_GameObject_MotherShakeTreeProjectile");
				pObjcet->Set_Position(CData_Manager::GetInstance()->Get_Player()->Get_Position() + _float3((-j - 1) * 10.f, (20.f + fRandom3), (-i - 1) * 10.f));
				//+-방향 4사분면
				pObjcet = m_pGameInstance->Add_CloneObject_And_Get(LEVEL_SNOWMOUNTAINBOSS, L"Layer_Monster", L"Prototype_GameObject_MotherShakeTreeProjectile");
				pObjcet->Set_Position(CData_Manager::GetInstance()->Get_Player()->Get_Position() + _float3(j * 10.f, (20.f + fRandom4), -i * 10.f));
				
			}
			
		}
	
		//이떄 카메라 쉐이킹 하면서 맵에 전체 공격 패턴 추가하면 될 거같음 
		m_bFlags[0] = true;
	}

	if (pActor->Is_Animation_End())
	{
		return new CMother_ShakeTreeLoop;
	}


	return nullptr;
}

void CMother_ShakeTreeStart::Release(CMother* pActor)
{
	__super::Release(pActor);
}

#include "..\Public\Tank_GroundWaveNoShield_01.h"

void CTank_GroundWaveNoShield_01::Initialize(CTank* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Play_Sound_Voice_Attack();

}

CState<CTank>* CTank_GroundWaveNoShield_01::Update(CTank* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		pActor->Look_At_Target_Lerp(fTimeDelta);

		m_bFlags[0] = pActor->Is_Inputable_Front(45);
		if (true == m_bFlags[0])
		{
			pActor->Create_GroundWave();
		}
	}
	else
	{
		pActor->Look_At_Target_Lerp(fTimeDelta);
	}

	if (pActor->Is_Animation_End())
	{
		return Idle(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CTank_GroundWaveNoShield_01::Release(CTank* pActor)
{
	__super::Release(pActor);
}

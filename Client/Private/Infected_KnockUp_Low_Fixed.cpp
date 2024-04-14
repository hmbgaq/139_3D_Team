#include "..\Public\Infected_KnockUp_Low_Fixed.h"

void CInfected_KnockUp_Low_Fixed::Initialize(CInfected* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_KnockUp(true);

	_int iCurrentKeyFrameIndex = pActor->Get_CurrentKeyFrames();
	_int iTargetKeyFrameIndex = pActor->Get_CurrentAnimIndex() != g_iAnimIndex ? 0 : max(iCurrentKeyFrameIndex - 1, 8); 

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true, true, iTargetKeyFrameIndex);
	

	pActor->Set_UseGravity(false);
}

CState<CInfected>* CInfected_KnockUp_Low_Fixed::Update(CInfected* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		pActor->Set_KnockUp(false);
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CInfected_KnockUp_Low_Fixed::Release(CInfected* pActor)
{
	__super::Release(pActor);
	
	pActor->Set_UseGravity(true);
}

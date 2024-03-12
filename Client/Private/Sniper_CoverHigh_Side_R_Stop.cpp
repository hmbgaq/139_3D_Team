#include "..\Public\Sniper_CoverHigh_Side_R_Stop.h"

void CSniper_CoverHigh_Side_R_Stop::Initialize(CBandit_Sniper* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
}

CState<CBandit_Sniper>* CSniper_CoverHigh_Side_R_Stop::Update(CBandit_Sniper* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End()) /* 리스폰 하는 애니메이션이 끝난다면 */
	{
		cout << "Side Stop end  " << endl;
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}

	return nullptr;
}

void CSniper_CoverHigh_Side_R_Stop::Release(CBandit_Sniper* pActor)
{
	__super::Release(pActor);
}

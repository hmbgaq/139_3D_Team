#include "..\Public\Player_ZipLine_Start_to_Loop.h"
#include "Player_ZipLine_Loop.h"

void CPlayer_ZipLine_Start_to_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

}

CState<CPlayer>* CPlayer_ZipLine_Start_to_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
		if (true == m_bFlags[0])
		{
			return new CPlayer_ZipLine_Loop();
		}
		
	}
	//else
	//{
	//	return new CPlayer_ZipLine_Start_to_Loop();
	//}

	return nullptr;

	//return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CPlayer_ZipLine_Start_to_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

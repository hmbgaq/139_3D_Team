#include "..\Public\Player_ZipLine_Start.h"
#include "Player_ZipLine_Start_to_Loop.h"
#include "Environment_Interact.h"
#include "Transform.h"

void CPlayer_ZipLine_Start::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Interection(true);

	pActor->Get_Transform()->Set_Look(pActor->Get_InteractObject()->Get_Look());
	//pActor->Set_Ladder_Count(6);

}

CState<CPlayer>* CPlayer_ZipLine_Start::Update(CPlayer* pActor, _float fTimeDelta)
{
	__super::Update(pActor, fTimeDelta);

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Animation_End();
	}
	else
	{
		return new CPlayer_ZipLine_Start_to_Loop();
	}

	return nullptr;

}

void CPlayer_ZipLine_Start::Release(CPlayer* pActor)
{
	__super::Release(pActor);
}

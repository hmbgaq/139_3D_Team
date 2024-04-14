#include "GameInstance.h"
#include "Player_ZipLine_Stop.h"

void CPlayer_ZipLine_Stop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);

	pActor->Set_Interection(false);
}

CState<CPlayer>* CPlayer_ZipLine_Stop::Update(CPlayer* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{
		m_pGameInstance->Get_Renderer()->Set_Chroma_Active(false);
		return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
	}
	
	return nullptr;
}

void CPlayer_ZipLine_Stop::Release(CPlayer* pActor)
{
	m_pGameInstance->Stop_Sound(SOUND_INTERACTBGM);
	__super::Release(pActor);
}

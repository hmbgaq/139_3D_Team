#include "Mother_Idle.h"
#include "Body_Mother.h"

void CMother_Idle::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true, false);
	//pActor->Get_Weapon(L"Weapon_hand_R")->Set_Synced(false);
	//pActor->m_bLookAt = true;
	//pActor->m_bTurn = true;

	//CBody_Mother* pBody = dynamic_cast<CBody_Mother*>(pActor->Get_Body());
	//pBody->Set_RenderState(CBody_Mother::RENDER_STATE::ORIGIN);

}

CState<CMother>* CMother_Idle::Update(CMother* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CMother_Idle::Release(CMother* pActor)
{
	__super::Release(pActor);
}

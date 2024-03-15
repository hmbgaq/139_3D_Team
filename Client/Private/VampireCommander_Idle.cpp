#include "VampireCommander_Idle.h"
#include "Body_VampireCommander.h"

void CVampireCommander_Idle::Initialize(CVampireCommander* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);
	pActor->Get_Weapon(L"Weapon_hand_R")->Set_Synced(false);
	pActor->m_bLookAt = true;
	pActor->m_bTurn = true;

	//CBody_VampireCommander* pBody = dynamic_cast<CBody_VampireCommander*>(pActor->Get_Body());
	//pBody->Set_RenderState(CBody_VampireCommander::RENDER_STATE::ORIGIN);

}

CState<CVampireCommander>* CVampireCommander_Idle::Update(CVampireCommander* pActor, _float fTimeDelta)
{
	return __super::Update_State(pActor, fTimeDelta, g_iAnimIndex);
}

void CVampireCommander_Idle::Release(CVampireCommander* pActor)
{
	__super::Release(pActor);
}

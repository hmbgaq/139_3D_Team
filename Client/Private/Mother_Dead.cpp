#include "Mother_Dead.h"
#include "Data_Manager.h"
#include "Effect_Manager.h"

void CMother_Dead::Initialize(CMother* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_NORMAL, true);
	m_pEffect = EFFECT_MANAGER->Play_Effect("Parasiter/", "Monster_Blood4.json", pActor, true, "Jaws_Center");

}

CState<CMother>* CMother_Dead::Update(CMother* pActor, _float fTimeDelta)
{
	if (pActor->Is_Animation_End())
	{	//죽여 버리기 여기서
		EFFECT_MANAGER->Return_ToPool(m_pEffect);
		pActor->Get_Body()->Set_RenderPass(ECast(MONSTER_SHADER::COMMON_DISSOLVE));
		pActor->m_bDeadState = true;
	}

	return nullptr;
}

void CMother_Dead::Release(CMother* pActor)
{
	__super::Release(pActor);
	CData_Manager::GetInstance()->Add_CurEXP(PARASITER_EXP);
}

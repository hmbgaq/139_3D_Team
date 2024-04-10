#include "..\Public\Player_ZipLine_Loop.h"
#include "Player_ZipLine_Stop.h"
#include "Environment_Interact.h"
#include "Transform.h"

void CPlayer_ZipLine_Loop::Initialize(CPlayer* pActor)
{
	__super::Initialize(pActor);

	pActor->Set_Animation(g_iAnimIndex, CModel::ANIM_STATE_LOOP, true);

	pActor->Set_Rotate_In_CameraDir(false);

	ZipLineMatrix = pActor->Get_InteractObject()->Get_ZipLineBoneMatrix();

	pActor->Set_RootMove_End(true);
	//pActor->Set_roo
	//TODOAdd_RootBone_ForTarget
}

CState<CPlayer>* CPlayer_ZipLine_Loop::Update(CPlayer* pActor, _float fTimeDelta)
{
	//__super::Update(pActor, fTimeDelta);

	

	if (false == m_bFlags[0])
	{
		m_bFlags[0] = pActor->Is_Inputable_Front(27);
	}
	else
	{
		pActor->Remove_Ladder_Count();

		if (false == pActor->Is_Exist_Ladder_Count())
		{
			return new CPlayer_ZipLine_Stop();
		}
		else
		{

			return new CPlayer_ZipLine_Loop();
		}
	}

	CTransform* pActorTransform = pActor->Get_Transform();
	pActorTransform->Add_RootBone_ForTarget(pActor->Get_AddRootMotion(), pActor->Get_Navigation(), pActor->Get_InteractObject()->Get_Transform());

	return nullptr;

}

void CPlayer_ZipLine_Loop::Release(CPlayer* pActor)
{
	__super::Release(pActor);
	pActor->Set_RootMove_End(false);
}

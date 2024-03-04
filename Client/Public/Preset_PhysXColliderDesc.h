#pragma once

//#include "PhysXCollider.h"
#include "Engine_Defines.h"
#include "PhysXCollider.h"

BEGIN(Engine)
class CTransform;
END

namespace Preset
{
	namespace PhysXColliderDesc
	{
		void GroundSetting(CPhysXCollider::PHYSXCOLLIDERDESC & Out_Desc, const _bool bTrigger);
		void DynamicPropSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, CTransform* pTransform);
		void DynamicPieceSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc, CTransform* pTransform);
		void ConvexStaticPropSetting(CPhysXCollider::PHYSXCOLLIDERDESC& Out_Desc,CTransform* pTransform);

	}
}
#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)
class CTransform;
END

namespace Preset
{
	namespace PhysXControllerDesc
	{
		PxCapsuleControllerDesc PlayerSetting(CTransform* pTransform);
		PxCapsuleControllerDesc CameraSetting(CTransform* pTransform);
		PxCapsuleControllerDesc BossBatSetting(CTransform* pTransform);
	}
}
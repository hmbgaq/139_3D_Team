#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CollisionSimulationEventCallBack :
    public PxSimulationEventCallback
{
public:
    CollisionSimulationEventCallBack();
    virtual ~CollisionSimulationEventCallBack() = default;

public:
    // PxSimulationEventCallback을(를) 통해 상속됨
    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
    virtual void onWake(PxActor** actors, PxU32 count) override;
    virtual void onSleep(PxActor** actors, PxU32 count) override;
    virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;

private:
    class CGameInstance m_pGameInstance = { nullptr };

public:
    void Release();
};

END
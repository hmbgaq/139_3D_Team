#pragma once
#include "Base.h"

BEGIN(Engine)

class CModel;
class CTransform;

class ENGINE_DLL CPhysXManager : public CBase
{
public:
	typedef struct BranchDesc
	{
		vector<_uint> Bones;
		vector<PxRigidDynamic*> Frames;
		vector<_matrix> RelativeMatrix;
	}BRANCHDESC;


	typedef struct PhysxPlayerDesc
	{
		CModel*				pPlayerModel = { nullptr };
		CTransform*			pPlayerTransform = { nullptr };
		PxRigidDynamic*		pPlayerActor = { nullptr };
		vector<BRANCHDESC>	Branches;
	}PLAYERDESC;

private:
	CPhysXManager();
	virtual ~CPhysXManager() = default;

public:
	HRESULT Initialize();

	void	Late_Tick(_float fTimeDelta);
	//void	Update_Branches();
	//void	Set_BranchesToBone();

	//void	Add_Player(CGameObject* pPlayer);
	//void	Add_BoneBranch(CGameObject* pPlayer, vector<_uint>& Bones);

	void	Reset();


private:
	PxFoundation*					m_PxFoundation = { nullptr };
	PxDefaultAllocator				m_PxAllocator;
	PxDefaultErrorCallback			m_PXErrorCallback;
	PxPhysics*						m_PhysX = { nullptr };
	PxDefaultCpuDispatcher*			m_PxDispatcher = { nullptr };
	PxScene*						m_PxScene = { nullptr };

	vector<PLAYERDESC>				m_PlayerInfos;


public:
	static CPhysXManager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END


//PxPvd* m_Pvd = nullptr;
//std::string							m_pvdIPAddress = "127.0.0.1";
//int									m_pvdPortNumber = 5425;
//_uint								m_pvdTimeOutSeconds = 10;

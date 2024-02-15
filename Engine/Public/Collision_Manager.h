#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollider;

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize(const _uint& In_iNumLayer);
	void	Tick(const _float& fTimeDelta);

private:
	void	Add_Collision(const _uint& In_iLayer, CCollider* _pCollider);
	void	Check_Group(const _uint& In_iLeftLayer, const _uint& In_iRightLayer);
	void	Update_ReservedColliderList();
	void	Remove_DeadCollision();
	void	Update_CollisionGroup(const _uint& In_iLeftLayer, const _uint& In_iRightLayer);
	void	End_CollisionCheck();


private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	vector<list<CCollider*>>	m_ColliderList;
	vector<list<CCollider*>>	m_ReservedColliderList; 

	vector<_uint>				m_arrCheck;
	_uint						m_iNumLayer = { 0 };

public:
	static CCollision_Manager* Create(const _uint& In_iNumLayer);
	virtual void Free() override;
};

END


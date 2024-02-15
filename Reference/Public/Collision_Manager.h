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
	void	Check_Group(const _uint& In_iLeftLayer, const _uint& In_iRightLayer);
	void	Update_CollisionGroup(const _uint& In_iLeftLayer, const _uint& In_iRightLayer);
	void	End_CollisionCheck();


private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	vector<list<CCollider*>>	m_ColliderList;
	vector<_uint>				m_arrCheck;

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END


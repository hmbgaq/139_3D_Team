#pragma once
#include "Component.h"


BEGIN(Engine)

class CTransform;


class ENGINE_DLL CRigidBody : public CComponent
{
	struct FORCE
	{
		_float3	vDir;
		_float	fPower;
	};

	//enum FORCE_MODE
	//{
	//	MODE_FORCE = 0,
	//	MODE_IMPURSE,
	//	FORCE_MODE_END
	//};


private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	_float Get_fDrag() { return m_fDrag; }
	void Set_fDrag(_float _fDrag) { m_fDrag = _fDrag; }

	_float Get_fMass() { return m_fMass; }
	void Set_fMass(_float _fMass) { m_fMass = _fMass; }

	void Set_Transform(CTransform* pOwnerTransform);

	_bool Is_Use_Gravity() { return m_bUseGravity; };
	void Set_UseGravity(_bool _bUseGravity) { m_bUseGravity = _bUseGravity; };


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Add_Force(_vector In_vDir, _float In_fPower);


private:
	list<FORCE> m_Forces;
	CTransform* m_pOwnerTransform = { nullptr };
	_bool		m_bUseGravity = { true };

	_float m_fDrag = { 4.5f };
	_float m_fMass = { 0.f };



public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once
#include "GameObject.h"

BEGIN(Engine)


class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObjectDesc
	{
		class CTransform* m_pParentTransform = { nullptr };

	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_float4x4	Get_WorldMatrix() { return m_WorldMatrix; }
	void		Set_WorldMatrix(_float4x4 _WorldMatrix) { m_WorldMatrix = _WorldMatrix; }

	_float3		Get_WorldPosition();


protected:
	class CTransform* m_pParentTransform = { nullptr };
	_float4x4	m_WorldMatrix = {};

public:
	virtual void Free() override;

};

END
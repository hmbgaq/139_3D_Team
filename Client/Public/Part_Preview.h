#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CPart_Preview final : public CGameObject
{
public:
	typedef struct tagPartPreviewDesc : public CGameObject::GAMEOBJECT_DESC
	{
		class CBone*			pSocketBone			= { nullptr };
		class CTransform*		pParentTransform	= { nullptr };
		class CGameObject*		pOwner				= { nullptr };
	}PART_PREVIEW_DESC;

private:
	CPart_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CPart_Preview(const CPart_Preview& rhs);
	virtual ~CPart_Preview() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	_float4x4 Get_WorldMatrix_Socket() { return m_WorldMatrix; }


private:
	class CTransform*	m_pParentTransform	= { nullptr };
	class CBone*		m_pSocketBone		= { nullptr };
	_float4x4			m_WorldMatrix		= {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CPart_Preview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END
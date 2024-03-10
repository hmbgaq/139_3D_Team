#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)
class CEffect_Trail;

class CPart_Preview final : public CGameObject
{
public:
	typedef struct tagPartPreviewDesc : public CGameObject::GAMEOBJECT_DESC
	{
		 CBone*			pSocketBone			= { nullptr };
		 CTransform*		pParentTransform	= { nullptr };
		 CGameObject*		pOwner				= { nullptr };
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
	HRESULT			Ready_Trail(_uint iLevelIndex, string strFileName);
	void			Set_Trail(CEffect_Trail* pTrail) { m_pTrail = pTrail; };
	CEffect_Trail*	Get_Trail() { return m_pTrail; }


public:
	_float4x4	Get_PartWorldMatrix() { return m_WorldMatrix; }
	void		Set_PartWorldMatrix(_float4x4 _WorldMatrix) { m_WorldMatrix = _WorldMatrix; }

private:
	CEffect_Trail*		m_pTrail = { nullptr };
	_bool				m_bTrailPlay = { TRUE };

private:
	CTransform*			m_pParentTransform	= { nullptr };
	CBone*				m_pSocketBone		= { nullptr };
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
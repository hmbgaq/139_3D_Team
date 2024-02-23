#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Rect final : public CEffect_Void
{
public:
	enum TYPE { SINGLE, SPRITE, TYPE_END };

	typedef struct tagTextureEffectDesc : public CEffect_Void::EFFECTVOID_DESC
	{
		TYPE		eType = { SINGLE };

	}EFFECT_RECT_DESC;


private:
	CEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Rect(const CEffect_Rect& rhs);
	virtual ~CEffect_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	ReSet_Effect()	override;
	virtual void	End_Effect()	override;

public:
	CTexture*				Get_TextureCom(TEXTURE eTexture) { return m_pTextureCom[eTexture]; }

public:
	EFFECT_RECT_DESC*		Get_Desc() { return &m_tRectDesc; }
	UVSPRITE_DESC*			Get_Sprite_Desc() { return &m_tSpriteDesc; }
	DISTORTION_DESC*		Get_Distortion_Desc() { return &m_tDistortionDesc; }

private:
	CShader*				m_pShaderCom				= { nullptr };
	CTexture*				m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom				= { nullptr };

private:
	EFFECT_RECT_DESC		m_tRectDesc			= {};
	UVSPRITE_DESC			m_tSpriteDesc		= {};
	DISTORTION_DESC			m_tDistortionDesc	= {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
class CVIBuffer_Effect_Model_Instance;
END

BEGIN(Client)

class CEffect_Instance final : public CGameObject
{
public:
	enum TYPE	{ FLAT, FIGURE, MESH_TYPE_END };
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_NOISE, TYPE_END };

public:
	typedef struct tagEffect_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		TYPE	eType				= { FLAT };

		wstring strModelTag			= {TEXT("")};

		_uint	iNumInstance		= { 0 };

		wstring	strTextureTag[TYPE_END];
		_int	iTextureIndex[TYPE_END] = { 0 };

		wstring	strShaderTag		= { TEXT("") };
		_uint	iShaderPassIndex	= { 0 };
		_int	iRenderGroup		= { 8 }; //! 밖에서 렌더러의 렌더그룹을 인트로 형변환해서 던져주자 현재 작성기준 CRENDERER::RENDERGROUP::RENDER_END가 8임
		
	}EFFECT_INSTANCE_DESC;

	typedef struct tagSpriteUV
	{
		_float	fTimeAcc = { 0.f };
		_float	fAddTime = { 0.05f };

		_float	fAnimationSizeX = { 292.5f };
		_float	fAnimationSizeY = { 292.5f };

		_float	fSpriteSizeX = { 2048.f };
		_float	fSpriteSizeY = { 2048.f };

		_int	iCurrentVer = { 0 };
		_int	iCurrentHor = { 0 };

		_int	iMinVer = { 0 };
		_int	iMinHor = { 0 };

		_int	iMaxVer = { 7 };
		_int	iMaxHor = { 7 };

	}SPRITEUV_DESC;

private:
	CEffect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Instance(const CEffect_Instance& rhs);
	virtual ~CEffect_Instance() = default;

public:
	virtual HRESULT				Initialize_Prototype()				override;
	virtual HRESULT				Initialize(void* pArg)				override;
	virtual void				Priority_Tick(_float fTimeDelta)	override;
	virtual void				Tick(_float fTimeDelta)				override;
	virtual void				Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT				Render()							override;


public:
	virtual _bool Write_Json(json& Out_Json)		 override;
	virtual void  Load_FromJson(const json& In_Json) override;

/* For.Desc */
public:
	EFFECT_INSTANCE_DESC* Get_Instance_Desc() { return &m_tInstanceDesc; }
	SPRITEUV_DESC*		  Get_Sprite_Desc() { return &m_tSpriteDesc; }

	void		Set_ShaderPassIndex(_uint iShaderPassIndex);

	_bool		bTest = { FALSE };

private:
	CShader*							m_pShaderCom			= { nullptr };	
	CModel*								m_pModelCom				= { nullptr };
	CTexture*							m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Effect_Model_Instance*	m_pVIBufferCom			= { nullptr };

	EFFECT_INSTANCE_DESC				m_tInstanceDesc = {};
	SPRITEUV_DESC						m_tSpriteDesc = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END
#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Grid;
END

BEGIN(Client)

class CGrid final : public CGameObject
{
public:
	enum TEXTURE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_END };

	typedef struct tagGridDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vGridColor = { 0.f, 1.f, 0.f, 1.f };

		wstring		strTextureTag[TEXTURE_END];
		_int		iTextureIndex[TEXTURE_END] = { 0 };

		_bool		bRender = { TRUE };

		// Shader
		_uint		iShaderPassIndex = { 2 };	// Grid는 2번

	}GRID_DESC;

private:
	CGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CGrid(const CGrid& rhs);
	virtual ~CGrid() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void	Priority_Tick(_float fTimeDelta)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	GRID_DESC* Get_Desc() { return &m_tDesc; }

private:
	GRID_DESC	m_tDesc = {};

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Grid*		m_pVIBufferCom = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	/* 원형객체를 생성한다. */
	static CGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
	
};

END


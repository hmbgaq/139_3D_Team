#pragma once

#include "Client_Defines.h"
#include "Effect_Void.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect_Trail final : public CEffect_Void
{
public:

	typedef struct tagTrailDesc
	{
		//_bool		bTrailOn; // EFFECTVOID_DESC의 bPlay
		wstring		strWeaponTag = { TEXT("") };


	}TRAIL_DESC;


private:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void Tick_Trail(_float _fTimeDelta, _float4x4 _ParentMatrix);

public:
	virtual void	Update_PivotMat() override;


/* For.Desc */
public:
	TRAIL_DESC* Get_TrailDesc() { return &m_tTrailDesc; }

public:
	CVIBuffer_Trail* Get_VIBufferCom() { return m_pVIBufferCom; }

private:
	CShader*				m_pShaderCom				= { nullptr };
	CTexture*				m_pTextureCom[TEXTURE_END]	= { nullptr };
	CVIBuffer_Trail*		m_pVIBufferCom				= { nullptr };

private:
	TRAIL_DESC				m_tTrailDesc = {};


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pool() override;

	virtual void Free() override;
};

END
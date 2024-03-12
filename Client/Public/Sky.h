#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSky final : public CGameObject
{
public:
	enum SKYTYPE { SKY_STAGE1, SKY_STAGE1BOSS, SKY_TEMP1, SKY_TEMP2 };

private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_TextureIndex(_int iIndex) { (_uint)m_eSkyType;}
	_int	Get_TextureIndex() { return (_uint)m_eSkyType; }
	
	SKYTYPE Get_CurrentSkyType() { return m_eSkyType;}
	void	Set_SkyType(SKYTYPE eType) { m_eSkyType = eType; }

	void	Set_Render_Tool(_bool bRender) { m_bRender_Tool = bRender; }

public:
	_uint	Get_SkyTextureCount() { return m_pTextureCom->Get_TextureCount();}

private:
	CShader*			m_pShaderCom = { nullptr };	
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Cube*		m_pVIBufferCom = { nullptr };
	
private:
	SKYTYPE				m_eSkyType = SKY_STAGE1;

	_bool				m_bRender_Tool = { TRUE };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strPrototypeTag);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual CGameObject* Pool();

	virtual void Free() override;
	
};

END
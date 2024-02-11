#pragma once

#include "Component.h"

/* 지정한 경로에 있는 텍스쳐들을 로드하여 객체화시킨다. */
/* 객체화한 텍스쳐들을 보관한다. */
/* 셰이더에 텍스쳐의 정보를 던지고 셰이더안에서 이 텍스쳐의 픽셀의 색을 샘플링하여 화면에 찌근ㄴ다. */
/* 화면에 그리기위한 텍스쳐.*/

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
	
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT  Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex = 0);
	HRESULT  Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);



private:
	_uint										m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*>			m_SRVs;	

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
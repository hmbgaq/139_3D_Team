#pragma once

#include "Component.h"
//#include "Hasher.h"

BEGIN(Engine)

class CConstantBuffer;
class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public: /* Get / Set */
	 
public: /* B_SRV */
	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize);

private:
	/* 내가 빌드하여 컴파일한 셰이더 파일 하나를 대표하는 컴객체 . */
	ID3DX11Effect*				m_pEffect = { nullptr };
	vector<ID3D11InputLayout*>	m_InputLayouts;
	D3DX11_TECHNIQUE_DESC		m_TechniqueDesc;

#pragma region About Const Buffer 

//public:
//	typedef struct tagPassDesc
//	{
//		string									strName;
//		ID3DX11EffectPass* pPass = nullptr;
//		D3DX11_PASS_SHADER_DESC					passVsDesc;
//		D3DX11_EFFECT_SHADER_DESC				effectVsDesc;
//		vector<D3D11_SIGNATURE_PARAMETER_DESC>	signatureDescs;
//
//		ID3D11InputLayout* pInputLayout = nullptr;
//	}PASSDESC;
//
//	typedef struct tagTechniqueDesc
//	{
//		string					strName;
//		D3DX11_TECHNIQUE_DESC	desc;
//		ID3DX11EffectTechnique* pTechnique = nullptr;
//		vector<PASSDESC>		vecPasses;
//		unordered_map<string, PASSDESC, djb2Hasher>	hashPasses;
//
//	}TECHNIQUEDESC;
//public:
//	using CBUFFERS = unordered_map<string, pair<CConstantBuffer*, ID3DX11EffectConstantBuffer*>, djb2Hasher>;
//
//	D3DX11_TECHNIQUE_DESC		m_TechniqueDesc;
//	CBUFFERS* m_hashConstantBuffers;
//	vector<TECHNIQUEDESC>	m_vecTechnique;
//
//	ID3DX11EffectConstantBuffer* GetConstantBuffer(const _char * pName);
//	HRESULT Bind_CBuffer(const string & strCBufferName, const void* pData, _uint iLength = 0);

#pragma endregion


public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
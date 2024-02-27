#include "Shader.h"
#include "ConstantBuffer.h"

unordered_map<ID3DX11Effect*, CShader::CBUFFERS*> CShader::m_hashBufferGroups;

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_InputLayouts(rhs.m_InputLayouts)
	, m_TechniqueDesc(rhs.m_TechniqueDesc)
{
	for (auto& pInputLayout : m_InputLayouts)
	{
		Safe_AddRef(pInputLayout);
	}

	Safe_AddRef(m_pEffect);
}


/* 셰이더파일을 빌드하여 ID3DX11Effect를 만들어냈다. */
/* ID3DX11Effect로부터 Pass에 까지 접근하여 인자도 던진 정점정보를 이 셰이더 패스에서 잘 받아줄 수있는지에 대한 InputLayout을 만들었다. */
HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	/*strShaderFilePath경로에 작성되어ㅣㅇㅆ는 hlsl언어 번역 빌드하여 ID3DX11Effect라는 녀석을 만들자.  */
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	pTechnique->GetDesc(&m_TechniqueDesc);

	for (size_t i = 0; i < m_TechniqueDesc.Passes; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex((uint32_t)i);

		D3DX11_PASS_DESC		PassDesc;
		pPass->GetDesc(&PassDesc);

		/* InputLayout : 내가 그리기위해 사용하는 정점의 입력정보.  */
		/* dx11에서는 고정기능 렌더링파이프라인에 대한 기능이 삭제되었다. */
		/* 우리가 직접 렌더링 파이프라인을 수행해야한다.(사용자정의 렌더링파이프라인) -> Shader */
		/* 그래서 우리에겐 반드시 셰이더가 필요하다. */
		/* 우리가 이 정점들을 그리기위해서는 셰이더가 필요하고, 이 셰이더는 반드시 내가 그릴려고하는 정점을 받아줄 수 있어야한다. */
		/* 내가 그리려고하는 정점이 사용하려고하는 셰이더에 입력이 가능한지?에 대한 체크를 사전에 미리 처리하고.
		가능하다면 dx11이 InputLayout이란 객체를 만들어준다. */
		ID3D11InputLayout* pInputLayout = nullptr;

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	//auto iter = m_hashBufferGroups.find(m_pEffect);

	//if (iter == m_hashBufferGroups.end())
	//{
	//		m_hashConstantBuffers = new CBUFFERS;
	//	m_hashBufferGroups.emplace(m_pEffect, m_hashConstantBuffers);
	//}
	//else
	//{
	//	m_hashConstantBuffers = iter->second;
	//}

	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_TechniqueDesc.Passes)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);

	pPass->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* 이 셰이더에 선언되어있는 전역변수의 핸들을 얻어온다.*/
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRV, 0, iNumTextures);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CShader::Bind_Buffer(const _char* pConstantName, const void* pData, _uint iSize)
{
	/* 주의사항 
	 * bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER 일경우에는 던져지는 상수버퍼의 크기가 16의 배수여야한다. 
	 따라서 빈공간에는 float[갯수] 만큼 padding 이 필요하다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectConstantBuffer* pConstantBuffer = pVariable->AsConstantBuffer();
	if (nullptr == pConstantBuffer)
		return E_FAIL;

	// 버퍼 생성
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = iSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = pData;

	ID3D11Buffer* pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, &initData, &pBuffer);
	if (FAILED(hr))
		return hr;

	// 셰이더 리소스에 상수 버퍼 설정
	return pConstantBuffer->SetConstantBuffer(pBuffer);

}

HRESULT CShader::Bind_Struct(const _char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectType* pType = pVariable->GetType();

	D3DX11_EFFECT_TYPE_DESC typeDesc;
	pType->GetDesc(&typeDesc);

	if (typeDesc.Class != D3D10_SVC_STRUCT)
		return E_FAIL;  // 지정된 변수가 구조체 타입이 아니면 실패

	return pVariable->SetRawValue(pData, 0, iSize);
}



CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	Safe_Release(m_pEffect);
}
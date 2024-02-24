#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_RandomNumber(rhs.m_RandomNumber)
	, m_pSpeeds(rhs.m_pSpeeds)
	, m_pLifeTimes(rhs.m_pLifeTimes)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	m_fTimeAcc = 0.f;

	m_InstancingDesc = *(INSTANCING_DESC*)pArg;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	_vector				vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	uniform_real_distribution<float>	RandomRange(0.1f, m_InstancingDesc.fRange);
	uniform_real_distribution<float>	RandomRotation(0.0f, XMConvertToRadians(360.0f));
	uniform_real_distribution<float>	RandomScale(m_InstancingDesc.vScale.x, m_InstancingDesc.vScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_InstancingDesc.vSpeed.x, m_InstancingDesc.vSpeed.y);
	uniform_real_distribution<float>	RandomLifeTime(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = RandomLifeTime(m_RandomNumber);

		_float	fScale = RandomScale(m_RandomNumber);

		pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);

		pVertices[i].vColor.w = 0.f;

		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);
		//XMStoreFloat4(&pVertices[i].vDir, vDir);

		_vector		vRotation = XMQuaternionRotationRollPitchYaw(RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber));

		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMVector3TransformNormal(vDir, RotationMatrix));
		pVertices[i].vPosition.w = 1.f;
	}


	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};


	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}


void CVIBuffer_Instancing::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float		fAlpha = max(m_pLifeTimes[i] - m_fTimeAcc, 0.f);

		pVertices[i].vColor.w = fAlpha;

		_vector		vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_InstancingDesc.vCenter));
		vDir = XMVectorSetW(vDir, 0.f);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * m_pSpeeds[i] * fTimeDelta);

	}


	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::ReSet()
{
	m_fTimeAcc = 0.f;

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	_vector				vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_float				fLength = { 0.0f };

	uniform_real_distribution<float>	RandomRange(0.1f, m_InstancingDesc.fRange);
	uniform_real_distribution<float>	RandomRotation(0.0f, XMConvertToRadians(360.0f));
	uniform_real_distribution<float>	RandomScale(m_InstancingDesc.vScale.x, m_InstancingDesc.vScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_InstancingDesc.vSpeed.x, m_InstancingDesc.vSpeed.y);
	uniform_real_distribution<float>	RandomLifeTime(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
		m_pLifeTimes[i] = RandomLifeTime(m_RandomNumber);

		_float	fScale = RandomScale(m_RandomNumber);

		pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);

		pVertices[i].vColor.w = 1.f;

		vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);

		_vector		vRotation = XMQuaternionRotationRollPitchYaw(RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber));

		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMVector3TransformNormal(vDir, RotationMatrix));
		pVertices[i].vPosition.w = 1.f;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Instancing::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pLifeTimes);
	}


	Safe_Release(m_pVBInstance);
}

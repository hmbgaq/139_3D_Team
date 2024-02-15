#pragma once

#include "Component.h"

/* 특정 형태(Rect, Cube, Terrain, Model) 를 구성하기위한 클래스들의 부모가 되는 클래스다 .*/
/* VIBuffer = Vertex(정점) + Index(그리는 순서에 따른 정점의 인덱스 집합.) + Buffer(메모리공간) */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

public:

	virtual HRESULT Bind_VIBuffers();
	/* 정점, 인덱스, 인스턴스 버퍼들을 생성한다. */
	HRESULT Create_Buffer(_Inout_ ID3D11Buffer**	ppBuffer);

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

	/* 지금 내가 만들려고 하는 버퍼의 속성을 설정하낟. */
	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

protected:
	_uint						m_iNumVertices = { 0 };
	_uint						m_iStride = { 0 };
	_uint						m_iNumVertexBuffers = { 0 };

	_uint						m_iNumIndices = { 0 };
	_uint						m_iIndexStride = { 0 };
	DXGI_FORMAT					m_eIndexFormat = {  };
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology = { };

	_float3*					m_pVerticesPos = { nullptr };
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
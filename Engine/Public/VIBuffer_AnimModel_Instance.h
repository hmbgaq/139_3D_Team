#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_AnimModel_Instance final : public CVIBuffer
{
public:
	typedef struct tagVIBuffer_AnimModelInstanceDesc
	{
		_uint iMaxInstance = {};
		_uint iSizePerInstance = {};
		void* pValue = { nullptr };
	}ANIMINSTANCE_BUFFER_DESC;

protected:
    CVIBuffer_AnimModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_AnimModel_Instance(const CVIBuffer_AnimModel_Instance& rhs);
    virtual ~CVIBuffer_AnimModel_Instance() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(CMesh* pMesh);
	virtual HRESULT		Render(CMesh* pMesh, _int iMeshIndex);
	virtual void		Update(_float fTimeDelta);


protected:
	ANIMINSTANCE_BUFFER_DESC	m_tModelDesc = {};

protected:
	ID3D11Buffer*		m_pVBInstance = { nullptr };


	
	
public:
	static CVIBuffer_AnimModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_Model_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagVIBuffer_ModelInstanceDesc
	{
		class CModel*	  pModel = { nullptr };
		_int			  iNumInstance = {0};

	}MODEL_INSTANCE_DESC;

protected:
    CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs);
    virtual ~CVIBuffer_Model_Instance() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(_uint iMeshContainerIndex);
	virtual void		Update(_float fTimeDelta);


public:
/*	void				Add_Mesh(_fmatrix vWorldMatrix);*/
	virtual void		Init_Instance(_int iNumInstance);
	virtual	HRESULT		Render(_int iMeshIndex);

protected:
	MODEL_INSTANCE_DESC	m_tModelDesc = {};

protected:
	ID3D11Buffer*		m_pVBInstance = { nullptr };
	_uint				m_iInstanceStride = { 0 };
	_uint				m_iNumInstance = { 0 };
	_uint				m_iIndexCountPerInstance = { 0 };

	_int				m_iNumMeshes = 0;
	_int				m_iNumMaterials = 0;

	vector<CMesh*>		m_vecInstanceMesh = {};
	vector<float4x4>	m_vecWorldMatrix = {};
	
	
public:
	virtual void Free() override;

};

END
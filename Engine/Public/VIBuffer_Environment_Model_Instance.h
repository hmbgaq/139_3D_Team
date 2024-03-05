#pragma once
#include "VIBuffer_Model_Instance.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_Environment_Model_Instance : public CVIBuffer_Model_Instance
{
public:
	typedef struct tagVIBuffer_EnvironmentModelInstanceDesc : public CVIBuffer_Model_Instance::MODEL_INSTANCE_DESC
	{
		_bool						bAnimType = false;
		vector<INSTANCE_INFO_DESC>	vecBufferInstanceInfo;
	}ENVIRONMENT_MODEL_INSTANCE_DESC;

private:
    CVIBuffer_Environment_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Environment_Model_Instance(const CVIBuffer_Environment_Model_Instance& rhs);
    virtual ~CVIBuffer_Environment_Model_Instance() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(_uint iMeshContainerIndex);
	virtual void		Update(vector<INSTANCE_INFO_DESC>& vecInstanceDesc);
			void		Update(INSTANCE_INFO_DESC InstanceDesc, _int iIndex);
			HRESULT		Remove_Instance(_uint iIndex);

			HRESULT		Bind_ShaderResources(class CShader* pShader, _int iIndex);
public:
/*	void					Add_Mesh(_fmatrix vWorldMatrix);*/
	void				Init_Instance(_int iNumInstance) override;
	virtual				HRESULT	Render(_int iMeshIndex) override;


private:
	vector<INSTANCE_INFO_DESC> m_tInstanceInfo;

public:
	static CVIBuffer_Environment_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
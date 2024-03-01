#pragma once
#include "VIBuffer_Model_Instance.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_Effect_Model_Instance : public CVIBuffer_Model_Instance
{
public:
	typedef struct tagVIBuffer_EffectModelInstanceDesc
	{
		class CModel*	  pModel			= { nullptr };

		_int			  iCurNumInstance	= { 1 };

		_float4		vCurrentPosition	= {0.f, 0.f, 0.f, 1.f};

	}EFFECT_MODEL_INSTANCE_DESC;

private:
    CVIBuffer_Effect_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Effect_Model_Instance(const CVIBuffer_Effect_Model_Instance& rhs);
    virtual ~CVIBuffer_Effect_Model_Instance() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(_uint iMeshContainerIndex);
	virtual void		Update(_float fTimeDelta) override;

	void				Init_Instance(_int iNumInstance) override;
	virtual				HRESULT	Render(_int iMeshIndex) override;

public:
	void ReSet();

public:
	virtual _bool Write_Json(json& Out_Json)		override;
	virtual void Load_FromJson(const json& In_Json)	override;



public:
	EFFECT_MODEL_INSTANCE_DESC* Get_Desc() { return &m_tBufferDesc; }

private:
	EFFECT_MODEL_INSTANCE_DESC			m_tBufferDesc;


public:
	static CVIBuffer_Effect_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
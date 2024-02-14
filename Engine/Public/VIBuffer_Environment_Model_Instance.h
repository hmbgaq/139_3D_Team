#pragma once
#include "VIBuffer_Model_Instance.h"


BEGIN(Engine)
class CMesh;

class ENGINE_DLL CVIBuffer_Environment_Model_Instance : public CVIBuffer_Model_Instance
{
public:
	typedef struct tagVIBuffer_EnvironmentModelInstanceDesc
	{
		class CModel*	  pModel = { nullptr };
		_int			  iNumInstance = {0};
	}ENVIRONMENT_MODEL_INSTANCE_DESC;

	typedef struct tag_InstanceDesc
	{
		_float3         vRotation;
		_float3         vScale;
		_float3			vTranslation;
		_float			fMaxRange;
		_float3			vCenter;

		_matrix Get_Matrix() const
		{
			_matrix TransformationMatrix;
			_matrix RotationMatrix, ScaleMatrix;

			_vector vPitchYawRoll;
			_vector vPosition;

			vPitchYawRoll = XMLoadFloat3(&vRotation);
			vPosition = XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f);

			RotationMatrix = XMMatrixRotationRollPitchYawFromVector(vPitchYawRoll);
			ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
			TransformationMatrix = ScaleMatrix * RotationMatrix;
			TransformationMatrix.r[3] = vPosition;

			return TransformationMatrix;
		}

		void	Bake_CenterWithMatrix()
		{
			_vector vCenterFromVector = XMLoadFloat3(&vCenter);
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(vCenterFromVector, Get_Matrix()));
		}
	}INSTANCE_INFO_DESC;

private:
    CVIBuffer_Environment_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CVIBuffer_Environment_Model_Instance(const CVIBuffer_Environment_Model_Instance& rhs);
    virtual ~CVIBuffer_Environment_Model_Instance() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Bind_VIBuffers(_uint iMeshContainerIndex);
	virtual void		Update(vector<INSTANCE_INFO_DESC>& vecInstanceDesc);


public:
/*	void				Add_Mesh(_fmatrix vWorldMatrix);*/
	void				Init_Instance(_int iNumInstance) override;
	virtual				HRESULT	Render(_int iMeshIndex) override;


public:
	static CVIBuffer_Environment_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END
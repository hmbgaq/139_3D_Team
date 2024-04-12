#pragma once

#include "Model.h"
#include "MyAINode.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}
	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	_float4x4& Get_CombinedTransformationFloat4x4()  { return m_CombinedTransformationMatrix; }

	_float4&   Get_CombinedPosition(_fmatrix WorldMatrix);

public:
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_float4x4 Get_TransformationMatrix() {
		return m_TransformationMatrix;
	}


public:
	HRESULT Initialize(CMyAINode pAINode, _int iParentIndex);
	void Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix, _float3& _Pos);


public:
	_int Get_Index() { return m_iIndex; };
	void Set_Index(_int _iIndex) { m_iIndex = _iIndex; };

private:
	_char				m_szName[MAX_PATH] = "";
	_int				m_iParentIndex = { 0 };
	_int				m_iIndex = { -10 };

	/* 이 뼈만의 상태행렬 */
	_float4x4			m_TransformationMatrix = {};

	/* 이 뼈만의 상태행렬 * 부모뼈의 m_CombindTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix = {};

public:
	static CBone* Create(CMyAINode pAINode, _int iParentIndex);
	CBone* Clone();
	virtual void Free() override;
};

END
#include "..\Public\Bone.h"

CBone::CBone()
{
}

_vector& CBone::Get_CombinedPosition(_fmatrix WorldMatrix)
{
	// TODO: 여기에 return 문을 삽입합니다.

	_vector vBonePosition = _float4(m_CombinedTransformationMatrix._41, m_CombinedTransformationMatrix._42, m_CombinedTransformationMatrix._43, m_CombinedTransformationMatrix._44);
	
	return XMVector3TransformCoord(vBonePosition, WorldMatrix);
}

HRESULT CBone::Initialize(CMyAINode pAINode, _int iParentIndex)
{
	m_iParentIndex = iParentIndex;

	strcpy_s(m_szName, pAINode.Get_Name().c_str());

	memcpy(&m_TransformationMatrix, &pAINode.Get_Transformation(), sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix, _float3& _Pos)
{
	if (-1 == m_iParentIndex)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	}
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
	}

	if (!strcmp(m_szName, "Root_$AssimpFbx$_Translation"))
	{
		_Pos.x = m_CombinedTransformationMatrix._41;
		_Pos.y = m_CombinedTransformationMatrix._42;
		_Pos.z = m_CombinedTransformationMatrix._43;

		m_CombinedTransformationMatrix._41 = 0;
		m_CombinedTransformationMatrix._42 = 0;
		m_CombinedTransformationMatrix._43 = 0;
	}

}

CBone * CBone::Create(CMyAINode pAINode, _int iParentIndex)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);
}


void CBone::Free()
{
}

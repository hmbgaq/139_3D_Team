#include "..\Public\Bone.h"

CBone::CBone()
{
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

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	if (-1 == m_iParentIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
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

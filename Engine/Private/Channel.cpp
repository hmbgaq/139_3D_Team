#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim * pChannel, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pChannel->mNodeName.data);

	_uint		iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
	{
		if (false == strcmp(m_szName, pBone->Get_Name()))
		{
			return true;
		}

		++iBoneIndex;

		return false;
	});

	if (iter == Bones.end())
		return E_FAIL;

	m_iBoneIndex = iBoneIndex;

	m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(pChannel->mNumPositionKeys, m_iNumKeyFrames);

	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame = {};

		if(i < pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pChannel->mScalingKeys[i].mTime;
		}
		if (i < pChannel->mNumRotationKeys)
		{
			// memcpy(&vRotation, &pChannel->mRotationKeys[i].mValue, sizeof(_float4));
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = (_float)pChannel->mRotationKeys[i].mTime;
		}
		if (i < pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex= 0;

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	/* 마지막 상태(행렬)을 취하면 된다. */
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	/* 선형보간을 통해 현재의 상태을 만들어낸다. */
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex+ 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vSourPosition = m_KeyFrames[*pCurrentKeyFrameIndex].vPosition;

		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vScale;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vRotation;
		vDestPosition = m_KeyFrames[*pCurrentKeyFrameIndex+ 1].vPosition;

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / 
			(m_KeyFrames[*pCurrentKeyFrameIndex+ 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
	}	

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	/* 이 채널과 같은 이름을 가진 뼈를 찾아서 그 뼈의 TransformationMAtrix를 갱신한다. */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel * CChannel::Create(const aiNodeAnim * pChannel, const CModel::BONES& Bones)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannel, Bones)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
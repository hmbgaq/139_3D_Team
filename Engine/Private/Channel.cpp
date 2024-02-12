#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CMyAINodeAnimation pChannel, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pChannel.Get_Name().c_str());

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

	m_iNumKeyFrames = max(pChannel.Get_NumScalingKeys(), pChannel.Get_NumRotationKeys());
	m_iNumKeyFrames = max(pChannel.Get_NumPositionKeys(), m_iNumKeyFrames);

	_float3		vScale;
	_float4		vRotation;
	_float3		vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame = {};

		if (i < pChannel.Get_NumScalingKeys())
		{
			memcpy(&vScale, &pChannel.Get_ScalingKeys(i).Get_Value(), sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel.Get_ScalingKeys(i).Get_Time();
		}
		if (i < pChannel.Get_NumRotationKeys())
		{
			// memcpy(&vRotation, &pChannel->mRotationKeys[i].mValue, sizeof(_float4));
			vRotation.x = pChannel.Get_RotationKeys(i).Get_X();
			vRotation.y = pChannel.Get_RotationKeys(i).Get_Y();
			vRotation.z = pChannel.Get_RotationKeys(i).Get_Z();
			vRotation.w = pChannel.Get_RotationKeys(i).Get_W();
			KeyFrame.fTrackPosition = pChannel.Get_RotationKeys(i).Get_Time();
		}
		if (i < pChannel.Get_NumPositionKeys())
		{
			memcpy(&vPosition, &pChannel.Get_PositionKeys(i).Get_Value(), sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel.Get_PositionKeys(i).Get_Time();
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	//ZeroMemory(&m_StartFrame, sizeof(m_StartFrame));
	//ZeroMemory(&m_EndFrame, sizeof(m_EndFrame));

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	Invalidate_TransformationMatrix_Normal(fCurrentTrackPosition, Bones, pCurrentKeyFrameIndex);
}

void CChannel::Invalidate_TransformationMatrix_Normal(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{
	KEYFRAME _StartFrame;
	KEYFRAME _EndFrame;

	_bool bIsNotEndKeyFrame = Update_KeyFrame(_StartFrame, _EndFrame, fCurrentTrackPosition, pCurrentKeyFrameIndex);

	_float	fStart = _StartFrame.fTrackPosition;
	_float	fEnd = _EndFrame.fTrackPosition;
	_float	fRatio = Calc_Ratio(fStart, fCurrentTrackPosition, fEnd);

	_matrix		TransformationMatrix;

	if (!bIsNotEndKeyFrame)
	{
		TransformationMatrix = Make_EndFrame_TransformationMatrix();
	}
	else
	{
		TransformationMatrix = Make_TransformationMatrix(_StartFrame, _EndFrame, fRatio);
	}

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_Reverse(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{
	KEYFRAME _StartFrame;
	KEYFRAME _EndFrame;

	// 나중에 수정할 예정
	*pCurrentKeyFrameIndex = 0;
	_bool bIsNotEndKeyFrame = Update_KeyFrame(_StartFrame, _EndFrame, fCurrentTrackPosition, pCurrentKeyFrameIndex);


	_float	fStart = _StartFrame.fTrackPosition;
	_float	fEnd = _EndFrame.fTrackPosition;
	_float	fRatio = 1 - Calc_Ratio(fStart, fCurrentTrackPosition, fEnd);

	_matrix		TransformationMatrix;

	if (!bIsNotEndKeyFrame)
	{
		TransformationMatrix = Make_EndFrame_TransformationMatrix();
	}
	else
	{
		TransformationMatrix = Make_TransformationMatrix(_EndFrame, _StartFrame, fRatio);
	}

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_Transition(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fCurrentTrackPosition, const CModel::BONES& Bones)
{
	_float	fStart = _StartFrame.fTrackPosition;
	_float	fEnd = _EndFrame.fTrackPosition;
	_float	fRatio = Calc_Ratio(fStart, fCurrentTrackPosition, fEnd);

	_matrix		TransformationMatrix;

	_bool bIsEnd = (1.0f <= fRatio);

	TransformationMatrix = Make_TransformationMatrix(_StartFrame, _EndFrame, fRatio);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Reset_Channel(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{
	Invalidate_TransformationMatrix(fCurrentTrackPosition, Bones, pCurrentKeyFrameIndex);
}

_bool CChannel::Is_Inputable(_float fCurrentTrackPosition, _uint _iIndex)
{
	_uint iIndex = _iIndex;
	if (m_iNumKeyFrames - 1 <= iIndex)
	{
		iIndex = m_iNumKeyFrames - 1;
	}

	KEYFRAME	InputableKeyFrame = m_KeyFrames[_iIndex];
	if (fCurrentTrackPosition >= InputableKeyFrame.fTrackPosition)
	{
		return true;
	}
	return false;
}

_bool CChannel::Is_Inputable_Front(_float fCurrentTrackPosition, _uint _iIndexFront)
{
	return Is_Inputable(fCurrentTrackPosition, _iIndexFront);
}

_bool CChannel::Update_KeyFrame(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		_StartFrame = m_KeyFrames.back();
		_EndFrame = m_KeyFrames.back();
		return false;
	}
	else
	{
		try
		{
			*pCurrentKeyFrameIndex = 0;
			while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
				++* pCurrentKeyFrameIndex;
		}
		catch (_uint e)
		{
			*pCurrentKeyFrameIndex = m_KeyFrames.size() - 2;
		}

		_StartFrame = m_KeyFrames[*pCurrentKeyFrameIndex];
		_EndFrame = m_KeyFrames[*pCurrentKeyFrameIndex + 1];
		return true;
	}
}

_float CChannel::Calc_Ratio(_float fStart, _float fNow, _float fEnd)
{
	if (fEnd <= fStart)
		return 1.0f;

	return min(((fNow - fStart) / (fEnd - fStart)), 1);
}

void CChannel::Calc_Now_Transform(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _vector& vScale, _vector& vRotation, _vector& vPosition, _float fRatio)
{
	_float3		vSourScale, vDestScale;
	_float4		vSourRotation, vDestRotation;
	_float3		vSourPosition, vDestPosition;

	vSourScale = _StartFrame.vScale;
	vSourRotation = _StartFrame.vRotation;
	vSourPosition = _StartFrame.vPosition;

	vDestScale = _EndFrame.vScale;
	vDestRotation = _EndFrame.vRotation;
	vDestPosition = _EndFrame.vPosition;

	vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
	vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
}

_matrix CChannel::Make_TransformationMatrix(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fRatio)
{
	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	Calc_Now_Transform(_StartFrame, _EndFrame, vScale, vRotation, vPosition, fRatio);

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	return TransformationMatrix;
}

_matrix CChannel::Make_SelectedFrame_TransformationMatrix(_uint iIndex)
{
	_uint iFrameIndex = iIndex;
	if (iFrameIndex < m_iNumKeyFrames - 1)
		iFrameIndex = m_iNumKeyFrames - 1;

	KEYFRAME	SelectedKeyFrame = m_KeyFrames[iFrameIndex];

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	vScale = XMLoadFloat3(&SelectedKeyFrame.vScale);
	vRotation = XMLoadFloat4(&SelectedKeyFrame.vRotation);
	vPosition = XMLoadFloat3(&SelectedKeyFrame.vPosition);

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	return TransformationMatrix;
}

_matrix CChannel::Make_StartFrame_TransformationMatrix()
{
	return Make_SelectedFrame_TransformationMatrix(0);
}

_matrix CChannel::Make_EndFrame_TransformationMatrix()
{
	return Make_SelectedFrame_TransformationMatrix(m_iNumKeyFrames - 1);
}

KEYFRAME CChannel::Make_NowFrame(_float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	KEYFRAME result;

	KEYFRAME _StartFrame;
	KEYFRAME _EndFrame;
	Update_KeyFrame(_StartFrame, _EndFrame, fCurrentTrackPosition, pCurrentKeyFrameIndex);


	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	_float	fStart = _StartFrame.fTrackPosition;
	_float	fNow = fCurrentTrackPosition;
	_float	fEnd = _EndFrame.fTrackPosition;
	_float	fRatio = Calc_Ratio(fStart, fNow, fEnd);

	Calc_Now_Transform(_StartFrame, _EndFrame, vScale, vRotation, vPosition, fRatio);

	XMStoreFloat3(&result.vPosition, vPosition);
	XMStoreFloat4(&result.vRotation, vRotation);
	XMStoreFloat3(&result.vScale, vScale);

	return result;
}

CChannel * CChannel::Create(CMyAINodeAnimation pChannel, const CModel::BONES& Bones)
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
#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_isFinished(rhs.m_isFinished)
	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);	
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrames.resize(m_iNumChannels);

	/* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fTrackPosition >= m_fDuration)
	{
		m_isFinished = true;
		m_fTrackPosition = m_fDuration;

		if (true == isLoop)
		{
			m_fTrackPosition = 0.0f;
			m_isFinished = false;
		}
	}		

	/* 내 애니메이션이 이용하는 전체 뼈의 상태를 m_fTrackPosition 시간에 맞는 상태로 갱신하다.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
	}
}

CChannel* CAnimation::Get_Channel_By_BoneIndex(_uint _iBoneIndex, _uint& _iChannelIndex)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (_iBoneIndex == m_Channels[i]->Get_BoneIndex())
		{
			CChannel* pChannel = m_Channels[i];
			_iChannelIndex = i;
			return pChannel;
		}
	}

	return nullptr;
}

void CAnimation::Reset_Animation(const CModel::BONES& Bones)
{
	m_isFinished = false;
	m_bIsTransition = false;
	m_fTrackPosition = 0.0f;
	m_PrevPos = { 0.f, 0.f, 0.f };

	Reset_TransitionKeyFrame();

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Reset_Channel(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
	}
}

void CAnimation::Set_Transition(CAnimation* prevAnimation, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	m_PrevPos = { 0.f, 0.f, 0.f };

	_float fTransitionDuration = (_fTransitionDuration > 0.f ? _fTransitionDuration : 0.3f);
	m_fTransitionEnd = m_Channels[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
	m_fTrackPosition = m_fTransitionEnd - fTransitionDuration;

	m_StartTransitionKeyFrame.clear();
	m_EndTransitionKeyFrame.clear();


	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = m_Channels[i];
		_uint		targetBoneIndex = pChannel->Get_BoneIndex();

		_uint		pPrevChannelIndex;
		CChannel* pPrevChannel = prevAnimation->Get_Channel_By_BoneIndex(targetBoneIndex, pPrevChannelIndex);

		KEYFRAME	_StartFrame;
		KEYFRAME	_EndFrame;

		if (pPrevChannel)
		{
			if (prevAnimation->Is_Transition())
			{
				_StartFrame = prevAnimation->Make_NowFrame(pPrevChannelIndex);
			}
			else
			{
				_float fPrevAnimTrackPosition = prevAnimation->Get_TrackPosition();
				_StartFrame = pPrevChannel->Make_NowFrame(fPrevAnimTrackPosition, &m_CurrentKeyFrames[i]);
			}
		}
		else
		{
			_StartFrame = pChannel->Get_KeyFrame(iTargetKeyFrameIndex);
		}

		_EndFrame = pChannel->Get_KeyFrame(iTargetKeyFrameIndex);

		_StartFrame.fTrackPosition = m_fTrackPosition;

		m_StartTransitionKeyFrame.push_back(_StartFrame);
		m_EndTransitionKeyFrame.push_back(_EndFrame);
	}
	m_bIsTransition = true;
}

KEYFRAME CAnimation::Make_NowFrame(_uint m_iChannelIndex)
{
	KEYFRAME result;
	KEYFRAME _StartFrame = m_StartTransitionKeyFrame[m_iChannelIndex];
	KEYFRAME _EndFrame = m_EndTransitionKeyFrame[m_iChannelIndex];

	_float		fRatio;

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;


	{
		_float	fStart = _StartFrame.fTrackPosition;
		_float	fNow = m_fTrackPosition;
		_float	fEnd = _EndFrame.fTrackPosition;
		fRatio = fEnd <= fStart ? 1.0f : min(((fNow - fStart) / (fEnd - fStart)), 1);
	}


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

	XMStoreFloat3(&result.vPosition, vPosition);
	XMStoreFloat4(&result.vRotation, vRotation);
	XMStoreFloat3(&result.vScale, vScale);

	return result;
}

_bool CAnimation::Is_Inputable_Front(_uint _iIndexFront)
{
	return m_Channels[0]->Is_Inputable_Front(m_fTrackPosition, _iIndexFront);
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, const CModel::BONES& Bones)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation * CAnimation::Clone()
{
	return new CAnimation(*this);	
}


void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}
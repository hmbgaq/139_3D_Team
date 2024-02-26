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
	, m_iMaxFrameCount(rhs.m_iMaxFrameCount)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);	
}

HRESULT CAnimation::Initialize(CMyAIAnimation pAIAnimation, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pAIAnimation.Get_Name().c_str());

	m_fDuration = (_float)pAIAnimation.Get_Duration();
	m_fTickPerSecond = (_float)pAIAnimation.Get_TickPerSecond();

	m_iNumChannels = pAIAnimation.Get_NumChannels();

	m_CurrentKeyFrames.resize(m_iNumChannels);

	/* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation.Get_Channel((_uint)i), Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	for (auto& pChannel : m_Channels)
		m_iMaxFrameCount = max(m_iMaxFrameCount, (_uint)pChannel->Get_KeyFrames().size());
	return S_OK;
}

_bool CAnimation::Invalidate_TransformationMatrix(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones)
{
	_bool _bPrevTransition = m_bIsTransition;
	if (m_bIsTransition)
	{
		m_fTrackPosition += m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;

		if (m_fTransitionEnd <= m_fTrackPosition)
		{
			m_bIsTransition = false;
			m_fTrackPosition = m_fTransitionEnd;
		}
	}
	else
	{
		switch (_eAnimState)
		{
		case Engine::CModel::ANIM_STATE_NORMAL:
			m_fTrackPosition += m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;
			if (m_fTrackPosition >= m_fDuration)
			{
				m_isFinished = true;
				m_fTrackPosition = m_fDuration;
			}
			break;
		case Engine::CModel::ANIM_STATE_LOOP:
			m_fTrackPosition += m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;
			if (m_fTrackPosition >= m_fDuration)
			{
				m_fTrackPosition = 0.0f;
				m_PrevPos = { 0.f, 0.f, 0.f };
			}
			break;
		case Engine::CModel::ANIM_STATE_REVERSE:
			m_fTrackPosition -= m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;
			if (m_fTrackPosition <= 0)
			{
				m_isFinished = true;
				m_fTrackPosition = 0.f;
			}
			break;
		case Engine::CModel::ANIM_STATE_STOP:
			m_isFinished = true;
			break;
		default:
			break;
		}

	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		if (m_bIsTransition)
		{
			KEYFRAME	_StartFrame = m_StartTransitionKeyFrame[i];
			KEYFRAME	_EndFrame = m_EndTransitionKeyFrame[i];

			m_Channels[i]->Invalidate_TransformationMatrix_Transition(_StartFrame, _EndFrame, m_fTrackPosition, Bones);
		}
		else
		{
			switch (_eAnimState)
			{
			case Engine::CModel::ANIM_STATE_NORMAL:
			case Engine::CModel::ANIM_STATE_LOOP:
				m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
			case Engine::CModel::ANIM_STATE_REVERSE:
				m_Channels[i]->Invalidate_TransformationMatrix_Reverse(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
				break;
			default:
				break;
			}
		}

	}

	m_bIsTransitionEnd_Now = _bPrevTransition != m_bIsTransition;

	return m_isFinished;
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

HRESULT CAnimation::Calculate_Animation(_uint iFrame, const CModel::BONES& Bones)
{
	/* 모든 채널의 키프레임을 iFrame으로 세팅한다. */
	for (auto& pChannel : m_Channels)
	{
		for (auto& iCurrentKeyFrame : m_CurrentKeyFrames)
			iCurrentKeyFrame = iFrame;
	}

	/* 위에서 지전항 키프레임대로, 모든 채널의 키프레임을 보간한다. (아직 부모 기준) */
	_uint iChannelIndex = 0;
	
	for (auto& pChannel : m_Channels)
	{
		m_CurrentKeyFrames[iChannelIndex]
			= pChannel->Invalidate_TransformationMatrix_Test(m_fTrackPosition, Bones, &(m_CurrentKeyFrames[iChannelIndex]));

		++iChannelIndex;
	}

	return S_OK;
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

_float4x4* CAnimation::Get_TransformationBoneMatrices(_float fTrackPosition, _float4x4* pMatrix)
{
	
	for (_int i = 0; i < m_iNumChannels; ++i)
	{
		_uint iBoneIndex = m_Channels[i]->Get_BoneIndex();
		pMatrix[iBoneIndex] = m_Channels[i]->Get_NowMatrix(fTrackPosition);
	}

	return pMatrix;
}

HRESULT CAnimation::Clear_Channels()
{
	/* Channel */
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();


	m_CurrentKeyFrames.clear();

	return S_OK;
}



CAnimation * CAnimation::Create(CMyAIAnimation pAIAnimation, const CModel::BONES& Bones)
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
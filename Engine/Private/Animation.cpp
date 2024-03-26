#include "..\Public\Animation.h"
#include "Channel.h"
#include "Bone.h"

#include "GameInstance.h"

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

	return S_OK;
}

_bool CAnimation::Invalidate_TransformationMatrix(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones, _bool _bIsSplitted)
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
		case Engine::CModel::ANIM_STATE_LOOP_REVERSE:
			m_fTrackPosition -= m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;
			if (m_fTrackPosition <= 0)
			{
				m_fTrackPosition = m_fDuration;
				m_PrevPos = { 0.f, 0.f, 0.f };
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
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

		if (true == _bIsSplitted && Is_UpperBody(iChannelIndex))
		{
			continue;
		}
		else 
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
					break;
				case Engine::CModel::ANIM_STATE_REVERSE:
				case Engine::CModel::ANIM_STATE_LOOP_REVERSE:
					m_Channels[i]->Invalidate_TransformationMatrix_Reverse(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
					break;
				default:
					break;
				}
			}
		}	
	}

	m_bIsTransitionEnd_Now = _bPrevTransition != m_bIsTransition;

	return m_isFinished;
}

_bool CAnimation::Invalidate_TransformationMatrix_Upper(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones, _float2 vMouseMove)
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
			m_isFinished = false;
			break;
		default:
			break;
		}

	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

		if (Is_UpperBody(iChannelIndex))
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
				case Engine::CModel::ANIM_STATE_STOP:
					m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
					break;
				case Engine::CModel::ANIM_STATE_REVERSE:
					m_Channels[i]->Invalidate_TransformationMatrix_Reverse(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
					break;
				default:
					break;
				}
			}

			//HERE
			if (m_Channels[i]->Get_BoneIndex() == 10)
			{
				CBone* pBone = Bones[m_Channels[i]->Get_BoneIndex()];

				_float4x4 Transform = pBone->Get_TransformationMatrix();

				_float4x4	UpperSpineMatrix;

				//Y - Right
				//X - Up
				//Z - Look

				_matrix RotateX = XMMatrixRotationX(XMConvertToRadians(vMouseMove.x + 1.0f));
				_matrix RotateY = XMMatrixRotationY(XMConvertToRadians(vMouseMove.y));


				//XMStoreFloat4x4(&UpperSpineMatrix, RotateX);
				XMStoreFloat4x4(&UpperSpineMatrix, XMMatrixMultiply(RotateX, RotateY));


				_float4x4 ResultMatrix = XMMatrixMultiply(Transform, UpperSpineMatrix);

				pBone->Set_TransformationMatrix(ResultMatrix);
			}
		}
	}

	return m_isFinished;
}

_bool CAnimation::Invalidate_TransformationMatrix_Parasiter(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones,_float3 pPlayerPos)
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
		case Engine::CModel::ANIM_STATE_LOOP_REVERSE:
			m_fTrackPosition -= m_fTickPerSecond / m_fStiffnessRate * fTimeDelta;
			if (m_fTrackPosition <= 0)
			{
				m_fTrackPosition = m_fDuration;
				m_PrevPos = { 0.f, 0.f, 0.f };
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
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

	
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
				break;
			case Engine::CModel::ANIM_STATE_REVERSE:
				m_Channels[i]->Invalidate_TransformationMatrix_Reverse(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
				break;
			default:
				break;
			}
		}
		

		//HERE
		if (m_Channels[i]->Get_BoneIndex() == 17)
		{
			CBone* pBone = Bones[m_Channels[i]->Get_BoneIndex()];

			_float4x4 Transform = pBone->Get_TransformationMatrix();

			_float4x4 playerToParasiterMatrix;

			// 플레이어 위치 가져오기
			_float3 playerPosition = pPlayerPos;

			// 몬스터 목뼈 변환 행렬 가져오기
			_matrix boneTransform = XMLoadFloat4x4(&Transform);
			
			////
			//_vector forward = XMVector3Normalize(XMVectorSet(boneTransform.m[0][0], boneTransform.m[0][1], boneTransform.m[0][2], 0.f));

			// 플레이어 위치를 로컬 공간으로 변환
			//_float3 localPosition = XMVector3TransformCoord(XMLoadFloat3(&playerPosition), Transform);

			//목뼈 방향 벡터
			_float3 BonePosition = _float3(Transform.m[0][0], Transform.m[0][1], Transform.m[0][2]);
		
			// 로컬 회전 행렬 계산
			_matrix localRotation = XMMatrixLookAtLH(BonePosition, playerPosition, XMVectorSet(0.0f, 1.0f, 0.0f, 0.f));

			// 월드 공간 변환 행렬에 곱하기
			boneTransform = XMMatrixMultiply(boneTransform, localRotation);


			//_float4x4 ResultMatrix = XMMatrixMultiply(Transform, playerToParasiterMatrix);

			pBone->Set_TransformationMatrix(localRotation);
		}
	}

	//m_bIsTransitionEnd_Now = _bPrevTransition != m_bIsTransition;

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

void CAnimation::Reset_Animation(const CModel::BONES& Bones, _bool _bIsSplitted)
{
	m_isFinished = false;
	m_bIsTransition = false;
	m_fTrackPosition = 0.0f;
	m_PrevPos = { 0.f, 0.f, 0.f };

	Reset_TransitionKeyFrame();

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

		if (true == _bIsSplitted && Is_UpperBody(iChannelIndex))
		{
			continue;
		}

		m_Channels[i]->Reset_Channel(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
	}
}

void CAnimation::Reset_UpperAnimation(const CModel::BONES& Bones)
{
	m_isFinished = false;
	m_bIsTransition = false;
	m_fTrackPosition = 0.0f;
	m_PrevPos = { 0.f, 0.f, 0.f };

	Reset_TransitionKeyFrame();

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

		if (Is_UpperBody(iChannelIndex))
		{
			m_Channels[i]->Reset_Channel(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
		}
	}
}

void CAnimation::Set_Transition(CAnimation* prevAnimation, _float _fTransitionDuration, _uint iTargetKeyFrameIndex, _bool _bIsSplitted)
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

void CAnimation::Set_Transition_Upper(CAnimation* prevAnimation, _float _fTransitionDuration, _uint iTargetKeyFrameIndex)
{
	_float fTransitionDuration = (_fTransitionDuration > 0.f ? _fTransitionDuration : 0.3f);
	m_fTransitionEnd = m_Channels[0]->Get_KeyFrame(iTargetKeyFrameIndex).fTrackPosition;
	m_fTrackPosition = m_fTransitionEnd - fTransitionDuration;

	m_StartTransitionKeyFrame.clear();
	m_EndTransitionKeyFrame.clear();


	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pNowChannel = m_Channels[i];
		_int iChannelIndex = m_Channels[i]->Get_BoneIndex();

		if (Is_UpperBody(iChannelIndex))
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
		else
		{
			KEYFRAME	_StartFrame;
			KEYFRAME	_EndFrame;
			m_StartTransitionKeyFrame.push_back(_StartFrame);
			m_EndTransitionKeyFrame.push_back(_EndFrame);
		}
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

_bool CAnimation::Is_Inputable_Front(_float _fTrackPosition,_uint _iIndexFront)
{
	return m_Channels[0]->Is_Inputable_Front(m_fTrackPosition, _iIndexFront);
}

_bool CAnimation::Is_UpperBody(_int iBoneIndex)
{
	return (iBoneIndex < 475 && iBoneIndex >= 301) || (iBoneIndex <= 10 && iBoneIndex >= 8);
}

_float4x4* CAnimation::Get_TransformationBoneMatrices(_float fTrackPosition, _float4x4* pMatrix)
{
	
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		_uint iBoneIndex = m_Channels[i]->Get_BoneIndex();
		pMatrix[iBoneIndex] = m_Channels[i]->Get_NowMatrix(fTrackPosition);
	}

	return pMatrix;
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
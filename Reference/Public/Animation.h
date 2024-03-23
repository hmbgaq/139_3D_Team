#pragma once

#include "Model.h"
#include "MyAIAnimation.h"

/* 특정 애니메이션(대기, 걷기, 뛰기, 때리기, 맞기) 을 표현하기위한 데이터들을 가진다. */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(CMyAIAnimation pAIAnimation, const CModel::BONES& Bones);
	_bool	Invalidate_TransformationMatrix(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones, _bool _bIsSplitted = false);
	_bool	Invalidate_TransformationMatrix_Upper(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones, _float2 vMouseMove);

	_bool Invalidate_TransformationMatrix_Parasiter(CModel::ANIM_STATE _eAnimState, _float fTimeDelta, const CModel::BONES& Bones, _float3 pPlayerPos);


public:
	vector<class CChannel*>* Get_Channels() { return &m_Channels; };
	CChannel* Get_Channel_By_BoneIndex(_uint _iBoneIndex, _uint& _iChannelIndex);
	void	Reset_Animation(const CModel::BONES& Bones, _bool _bIsSplitted = false);
	void	Reset_UpperAnimation(const CModel::BONES& Bones);


	_float	Get_TrackPosition() { return m_fTrackPosition; };
	void	Set_TrackPosition(_float _fTrackPosition) { m_fTrackPosition = _fTrackPosition; };

	void	Set_Transition(CAnimation* prevAnimation, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0, _bool _bIsSplitted = false);
	_bool	Is_Transition_End() { return m_fTransitionEnd <= m_fTrackPosition; }
	_bool	Is_Transition() { return m_bIsTransition; }

	void	Set_Transition_Upper(CAnimation* prevAnimation, _float _fTransitionDuration = 0.2f, _uint iTargetKeyFrameIndex = 0);


	KEYFRAME Make_NowFrame(_uint m_iChannelIndex);

	void	Reset_TransitionKeyFrame() {
		m_StartTransitionKeyFrame.clear();
		m_EndTransitionKeyFrame.clear();
	};

	_float3 Get_PrevPos() {
		return m_PrevPos;
	}

	void	Set_PrevPos(_float3 _PrevPos) {
		m_PrevPos = _PrevPos;
	}

	_float	Get_TickPerSecond() {
		return m_fTickPerSecond;
	}

	void	Set_TickPerSecond(_float _fTickPerSecond) { m_fTickPerSecond = _fTickPerSecond; }

	_bool	Is_TransitionEnd_Now() {
		return m_bIsTransitionEnd_Now;
	}

	_bool	Is_Inputable_Front(_uint _iIndexFront);

	_bool Is_Inputable_Front(_float _fTrackPosition, _uint _iIndexFront);

	void Set_StiffnessRate(_float fStiffnessRate) { m_fStiffnessRate = fStiffnessRate; }
	_float Get_StiffnessRate() { return m_fStiffnessRate; }

	_char* Get_Name() {
		return m_szName;
	}

	_float Get_Duration() { return m_fDuration; }


	_bool Is_UpperBody(_int iBoneIndex);

public:
	_uint Get_CurrentKeyFrames(_uint iIndex = 0) { return m_CurrentKeyFrames[iIndex]; };



public: //!For. AnimInstance
	_float4x4*				Get_TransformationBoneMatrices(_float fTrackPosition, _float4x4* pMatrix);

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f };		/* 내 애니메이션을 전체 재생하기위한 전체 길이. */
	_float					m_fTickPerSecond = { 0.f }; /* 애니메이션의 재생 속도 : m_TickPerSecond * fTimeDelta */
	_float					m_fTrackPosition = { 0.f }; /* 현재 재생되고 있는 위치. */

	_uint					m_iNumChannels = { 0 }; /* 이 애니메이션이 사용하는 뼈의 갯수. */
	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_CurrentKeyFrames;
	_bool					m_isFinished = { false };

	_bool					m_bIsTransition = { false };
	_bool					m_bIsTransitionEnd_Now = { false };
	_float					m_fTransitionEnd = { 0.f };

	vector<KEYFRAME>		m_StartTransitionKeyFrame;
	vector<KEYFRAME>		m_EndTransitionKeyFrame;

	_float3					m_PrevPos = { 0.f, 0.f, 0.f };
	_float					m_fStiffnessRate = { 1.f };

	/*! "Stiffness Rate"은 물리학이나 엔지니어링 분야에서 사용되는 용어로, 물체가 변형될 때 얼마나 강하게 반발하는지를 나타내는 지표입니다. 일반적으로 강도와 관련이 있으며, 변형된 물체의 탄성을 측정하는 데 사용	*/



public:
	static CAnimation* Create(CMyAIAnimation pAIAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END
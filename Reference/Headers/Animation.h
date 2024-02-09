#pragma once

#include "Model.h"

/* 특정 애니메이션(대기, 걷기, 뛰기, 때리기, 맞기) 을 표현하기위한 데이터들을 가진다. */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones);

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f }; /* 내 애니메이션을 전체 재생하기위한 전체 길이. */
	_float					m_fTickPerSecond = { 0.f }; /* 애니메이션의 재생 속도 : m_TickPerSecond * fTimeDelta */
	_float					m_fTrackPosition = { 0.f }; /* 현재 재생되고 있는 위치. */

	_uint					m_iNumChannels = { 0 }; /* 이 애니메이션이 사용하는 뼈의 갯수. */
	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_CurrentKeyFrames;
	_bool					m_isFinished = { false };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END
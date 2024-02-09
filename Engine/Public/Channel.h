#pragma once

#include "Model.h"

/* 이 애니메이션에서 사용하는 뼈의 정보다. */
/* 시간대에 따른 뼈의 상태(Scale, Rotation, Position)를 저장한다.*/

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	/*_uint				m_iCurrentKeyFrameIndex = { 0 };*/
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END
#pragma once

#include "Model.h"
#include "MyAINodeAnimation.h"

/* 이 애니메이션에서 사용하는 뼈의 정보다. */
/* 시간대에 따른 뼈의 상태(Scale, Rotation, Position)를 저장한다.*/

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CMyAINodeAnimation pChannel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);
	void Invalidate_TransformationMatrix_Normal(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);
	void Invalidate_TransformationMatrix_Reverse(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);
	void Invalidate_TransformationMatrix_Transition(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fCurrentTrackPosition, const CModel::BONES& Bones);

public:
	_uint		Get_BoneIndex() { return m_iBoneIndex; };
	void		Reset_Channel(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);
	KEYFRAME	Get_KeyFrame(_uint _iKeyFrameIndex) {
		if (_iKeyFrameIndex >= m_iNumKeyFrames - 1)
			return m_KeyFrames.back();
		return m_KeyFrames[_iKeyFrameIndex];
	};


	_bool		Is_Inputable(_float fCurrentTrackPosition, _uint _iIndex);
	_bool		Is_Inputable_Front(_float fCurrentTrackPosition, _uint _iIndexFront);

public:
	_bool	Update_KeyFrame(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);
	_float	Calc_Ratio(_float fStart, _float fNow, _float	fEnd);
	void	Calc_Now_Transform(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _vector& vScale, _vector& vRotation, _vector& vPosition, _float fRatio);
	_matrix	Make_TransformationMatrix(KEYFRAME& _StartFrame, KEYFRAME& _EndFrame, _float fRatio);
	_matrix Make_SelectedFrame_TransformationMatrix(_uint iIndex);
	_matrix Make_StartFrame_TransformationMatrix();
	_matrix Make_EndFrame_TransformationMatrix();

	KEYFRAME Make_NowFrame(_float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

	_float4x4& Get_NowMatrix(_float fTrackPosition);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	
	
	/*_uint				m_iCurrentKeyFrameIndex = { 0 };*/
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(CMyAINodeAnimation pChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END
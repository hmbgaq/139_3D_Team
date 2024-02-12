#pragma once

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"

/* 원형 컴포넌트들을 레벨별로 보관한다. */
/* 복제하고하는 원형을 찾아 복제하여 리턴한다. */

BEGIN(Engine)

class CComponent;

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent> pPrototype);
	shared_ptr<CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint											m_iNumLevels = { 0 };

	map<const wstring, shared_ptr<CComponent>>*			m_pPrototypes = { nullptr };
	typedef map<const wstring, shared_ptr<CComponent>>	PROTOTYPES;

private:
	shared_ptr<CComponent> Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END
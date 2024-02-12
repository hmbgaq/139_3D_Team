#pragma once

#include "Component.h"

BEGIN(Engine)

class CCell;
class CShader;

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int		iCurrentIndex;
	}NAVI_DESC;
public:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;	

#ifdef _DEBUG
	virtual HRESULT Render();
#endif

public:
	void Update(_fmatrix WorldMatrix);
	_bool isMove(_fvector vPosition);

private:
	vector<shared_ptr<CCell>>			m_Cells;
	static _float4x4				m_WorldMatrix;
	_int							m_iCurrentIndex = { -1 };

#ifdef _DEBUG
private:
	shared_ptr<CShader>					m_pShader = { nullptr };
#endif

private:
	HRESULT Make_Neighbors();

public:
	static shared_ptr<CNavigation> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationFilePath);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END
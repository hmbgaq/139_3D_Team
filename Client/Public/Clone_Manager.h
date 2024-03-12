#pragma once
#include "Client_Defines.h"
#include "Engine_Macro.h"
#include "Base.h"
#include "GameInstance.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CClone_Manager : public CBase
{
	DECLARE_SINGLETON(CClone_Manager);

private:
	CClone_Manager();
	virtual ~CClone_Manager() = default;

public:
	HRESULT		Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:	// 생성 함수 작성
	template<typename T>
	T* Clone_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag,  void* pArg = nullptr)
	{
		CGameObject* pClonedObject = m_pGameInstance->Add_CloneObject_And_Get(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
		T* pResult = dynamic_cast<T*>(pClonedObject);
		return pResult;
	};

	template<typename T>
	T* Clone_Object(const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr)
	{
		return Clone_Object(m_pGameInstance->Get_NextLevel(), strLayerTag, strPrototypeTag, pArg);
	};



	template<typename T>
	T* Clone_Component() 
	{
	};



private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


public:
	virtual void	Free();
};

END


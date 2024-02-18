//#pragma once
//
//#include "GameObject.h"
//
//BEGIN(Engine)
//
//class CEventActor;
//
//class ENGINE_DLL CTrigger abstract : public CGameObject, public IEvent
//{
//protected:
//	CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
//	CTrigger(const CTrigger& rhs);
//	virtual ~CTrigger() = default;
//
//public:
//	HRESULT Initialize(vector<CEventActor*> _Actors);
//
//public:
//	virtual void Activate() override;
//
//protected:
//	vector<CEventActor*> m_Actors = { nullptr };
//
//
//protected:
//	virtual void Free() override;
//};
//
//END
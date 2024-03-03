#pragma once
#include "Base.h"

BEGIN(Engine)

class CustomContextManagerCallback : public DxContextManagerCallback
{
public:
	CustomContextManagerCallback(ID3D11Device* pDevice);

	// DxContextManagerCallback을(를) 통해 상속됨
	virtual void acquireContext() override;
	virtual void releaseContext() override;
	virtual ID3D11Device* getDevice() const override;
	virtual ID3D11DeviceContext* getContext() const override;
	virtual bool synchronizeResources() const override;
public:
	std::recursive_mutex mMutex;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
	bool mSynchronizeResources;
};

END
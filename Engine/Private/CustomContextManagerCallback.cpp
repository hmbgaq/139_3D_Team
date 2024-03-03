#include "CustomContextManagerCallback.h"

CustomContextManagerCallback::CustomContextManagerCallback(ID3D11Device* pDevice)
{
	mDevice = pDevice;
}

void CustomContextManagerCallback::acquireContext()
{
	mMutex.lock();
}

void CustomContextManagerCallback::releaseContext()
{
	mMutex.unlock();
}

ID3D11Device* CustomContextManagerCallback::getDevice() const
{
	return mDevice;
}

ID3D11DeviceContext* CustomContextManagerCallback::getContext() const
{
	return mContext;
}

bool CustomContextManagerCallback::synchronizeResources() const
{
	return mSynchronizeResources;
}

#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIQuatKey
{
public:
	CMyAIQuatKey() {};
	CMyAIQuatKey(aiQuatKey _pQuatKey) { 
		//m_pQuatKey = _pQuatKey;

		x = _pQuatKey.mValue.x;
		y = _pQuatKey.mValue.y;
		z = _pQuatKey.mValue.z;
		w = _pQuatKey.mValue.w;

		mTime = _pQuatKey.mTime;
	};
	
	CMyAIQuatKey(ifstream& is) {

		read_typed_data(is, x);
		read_typed_data(is, y);
		read_typed_data(is, z);
		read_typed_data(is, w);

		read_typed_data(is, mTime);
	}

private:
	//aiQuatKey m_pQuatKey;

	_float x = 0;
	_float y = 0;
	_float z = 0;
	_float w = 0;
	_float mTime = 0;

public:
	void Bake_Binary(ofstream& os)
	{
		write_typed_data(os, x);
		write_typed_data(os, y);
		write_typed_data(os, z);
		write_typed_data(os, w);

		write_typed_data(os, mTime);
	}

	_float Get_X() { return x; }
	_float Get_Y() { return y; }
	_float Get_Z() { return z; }
	_float Get_W() { return w; }

	_float Get_Time() { return mTime; }


};

END
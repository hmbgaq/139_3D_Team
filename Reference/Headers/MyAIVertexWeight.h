#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyAIVertexWeight
{
public:
	CMyAIVertexWeight() {};
	CMyAIVertexWeight(aiVertexWeight _Weight) {
		//mWeight = _Weight;
		mVertexId = _Weight.mVertexId;
		mWeight = _Weight.mWeight;
	};

	CMyAIVertexWeight(ifstream& is) {
		read_typed_data(is, mVertexId);
		read_typed_data(is, mWeight);
	}

private:
	//aiVertexWeight mWeight;

	_uint	mVertexId = 0;
	_float	mWeight = 0.f;

public:
	void Bake_Binary(ofstream& os) 
	{
		write_typed_data(os, mVertexId);
		write_typed_data(os, mWeight);
	}

	_uint Get_VertexId() { return mVertexId; }
	_float Get_Weight() { return mWeight; }




};

END
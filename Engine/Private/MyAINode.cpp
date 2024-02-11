#include "stdafx.h"
#include "MyAINode.h"

CMyAINode::CMyAINode(aiNode* _pNode)
{ 
	mNumChildren = _pNode->mNumChildren;
	mName = _pNode->mName.data;

	memcpy(&mTransformation, &_pNode->mTransformation, sizeof(_float4x4));

	for (_uint i = 0; i < mNumChildren; ++i)
	{
		mChildren.push_back(CMyAINode(_pNode->mChildren[i]));
	}

}

CMyAINode::CMyAINode(ifstream& is)
{
	size_t istringSize;
	read_typed_data(is, istringSize);
	mName.resize(istringSize);
	is.read(&mName[0], istringSize);

	read_typed_data(is, mNumChildren);
	read_typed_data(is, mTransformation);

	for (_uint i = 0; i < mNumChildren; ++i)
	{
		mChildren.push_back(CMyAINode(is));
	}
}

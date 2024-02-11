#include "stdafx.h"
#include "MyAIMesh.h"

CMyAIMesh::CMyAIMesh(aiMesh* _pMesh) 
{
	//m_pMesh = _pMesh; 

	mMaterialIndex = _pMesh->mMaterialIndex;
	mName = _pMesh->mName.data;
	mNumVertices = _pMesh->mNumVertices;
	mNumFaces = _pMesh->mNumFaces;
	mNumBones = _pMesh->mNumBones;

	for (_uint i = 0; i < mNumVertices; ++i) 
	{
		_float3 _pVertices;
		memcpy(&_pVertices, &_pMesh->mVertices[i], sizeof(_float3));
		mVertices.push_back(_pVertices);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _pNormal;
		memcpy(&_pNormal, &_pMesh->mNormals[i], sizeof(_float3));
		mNormals.push_back(_pNormal);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _TextureCoords;
		memcpy(&_TextureCoords, &_pMesh->mTextureCoords[0][i], sizeof(_float3));
		mTextureCoords[0].push_back(_TextureCoords);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _Tangents;
		memcpy(&_Tangents, &_pMesh->mTangents[i], sizeof(_float3));
		mTangents.push_back(_Tangents);
	}

	for (_uint i = 0; i < mNumFaces; ++i) 
	{
		mFaces.push_back(CMyAIFace(_pMesh->mFaces[i]));
	}


	for (_uint i = 0; i < mNumBones; ++i) 
	{
		mBones.push_back(CMyAIBone(_pMesh->mBones[i]));
	}

}
CMyAIMesh::CMyAIMesh(ifstream& is)
{
	size_t istringSize;
	read_typed_data(is, istringSize);
	mName.resize(istringSize);
	is.read(&mName[0], istringSize);

	read_typed_data(is, mMaterialIndex);

	read_typed_data(is, mNumVertices);
	read_typed_data(is, mNumFaces);
	read_typed_data(is, mNumBones);


	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _Vertice;
		read_typed_data(is, _Vertice);

		mVertices.push_back(_Vertice);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _Normal;
		read_typed_data(is, _Normal);

		mNormals.push_back(_Normal);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _TextureCoord;
		read_typed_data(is, _TextureCoord);

		mTextureCoords[0].push_back(_TextureCoord);
	}

	for (_uint i = 0; i < mNumVertices; ++i)
	{
		_float3 _Tangent;
		read_typed_data(is, _Tangent);

		mTangents.push_back(_Tangent);
	}


	for (_uint i = 0; i < mNumFaces; ++i)
	{
		mFaces.push_back(CMyAIFace(is));
	}

	for (_uint i = 0; i < mNumBones; ++i)
	{
		mBones.push_back(CMyAIBone(is));
	}

}

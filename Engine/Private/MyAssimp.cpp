#include "stdafx.h"
#include "..\Public\MyAssimp.h"


CMyAIScene CMyAssimp::ReadFile(const string& strModelFilePath, _uint iFlag, _bool _bIsBinary)
{
	string filePath = strModelFilePath;
	string fileType;

	if (false == _bIsBinary)
	{
		fileType = ".fbx";
		filePath += fileType;

		const aiScene* pAIScene = m_Importer.ReadFile(filePath, iFlag);
		CMyAIScene result = CMyAIScene(pAIScene);
		result.Bake_Binary(strModelFilePath);
		return result;
	}
	else 
	{
		fileType = ".bin";
		filePath += fileType;

		return CMyAIScene(filePath);
	}
}

CMyAIScene CMyAssimp::ReadFile(const string& strModelFilePath, _uint iFlag)
{
	string filePath = strModelFilePath + ".bin";

	ifstream is(filePath, std::ios::binary);

	_bool IsBinFileOpen = is.is_open();
	is.close();

	return ReadFile(strModelFilePath, iFlag, IsBinFileOpen);

}

void CMyAssimp::FreeScene()
{
	m_Importer.FreeScene();
}

#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	Ready_Sound();

	return S_OK;
}

HRESULT CSound_Manager::Ready_Sound()
{
	FMOD_RESULT	result;

	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	result = FMOD_System_SetOutput(m_pSystem, FMOD_OUTPUTTYPE_ALSA);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	result = FMOD_System_CreateChannelGroup(m_pSystem, "BGM_Group", &m_pChannelGroup[BGM_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND1_Group", &m_pChannelGroup[SND1_GROUP]);
	result = FMOD_System_CreateChannelGroup(m_pSystem, "SND2_Group", &m_pChannelGroup[SND2_GROUP]);

	result = FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterChanelGroup);

	/* For. BGM */
	Load_SoundFile_GroupAsync(L"BGM", "../Bin/Resources/Sound/BGM/");

	Load_SoundFile_GroupAsync(L"BGM_LOADING", "../Bin/Resources/Sound/LoadingBGM/");
	

	//Wait_GroupAsync();

	Load_SoundFile_GroupAsync(L"AMBIENCE", "../Bin/Resources/Sound/Ambience/");
	//Wait_GroupAsync();

	Load_SoundFile_GroupAsync(L"PLAYER_ATTACK", "../Bin/Resources/Sound/Player/Attack/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"PLAYER_HIT", "../Bin/Resources/Sound/Player/Hit/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"PLAYER_HIT2", "../Bin/Resources/Sound/Player/Hit/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"PLAYER_FOOTSTEP", "../Bin/Resources/Sound/Player/Footstep/");
	////Wait_GroupAsync();

	Load_SoundFile_GroupAsync(L"ENEMY_ATTACK", "../Bin/Resources/Sound/Attack/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"ENEMY_HIT", "../Bin/Resources/Sound/Hit/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"ENEMY_HIT2", "../Bin/Resources/Sound/Hit/");
	////Wait_GroupAsync();
	Load_SoundFile_GroupAsync(L"ENEMY_FOOTSTEP", "../Bin/Resources/Sound/Footstep/");
	////Wait_GroupAsync();

	Load_SoundFile_GroupAsync(L"CRITICAL", "../Bin/Resources/Sound/Hit/");


	Load_SoundFile_GroupAsync(L"SNIPER_ATTACKLONG", "../Bin/Resources/Sound/Enemy/Sniper/Attack/LongDistance/");
	Load_SoundFile_GroupAsync(L"SNIPER_ATTACKSHORT", "../Bin/Resources/Sound/Enemy/Sniper/Attack/ShotDistance/");
	Load_SoundFile_GroupAsync(L"SNIPER_DEATH", "../Bin/Resources/Sound/Enemy/Sniper/Death/");
	Load_SoundFile_GroupAsync(L"SNIPER_HIT", "../Bin/Resources/Sound/Enemy/Sniper/Hit/");
	Load_SoundFile_GroupAsync(L"SNIPER_RELOAD", "../Bin/Resources/Sound/Enemy/Sniper/Reload/");

	Load_SoundFile_GroupAsync(L"ZENU_ATTACK", "../Bin/Resources/Sound/Enemy/ZenuGiant/Attack/LongDistance/");
	Load_SoundFile_GroupAsync(L"ZENU_THROWAXE", "../Bin/Resources/Sound/Enemy/ZenuGiant/AxeThrowProjectTile/");
	Load_SoundFile_GroupAsync(L"ZENU_BODYMOVE", "../Bin/Resources/Sound/Enemy/ZenuGiant/BodyMove/");
	Load_SoundFile_GroupAsync(L"ZENU_ATTACK", "../Bin/Resources/Sound/Enemy/ZenuGiant/Hit/");
	Load_SoundFile_GroupAsync(L"ZENU_DEATH", "../Bin/Resources/Sound/Enemy/ZenuGiant/Death/");
	Load_SoundFile_GroupAsync(L"ZENU_FOOTSTEP", "../Bin/Resources/Sound/Enemy/ZenuGiant/FootStep/");
	Load_SoundFile_GroupAsync(L"ZENU_HIT", "../Bin/Resources/Sound/Enemy/ZenuGiant/Hit/");
	Load_SoundFile_GroupAsync(L"ZENU_SPAWN", "../Bin/Resources/Sound/Enemy/ZenuGiant/Spawn/");
	


	return S_OK;
}

void CSound_Manager::Play_Sound(const wstring& strGroupKey, const wstring& strSoundKey, CHANNELID eID, _float fVolume)
{
	// 카테고리 키
	auto iterCate = m_mapSound.find(strGroupKey);
	if (iterCate == m_mapSound.end())
		return;

	// 사운드 키
	auto iter = iterCate->second->Get_MapSound().find(strSoundKey);


	// 여기부터 사운드 플레이
	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}
	else
	{
		FMOD_Channel_Stop(m_pChannelArr[eID]);
		FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SND1_GROUP], FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);//fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Play_BGM(const wstring& strGroupKey, const wstring& strSoundKey, _float fVolume)
{
	// 카테고리 키
	auto iterCate = m_mapSound.find(strGroupKey);
	if (iterCate == m_mapSound.end())
		return;

	// 사운드 키
	auto iter = iterCate->second->Get_MapSound().find(strSoundKey);
	if (iter == iterCate->second->Get_MapSound().end())
		return;


	// 여기부터 사운드 플레이
	Stop_Sound(SOUND_BGM);
	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[BGM_GROUP], FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);//fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Stop_Sound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::Stop_All()
{
	for (int i = 0; i < static_cast<int>(MAXCHANNEL); ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager::Set_ChannelVolume(CHANNELID eID, _float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Load_SoundFile(const wstring& pGroupKey, const string& pPath)
{
	// m_mapSound 보호
	m_mtxSound.lock();
	// 여기는 카테고리 키 만들기, 없으면 키를 만들고 컨테이너를 만들어 준다.
	auto iter = m_mapSound.find(pGroupKey);
	if (iter == m_mapSound.end())
	{
		m_mapSound.emplace(pGroupKey, FSoundData::Create());
	}
	m_mtxSound.unlock();

	// 여기부터 파일 로드부
	_char sText[128] = "";
	strcpy_s(sText, pPath.c_str());
	strcat_s(sText, "*.*");

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	auto handle = _findfirst(sText, &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "";
	char szFullPath[128] = "";

	strcpy_s(szCurPath, pPath.c_str());

	// 이게 뭐게 ㅋ
	using sound_tuple = tuple<FMOD_SOUND*, FMOD_RESULT, string, string>;
	enum ETMP_SOUND : int {
		TMP_SOUND,
		TMP_RESULT,
		TMP_PATH,
		TMP_FILE_NAME
	};

	// 비동기 처리는 쓰레드를 사용하기 때문에 모든 데이터를 별도의 컨테이너에 저장하고 처리해줍니다.
	vector<future<FMOD_RESULT>> vecAsync;
	vector<sound_tuple> vecSoundData;

	// 데이터 입력부
	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/Success.wav"
		strcat_s(szFullPath, fd.name);
		sound_tuple tpSound;

		// 쓰레드에 넘겨줄 데이터를 입력
		get<TMP_PATH>(tpSound) = string(szFullPath);
		get<TMP_FILE_NAME>(tpSound) = string(fd.name);
		get<TMP_RESULT>(tpSound) = FMOD_OK;
		get<TMP_SOUND>(tpSound) = nullptr;

		vecSoundData.push_back(tpSound);

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	// 쓰레드 실행
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync.push_back(async(launch::async, &CSound_Manager::LoadSoundFile_Async, this
			, get<TMP_PATH>(vecSoundData[i]).c_str(), get<TMP_FILE_NAME>(vecSoundData[i]).c_str()
			, ref(get<TMP_RESULT>(vecSoundData[i])), &get<TMP_SOUND>(vecSoundData[i])));
	}

	// 결과 받아오기
	for (size_t i = 0; i < vecSoundData.size(); i++)
	{
		vecAsync[i].get();

		m_mtxSound.lock();
		if (get<TMP_RESULT>(vecSoundData[i]) == FMOD_OK)
		{
			int iLength = (int)strlen(get<TMP_FILE_NAME>(vecSoundData[i]).c_str()) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, get<TMP_FILE_NAME>(vecSoundData[i]).c_str(), iLength, pSoundKey, iLength);

			m_mapSound[pGroupKey]->Get_MapSound().emplace(pSoundKey, get<TMP_SOUND>(vecSoundData[i]));

			Safe_Delete_Array(pSoundKey);
		}
		m_mtxSound.unlock();
	}

	// FMOD 업데이트
	FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

void CSound_Manager::Load_SoundFile_GroupAsync(const wstring& pGroupKey, const string& pPath)
{
	m_vecAsyncSoundGroup.push_back(async(launch::async, &CSound_Manager::Load_SoundFile, this, pGroupKey, pPath));
}

void CSound_Manager::Wait_GroupAsync()
{
	for (auto iter = m_vecAsyncSoundGroup.begin(); iter != m_vecAsyncSoundGroup.end();)
	{
		auto& Future = (*iter);
		auto state = Future.wait_for(1ms);
		if (state == future_status::ready)
		{
			Future.get();
			iter = m_vecAsyncSoundGroup.erase(iter);

			// 처음으로 돌아가기
			if (iter == m_vecAsyncSoundGroup.end())
			{
				iter = m_vecAsyncSoundGroup.begin();
				continue;
			}
		}
		else if (state == future_status::timeout)
		{
			iter = m_vecAsyncSoundGroup.erase(iter);
			continue;
		}

		if (++iter == m_vecAsyncSoundGroup.end())
			iter = m_vecAsyncSoundGroup.begin();
	}
	m_vecAsyncSoundGroup.clear();
}

FMOD_RESULT CSound_Manager::LoadSoundFile_Async(const string& pPath, const string& pFileName, FMOD_RESULT& hResult, FMOD_SOUND** pSound)
{
	hResult = FMOD_System_CreateSound(m_pSystem, pPath.c_str(), FMOD_DEFAULT, NULL, pSound);

	return hResult;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& pairSoundCtn : m_mapSound)
	{
		for (auto& pairSound : pairSoundCtn.second->Get_MapSound())
		{
			FMOD_Sound_Release(pairSound.second);
		}
		Safe_Release(pairSoundCtn.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}

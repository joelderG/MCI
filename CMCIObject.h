#pragma once
class CMCIObject
{
public:
	CMCIObject();
	~CMCIObject();
	bool OpenFile(LPCWSTR pszFileName);
	bool OpenAudioCD(LPCWSTR drive, BYTE& tracks);
	void Close();
	void Play();
	void Pause();
	void Stop();

	bool SetVideoPosition(HWND hwnd, CRect rect);
	bool GetTMSFPosition(BYTE& track, BYTE& min, BYTE& sek, BYTE& frame);
	bool GetTrackLength(BYTE track, BYTE& min, BYTE& sek, BYTE& frame);
	bool TMSFSeek(BYTE track, BYTE min, BYTE sek, BYTE frame);
	int CMCIObject::GetPlayLength(BYTE track);

private:
	void MCIError();
	MCI_OPEN_PARMS m_op;
	DWORD m_result;
};


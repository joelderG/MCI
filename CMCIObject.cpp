#include "pch.h"
#include "CMCIObject.h"

CMCIObject::CMCIObject() {
	m_op.dwCallback = 0;
	m_op.wDeviceID = 0;
	m_op.lpstrDeviceType = 0;
	m_op.lpstrElementName = 0;
	m_op.lpstrAlias = 0;
	m_result = 0;
}

CMCIObject::~CMCIObject() {
	if (m_op.wDeviceID != 0) {
		Close();
	}
}

bool CMCIObject::OpenFile(LPCWSTR pszFileName) {
	if (m_op.wDeviceID != 0) Close();
	m_op.lpstrDeviceType = 0;
	m_op.lpstrElementName = pszFileName;
	if ((m_result = mciSendCommand(0,
		MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD_PTR)&m_op)) != 0) {
		MCIError();
		return false;
	}
	
	MCI_SET_PARMS t;
	t.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&t)) != 0) {
		MCIError();
		return false;
	}
	return true;
}

bool CMCIObject::OpenAudioCD(LPCWSTR drive, BYTE& tracks) {
	DWORD flag = 0;
	if (m_op.wDeviceID != 0) Close();
	m_op.lpstrDeviceType = (LPCWSTR)MCI_DEVTYPE_CD_AUDIO; // dirty!
	if (m_op.lpstrElementName = drive)
		flag = MCI_OPEN_ELEMENT;
	if ((m_result = mciSendCommand(0,
		MCI_OPEN, MCI_OPEN_TYPE_ID | flag |
		MCI_WAIT | MCI_OPEN_TYPE, (DWORD_PTR)&m_op)) != 0) {
			MCIError();
			return false;
	}

	MCI_SET_PARMS t;
	t.dwTimeFormat = MCI_FORMAT_TMSF;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&t)) != 0) {
		MCIError();
		return false;
	}

	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0) {
		MCIError();
		return false;
	}
	tracks = (BYTE)status.dwReturn;
	return true;
}

bool CMCIObject::SetVideoPosition(HWND hwnd, CRect rect) {
	MCI_ANIM_WINDOW_PARMS wparms;
	MCI_ANIM_RECT_PARMS where;

	wparms.hWnd = hwnd; // set destination window
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_WINDOW, MCI_ANIM_WINDOW_HWND, (DWORD_PTR)&wparms)) != 0) {
		MCIError();
		return false;
	}
	where.rc = rect; // set destination rect
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_PUT, MCI_ANIM_RECT | MCI_ANIM_PUT_DESTINATION,
		(DWORD_PTR) & where)) != 0) {
		MCIError();
		return false;
	}
	return true;
}

bool CMCIObject::TMSFSeek(BYTE track, BYTE min, BYTE sek, BYTE frame) {
	if (m_op.wDeviceID == 0) return 0; // Not open
	MCI_STATUS_PARMS status;			// ask for current time format
	status.dwItem = MCI_STATUS_TIME_FORMAT;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM,
		(DWORD_PTR)&status)) != 0) {
		return false;
	}
	Stop();	// Just in Case
	MCI_SEEK_PARMS s;
	switch (status.dwReturn) {
	case MCI_FORMAT_TMSF:
		s.dwTo = 0;
		s.dwTo |= frame; s.dwTo <<= 8;
		s.dwTo |= sek; s.dwTo <<= 8;
		s.dwTo |= min; s.dwTo <<= 8;
		s.dwTo |= track;
		break;
	case MCI_FORMAT_MILLISECONDS:
		s.dwTo = (sek + min * 60) * 1000;
	}
	if (m_result = mciSendCommand(m_op.wDeviceID,
		MCI_SEEK, MCI_TO | MCI_WAIT, (DWORD_PTR)(LPVOID)&s) != 0) {
		MCIError();
		return false;
	}
	return true;
}

bool CMCIObject::GetTrackLength(BYTE track, BYTE& min, BYTE& sek, BYTE& frame) {
	if (m_op.wDeviceID == 0) return false; // Not open
	MCI_STATUS_PARMS length;
	length.dwTrack = track;
	length.dwItem = MCI_STATUS_LENGTH;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
		(DWORD_PTR)(LPMCI_STATUS_PARMS)&length)) != 0) {
		MCIError();
		return false;
	}

	m_result = length.dwReturn & 0x000000FF; min = (BYTE)m_result;
	m_result = length.dwReturn & 0x0000FF00; sek = (BYTE)(m_result >> 8);
	m_result = length.dwReturn & 0x00FF0000; frame = (BYTE)(m_result >> 16);
	return true;
}

bool CMCIObject::GetTMSFPosition(BYTE& track, BYTE& min, BYTE& sek, BYTE& frame) {
	track = min = sek = frame = 0;
	if (m_op.wDeviceID == 0) return false;

	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_TIME_FORMAT;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0)
		return false;

	MCI_STATUS_PARMS pos;
	pos.dwItem = MCI_STATUS_POSITION;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&pos)) != 0)
		return false;

	switch (status.dwReturn) {
	case MCI_FORMAT_TMSF:
		m_result = pos.dwReturn & 0x000000FF; track = (BYTE)m_result;
		m_result = pos.dwReturn & 0x0000FF00; min = (BYTE)(m_result >> 8);
		m_result = pos.dwReturn & 0x00FF0000; sek = (BYTE)(m_result >> 16);
		m_result = pos.dwReturn & 0xFF000000; frame = (BYTE)(m_result >> 24);
		return true;
	case MCI_FORMAT_MILLISECONDS:
		sek = (BYTE) ((pos.dwReturn / 1000) % 60);
		min = (BYTE) ((pos.dwReturn / 1000) / 60);
		return true;
	}
	return false; // dont count not supported media
}


void CMCIObject::Close() {
	MCI_GENERIC_PARMS gp;
	if (m_op.wDeviceID != 0) {
		Stop(); // Just in Case
		if ((m_result = mciSendCommand(m_op.wDeviceID,
			MCI_CLOSE, MCI_WAIT, (DWORD_PTR)&gp)) != 0)
			MCIError();
		m_op.wDeviceID = 0;
	}
}

void CMCIObject::Play() {
	MCI_PLAY_PARMS play;
	if (m_op.wDeviceID == 0) return; // Not open
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_PLAY, 0, (DWORD_PTR)&play)) != 0)
		MCIError();
}

void CMCIObject::Stop() {
	if (m_op.wDeviceID == 0) return; // Not open
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STOP, MCI_WAIT, 0)) != 0)
		MCIError();
}

void CMCIObject::Pause() {
	if (m_op.wDeviceID == 0) return; // Not open
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_PAUSE, MCI_WAIT, 0)) != 0)
		MCIError();
}

void CMCIObject::MCIError() {
	char buf[512];
	buf[0] = '\0';
	mciGetErrorString(m_result, (LPWSTR)buf, sizeof(buf));
	if (!strlen(buf))
		strcpy_s(buf, "Unknown error");
	AfxMessageBox((LPCWSTR)buf);
	m_op.wDeviceID = 0;
}

bool CMCIObject::GetMediaLength(DWORD& totalms) {
	if (!m_op.wDeviceID == 0) return false;
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_LENGTH;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0) {
		MCIError();
		return false;
	}

	totalms = status.dwReturn;
	return true;
}

bool CMCIObject::GetTrackPercent(float& per) {
	BYTE t, m, s, f, full_min, full_s, full_f = 0;
	float total_s, current_s = 0.0f;
	DWORD totalMs, currentMs = 0;

	if (m_op.wDeviceID == 0) return false;

	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_TIME_FORMAT;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0) {
		return false;
	}

	switch (status.dwReturn) {
	case MCI_FORMAT_TMSF:
		if (!GetTMSFPosition(t, m, s, f)) {
			return false;
		}

		if (!GetTrackLength(t, full_min, full_s, full_f)) {
			return false;
		}

		total_s = full_min * 60 + full_s;
		current_s = m * 60 + s;
		if (total_s > 0) {
			per = (current_s / total_s) * 100.0f;
			return true;
		}
	case MCI_FORMAT_MILLISECONDS:
		if (!GetMediaLength(totalMs)) {
			return false;
		}

		MCI_STATUS_PARMS status;
		status.dwItem = MCI_STATUS_POSITION;
		if ((m_result = mciSendCommand(m_op.wDeviceID,
			MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0) {
			MCIError();
			return false;
		}

		currentMs = status.dwReturn;

		if (totalMs > 0) {
			per = (float)currentMs / (float)totalMs * 100.0f;
			return true;
		}
	}
	return false;
}

int CMCIObject::GetPlayLength(BYTE track) {
	if (m_op.wDeviceID == 0) return 0; // Nicht geöffnet
	int sec = 0;
	BYTE min, sek;
	MCI_STATUS_PARMS length;
	length.dwTrack = track; // Track setzen
	length.dwItem = MCI_STATUS_LENGTH;

	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_TIME_FORMAT;
	if ((m_result = mciSendCommand(m_op.wDeviceID,
		MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status)) != 0)
		return sec; // Fehler, Rückgabewert 0

	switch (status.dwReturn) {
	case MCI_FORMAT_TMSF:
		if ((m_result = mciSendCommand(m_op.wDeviceID,
			MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
			(DWORD_PTR)(LPMCI_STATUS_PARMS)&length)) != 0) {
			MCIError();
			return 0; // Fehler
		}
		// Zerlege length.dwReturn in Minuten, Sekunden und Frames
		min = (length.dwReturn >> 8) & 0xFF; // Minuten
		sek = (length.dwReturn >> 16) & 0xFF; // Sekunden
		// Berechne die gesamte Zeit in Sekunden
		return (min * 60) + sek; // Gesamtlänge in Sekunden

	case MCI_FORMAT_MILLISECONDS:
		if ((m_result = mciSendCommand(m_op.wDeviceID,
			MCI_STATUS, MCI_STATUS_ITEM,
			(DWORD_PTR)(LPMCI_STATUS_PARMS)&length)) != 0) {
			MCIError();
			return 0; // Fehler
		}
		// Gebe die Länge in Sekunden zurück
		return length.dwReturn / 1000; // Gesamtlänge in Sekunden
	}
	return sec; // Unbekanntes Format
}

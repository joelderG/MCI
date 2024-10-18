
// MCIDlg.h: Headerdatei
//

#pragma once
#include "CMCIObject.h"

// CMCIDlg-Dialogfeld
class CMCIDlg : public CDialogEx
{
// Konstruktion
public:
	CMCIDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MCI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;
	CMCIObject mci;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedVideo();
	afx_msg void OnBnClickedAudiomp3();
	afx_msg void OnBnClickedAudiomidi();
	afx_msg void OnBnClickedAudiocd();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedDestination();
	afx_msg void OnBnClickedClose();
	afx_msg void OnTrackListChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

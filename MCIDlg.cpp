
// MCIDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "MCI.h"
#include "MCIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMCIDlg-Dialogfeld



CMCIDlg::CMCIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MCI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMCIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMCIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Video, &CMCIDlg::OnBnClickedVideo)
	ON_BN_CLICKED(IDC_AudioMP3, &CMCIDlg::OnBnClickedAudiomp3)
	ON_BN_CLICKED(IDC_AudioMIDI, &CMCIDlg::OnBnClickedAudiomidi)
	ON_BN_CLICKED(IDC_AudioCD, &CMCIDlg::OnBnClickedAudiocd)
	ON_BN_CLICKED(IDC_Play, &CMCIDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Pause, &CMCIDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_Destination, &CMCIDlg::OnBnClickedDestination)
	ON_BN_CLICKED(IDC_Close, &CMCIDlg::OnBnClickedClose)
	ON_LBN_SELCHANGE(IDC_TRACKLIST, &CMCIDlg::OnTrackListChange)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMCIDlg-Meldungshandler

BOOL CMCIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen
	SetTimer(1, 200, 0); // start Timer

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CMCIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CMCIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMCIDlg::OnBnClickedVideo()
{
	/*mci.OpenFile(L"examples/Fish.mp4");*/
	CRect r;
	GetDlgItem(IDC_Destination)->GetWindowRect(r);
	ScreenToClient(r);
	mci.OpenFile(L"examples/video/test.mpg");
	mci.SetVideoPosition(GetSafeHwnd(),
		CRect(r.left, r.top, r.Width(), r.Height()));
	mci.Play();
}


void CMCIDlg::OnBnClickedAudiomp3()
{
	mci.OpenFile(L"examples/mp3/maus_castor_stereo.mp3");
	// mci.SetVideoPosition(GetSafeHwnd(), CRect(30, 60, 210, 140));
	mci.Play();
}


void CMCIDlg::OnBnClickedAudiomidi()
{
	mci.OpenFile(L"examples/mid/take5.mid");
	// mci.SetVideoPosition(GetSafeHwnd(), CRect(30, 60, 210, 140));
	mci.Play();
}


void CMCIDlg::OnBnClickedAudiocd()
{
	BYTE min, sek, frame, tracks;
	mci.OpenAudioCD(0, tracks);

	((CListBox*)GetDlgItem(IDC_TRACKLIST))->ResetContent();
	RedrawWindow();
	for (int i = 1; i <= tracks; i++) {
		mci.GetTrackLength(i, min, sek, frame);
		CString temp; // Entry in Listbox
		temp.Format(L"[%02d] %02d:%02d", i, min, sek);
		((CListBox*)GetDlgItem(IDC_TRACKLIST))->AddString(temp);
	}

	mci.TMSFSeek(1, 0, 0, 0);
	mci.Play();
}


void CMCIDlg::OnBnClickedPlay()
{
	mci.Play();
}


void CMCIDlg::OnBnClickedPause()
{
	mci.Pause();
}


void CMCIDlg::OnBnClickedDestination()
{

}


void CMCIDlg::OnBnClickedClose()
{
	mci.Close();
}


void CMCIDlg::OnTrackListChange()
{
	mci.TMSFSeek(((CListBox*)GetDlgItem(IDC_TRACKLIST))->GetCurSel() + 1, 0, 0, 0);
	mci.Play();
}


void CMCIDlg::OnTimer(UINT_PTR nIDEvent)
{
	int pos = 0;
	float percent= 0;
	CString str, str_pct, str_per;
	unsigned char t_p, m_p, s_p, f_p;
	mci.GetTMSFPosition(t_p, m_p, s_p, f_p);
	str.Format(L"[%02d] %02d:%02d", t_p, m_p, s_p);
	SetDlgItemText(IDC_TIME, str);

	mci.GetTrackPercent(percent);
	str_pct.Format(L"%.02f%%", percent);
	SetDlgItemText(IDC_TIMEPCT, str_pct);

	pos = (int)s_p;
	int gesamt_seconds = mci.GetPlayLength(t_p);
	if (gesamt_seconds == 0)
		gesamt_seconds = 1;
	else gesamt_seconds = mci.GetPlayLength(t_p);

	str_per.Format(L"%.02f%%", (((float)pos / gesamt_seconds) * 100));
	SetDlgItemText(IDC_TIMEPCT, str_per);

	CDialogEx::OnTimer(nIDEvent);
}

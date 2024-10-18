
// MCI.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CMCIApp:
// Siehe MCI.cpp für die Implementierung dieser Klasse
//

class CMCIApp : public CWinApp
{
public:
	CMCIApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CMCIApp theApp;

#ifndef _MAIN_APP_H
#define _MAIN_APP_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/event.h>
#endif  // precompiled headers

#include <wx/snglinst.h>

std::string getCurrentDateTimeMicrosecond();

class MainApp : public wxApp {
public:
	MainApp();
	~MainApp() {};

	bool OnInit(std::string&, bool, std::string&, std::string&, std::string&, std::string&, std::string&);
	int OnExit(std::string&);
	void OnInitCmdLine(wxCmdLineParser& parser);
	bool OnCmdLineParsed(wxCmdLineParser& parser);
	int GetLayerIndex(std::string&);

	bool UpdateFrameCanvas(std::string&, int, int, std::string&, std::string&, bool);	

	bool GetDBCreateResult() { return m_bDBCreateResult; }	
	std::string* GetIMGDirPath() { return &m_sIMGDirPath; }
#ifdef LINUX_CRASHRPT
	//! fatal exeption handling
	void OnFatalException();
#endif

#ifdef __WXMSW__
	//  Catch malloc/new fail exceptions
	//  All the rest will be caught be CrashRpt
	bool OnExceptionInMainLoop();
#endif

public:
	bool m_bDBCreateResult;
	std::string m_sIMGDirPath;
	wxSingleInstanceChecker* m_checker;
};

#endif

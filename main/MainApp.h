#ifndef _MAIN_APP_H
#define _MAIN_APP_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/event.h>
#endif  // precompiled headers

#include <wx/snglinst.h>

class MainApp : public wxApp {
public:
	MainApp();
	~MainApp() {};

	bool OnInit(std::string&, bool);
	void OnInitCmdLine(wxCmdLineParser& parser);
	bool OnCmdLineParsed(wxCmdLineParser& parser);
	int GetLayerIndex(std::string&);

	bool UpdateFrameCanvas(std::string&, int, int, std::string&, std::string&, bool);	

	bool GetDBCreateResult() { return m_bDBCreateResult; }
#ifdef LINUX_CRASHRPT
	//! fatal exeption handling
	void OnFatalException();
#endif

#ifdef __WXMSW__
	//  Catch malloc/new fail exceptions
	//  All the rest will be caught be CrashRpt
	bool OnExceptionInMainLoop();
#endif

	wxSingleInstanceChecker* m_checker;	

public:
	bool m_bDBCreateResult;
};

#endif

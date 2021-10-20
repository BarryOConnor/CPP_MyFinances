#pragma once
//using namespace std;
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include "MyFinancesDefinitions.h"
#include "Structs.h"
#include "BaseClass.h"
#include "WindowClass.h"
#include "EventManagerClass.h"



/*=========================================
APP MANAGER CLASS
===========================================*/
class ApplicationManagerClass : public EventManagerClass {
	HWND app_handle;
	HINSTANCE app_instance;
	WPARAM control_font;
	int current_user;
	COLORREF heading_background_color;
	WPARAM heading_font;
	COLORREF heading_text_color;
	COLORREF label_background_color;
	HMENU menu_account;
	HMENU menu_file;
	map< string, bool > transaction_map;


	void addMainMenu();
	void centerApplication();
	bool registerWindowClasses(HINSTANCE val_hinst, WNDPROC val_main_proc, WNDPROC val_child_proc);
	SizeStruct updateApplicationSize();
	void updateApplicationSize(SizeStruct);


public:
	bool build_finished;
	string current_sort;
	string date_from;
	string date_to;
	SortVarsStruct sorting;
	vector<WindowClass> windows;

	ApplicationManagerClass();

	HWND addWindow(WindowNames window_name, LPCWSTR window_class, HWND window_parent = NULL);
	void centerDialog();
	wstring convertStrings(const string & ref_string);
	string convertStrings(const wstring & ref_string);
	bool fileExists(const string & filename);	
	HWND getAppHandle();
	HINSTANCE getAppInstance();
	SizeStruct getAppSize();
	int getCurrentUser();
	WPARAM getControlFont();
	WPARAM getHeadingFont();
	COLORREF getHeadingTextColor();
	COLORREF getHeadingBackgroundColor();
	COLORREF getLabelBackgroundColor();
	INT_PTR handleLabelRepaint(HDC control_hdc, HWND control_handle);
	bool initialise(HINSTANCE val_hinst, WNDPROC val_main_proc, WNDPROC val_child_proc);
	bool isIncome(const string &trans_type);
	SYSTEMTIME makeSystemTime(const char *date);
	UINT msgbox(wstring message, MessageTypes icon = ICN_INFO);
	UINT msgbox(string message, MessageTypes icon = ICN_INFO);
	void msgbox(int message);
	void populateListview();
	void positionWindows();
	void setCurrentUser(int new_int);
	void updateMainMenu();
};
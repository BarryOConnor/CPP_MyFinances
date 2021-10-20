#pragma once
#include <Windows.h>
#include "MyFinancesDefinitions.h"
#include "EncryptionClass.h"
class EventManagerClass : public EncryptionClass{
	friend class ApplicationManagerClass;
public:

	void event_addNewTransaction();
	void event_attemptLogin(WindowNames current_window, WindowNames redirect_window);
	bool event_checkClose(HWND window_handle);
	void event_createAccount(WindowNames current_window, WindowNames redirect_window);
	void event_createCSV();
	void event_deleteAccount();
	void event_deleteSelectedTransaction();
	void event_editSelectedTransaction();
	void event_handleClick(HWND window_handle, WPARAM wParam, LPARAM lParam );
	LRESULT event_handleNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void event_importCSV();
	void event_logout(WindowNames redirect_window);
	void event_toggleAccount();
	void event_transitionWindows(WindowNames hide_this, WindowNames show_this);
	void event_showNewTransaction();
	void event_showSelectedTransaction();
	void event_updateAccount(WindowNames current_window, WindowNames redirect_window);
};


#pragma once
#include "stdafx.h"
#include <commctrl.h>
#include <map>

ApplicationManagerClass app_mgr;




int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	/*WinMain is the main program function*/

	INITCOMMONCONTROLSEX comm_ctrls;
	comm_ctrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	comm_ctrls.dwICC = ICC_STANDARD_CLASSES;

	app_mgr.initialise(hInst, (WNDPROC)MainWinProc, (WNDPROC)ChildWinProc);

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (IsDialogMessage(app_mgr.windows[WIN_LOGIN].getHandle(), &msg)
			|| IsDialogMessage(app_mgr.windows[WIN_ACCOUNT].getHandle(), &msg)
			|| IsDialogMessage(app_mgr.windows[WIN_BUDGET].getHandle(), &msg))  //needed to allow for tabbing on relevant forms
		{
			continue;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}



/*=========================================
              EVENT FUNCTIONS

ALL "event_" functions are in the EventManagerClass
===========================================*/




LRESULT CALLBACK ChildWinProc(HWND child_hWnd, UINT child_message, WPARAM child_wParam, LPARAM child_lParam)
{

	//Event handler for the Application'schild/dialog windows
	switch (child_message)
	{
	case WM_NOTIFY:
		return(app_mgr.event_handleNotify(child_hWnd, child_message, child_wParam, child_lParam));
		break;
	case WM_COMMAND:
		//a command (menu or button) was clicked
		// event_ methods belong to event handler class
		app_mgr.event_handleClick(child_hWnd, child_wParam, child_lParam);
		break;
	case WM_CTLCOLORSTATIC:
		app_mgr.handleLabelRepaint((HDC)child_wParam, (HWND)child_lParam);
		break;
	default:
		return DefWindowProcW(child_hWnd, child_message, child_wParam, child_lParam);
	}
}





LRESULT CALLBACK MainWinProc(HWND parent_hWnd, UINT parent_message, WPARAM parent_wParam, LPARAM parent_lParam)
{
	
	//Event handler for the Application's main window
	switch (parent_message){
		case WM_COMMAND:
			//a command (menu or button) was clicked
			app_mgr.event_handleClick(parent_hWnd, parent_wParam, parent_lParam);
			break;
		case WM_CLOSE:  // Window Closing
			if (app_mgr.event_checkClose(parent_hWnd)) DestroyWindow(parent_hWnd);
			if(app_mgr.event_checkClose(parent_hWnd)) DestroyWindow(parent_hWnd);
			break;
		case WM_SIZE:
			if (parent_wParam == SIZE_MAXIMIZED || parent_wParam == SIZE_RESTORED) {
				app_mgr.positionWindows();
			}
			break;
		break;

		case WM_EXITSIZEMOVE:  
			// Window Resized
			app_mgr.positionWindows();
			break;
		case WM_DESTROY:  // Window Destroyed
			PostQuitMessage(0); //Application Quit
			break;
		default:
			return DefWindowProcW(parent_hWnd, parent_message, parent_wParam, parent_lParam);
	}
}
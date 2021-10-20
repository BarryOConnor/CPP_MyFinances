#pragma once
#include "stdafx.h"
//class ApplicationManagerClass;
//extern ApplicationManagerClass app_mgr;
#include <Windows.h>
#include <commdlg.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "MyFinancesDefinitions.h"
#include "WindowClass.h"
#include "EncryptionClass.h"
#include "EventManagerClass.h"
#include "3rdpartylibs/sqlite3.h"
#include "3rdpartylibs/sqlitewrapper.h"




void EventManagerClass::event_addNewTransaction() {
	/*
	Method used to update the database with a new transaction details
	*/

	if (app_mgr.windows[WIN_DIALOG].validateForm()) {// validate the form before submitting
		DatabaseClass database(APP_DATABASE);//open the database
		// prepare the SQL statement
		StatementClass statement(database, "INSERT into tblTransactions (userID, transDate, transType, transDescription, transAmount) VALUES (?, ?, ?, ?, ?);");
		// bind the parameters

		SYSTEMTIME temp_date;  // systemtime struct to store a systemtime
		ostringstream ossDate; // formatting the date

		//get the date from the datepicker control
		DateTime_GetSystemtime(app_mgr.windows[WIN_DIALOG].controls[DPK_DIALOG_DATE].getHandle(), &temp_date);
		//convert to the correct format
		ossDate << temp_date.wYear << "-" << setw(2) << setfill('0') << temp_date.wMonth << "-" << setw(2) << setfill('0') << temp_date.wDay;


		//bind the values to statement parameters
		statement.bind().text(1, encrypt(to_string(app_mgr.getCurrentUser())));
		statement.bind().text(2, ossDate.str()); //can't encrypt dates if we want to search
		int const cbobox_index{ SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_GETCURSEL, NULL, NULL) };  // get the selected index of the combobox
		switch (cbobox_index) {
		case 0:
			statement.bind().text(3, encrypt("BAC"));
			break;
		case 1:
			statement.bind().text(3, encrypt("C/L"));
			break;
		case 2:
			statement.bind().text(3, encrypt("DPC"));
			break;
		case 3:
			statement.bind().text(3, encrypt("EDC"));
			break;
		case 4:
			statement.bind().text(3, encrypt("POS"));
			break;
		case 5:
			statement.bind().text(3, encrypt("FOOD"));
			break;
		case 6:
			statement.bind().text(3, encrypt("BILL"));
			break;
		case 7:
			statement.bind().text(3, encrypt("GIFT-OUT"));
			break;
		case 8:
			statement.bind().text(3, encrypt("GIFT-IN"));
			break;
		case 9:
			statement.bind().text(3, encrypt("TRAVEL"));
			break;
		}
		statement.bind().text(4, encrypt(app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_DESCRIPTION].getControlText()));
		statement.bind().text(5, encrypt(app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_AMOUNT].getControlText()));

		statement.exec(); // run the query


		if (database.changes() > 0) {// success
			//update the  listview
			app_mgr.populateListview();

			//notify the user and re-enable the window
			app_mgr.msgbox("This transcastion has been successfully added.");
			app_mgr.windows[WIN_DIALOG].visibility(false);
			EnableWindow(app_mgr.windows[WIN_BUDGET].getHandle(), true);
		}
		else {
			app_mgr.msgbox("Unable to add this transaction. Please try again.");
		}

	}
}






void EventManagerClass::event_attemptLogin(WindowNames current_window, WindowNames redirect_window) {
/*
Method used to attempt a login from the details provided
*/
	if (app_mgr.windows[current_window].validateForm()) {// validate the form before submitting
		DatabaseClass database(APP_DATABASE);//open the database
		// prepare the SQL statement
		StatementClass statement(database, "SELECT userID, userFullName FROM tblUsers WHERE userUsername = ? and userPassword = ? LIMIT 1;");
		// bind the parameters
		statement.bind().text(1, encrypt(app_mgr.windows[current_window].controls[TXT_LOGIN_USERNAME].getControlText()));
		statement.bind().text(2, encrypt(app_mgr.windows[current_window].controls[TXT_LOGIN_PASSWORD].getControlText()));

		while (statement.step()) { // success
			app_mgr.setCurrentUser(statement.row().int32(0)); //set the current user
			app_mgr.windows[WIN_BUDGET].controls[LBL_BUDGET_HEADING].setControlText("Welcome " + decrypt(statement.row().text(1)));
			app_mgr.updateMainMenu(); //update the main menu now they are logged in
			app_mgr.current_sort = "ORDER BY transDate DESC";
			app_mgr.populateListview(); //populate the listview with the logged in user's data
			event_transitionWindows(current_window, redirect_window);
		}
		if (app_mgr.getCurrentUser()==0) {//fail
			app_mgr.msgbox("Login attempt unsuccessful.");
		}
	}
}





bool EventManagerClass::event_checkClose(HWND window_handle)
{
/*
Method used to toggle the WIN_ACCOUNT window from being a "CREATE" account page to "UPDATE" account page
*/
	if (app_mgr.msgbox(L"Are you sure you want to exit?", ICN_QUESTION) == IDOK) {  // 
		return true; 
	}
	else {
		return false;
	};
}





void EventManagerClass::event_createAccount(WindowNames current_window, WindowNames redirect_window)
{
/*
Method used to create a new account from submitted data
*/
	if (app_mgr.windows[current_window].validateForm()) {  // validate the form before submitting
		DatabaseClass database(APP_DATABASE);  //open the database
		int const cbobox_index{ SendMessage(app_mgr.windows[current_window].controls[CBO_ACCOUNT_SECRET_QUESTION].getHandle(), CB_GETCURSEL, NULL, NULL) };  // get the selected index of the combobox
		// prepare the SQL statement
		StatementClass statement(database, L"INSERT INTO tblUsers (userFullName, userUsername, userPassword, userEmail, userSecretQuestion, userSecretAnswer) VALUES (? , ? , ? , ? , ? , ? );"); 
		
		// Bind the encrypted parameters to the sql statement
		string encrypted_text;
		//encrypt the contents of the control
		encrypted_text = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_FULLNAME].getControlText());
		//send encrypted text to the database
		statement.bind().text(1, encrypted_text);
		encrypted_text = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_USERNAME].getControlText());
		statement.bind().text(2, encrypted_text);
		encrypted_text = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_PASSWORD].getControlText());
		statement.bind().text(3, encrypted_text);
		encrypted_text = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_EMAIL].getControlText());
		statement.bind().text(4, encrypted_text);
		encrypted_text = encrypt(to_string(cbobox_index));
		statement.bind().text(5, encrypted_text);
		encrypted_text = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_SECRET_ANSWER].getControlText());
		statement.bind().text(6, encrypted_text);
		statement.exec(); // execute the SQL statement

		if (database.changes() > 0){ // if changes were made to the database
			app_mgr.msgbox(L"Account created successfully. Returning you to the Login screen.\n\n Please Login with your new account details");
			event_transitionWindows(current_window, redirect_window);
		}
		else {
			app_mgr.msgbox(L"There was a problem creating your account.\n\n Please try again.");
		}
	}
}




void EventManagerClass::event_createCSV() {
/*====================
Method to export a CSV
======================*/

	//Get system file dialog and set some default data
	wchar_t szFile[MAX_PATH] = L"MyFinancesBACKUP.csv";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"CSV FIles\0*.CSV\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	
	if (GetSaveFileName(&ofn)) {
		ofstream outFile(ofn.lpstrFile);
		
		DatabaseClass database(APP_DATABASE);//open the database
		// prepare the SQL statement
		StatementClass statement(database, "SELECT * FROM tblTransactions WHERE userID = ?;");
		// bind the parameters
		statement.bind().text(1, encrypt(to_string(app_mgr.getCurrentUser())));

		//output column headers
		outFile << "TRANSACTION_ID,USER_ID,DATE,TYPE,DESCRIPTION,AMOUNT" << "\n";
		while (statement.step()) { // success

			outFile << "\"" << statement.row().int32(0) << "\"|\"" << decrypt(statement.row().text(1)) << "\"|\"" << statement.row().text(2) << "\"|\"" << decrypt(statement.row().text(3)) << "\"|\"" << decrypt(statement.row().text(4)) << "\"|\"" << decrypt(statement.row().text(5)) << "\"\n";
		}
		outFile.close();
		app_mgr.msgbox("Backup Complete");
	}
};





void EventManagerClass::event_deleteAccount()
{
/*
Method used to delete the current user's account
*/
	DatabaseClass database(APP_DATABASE);  //open the database
	//set up the SQL for the first table
	StatementClass statementTrans(database, L"DELETE FROM tblTransactions WHERE userID = ?;");
	statementTrans.bind().int32(1, app_mgr.getCurrentUser());//bind the current user id
	statementTrans.exec();//run the query

	//set the query for the second table
	StatementClass statementUsers(database, L"DELETE FROM tblUsers WHERE userID = ?;");
	statementUsers.bind().int32(1, app_mgr.getCurrentUser());//bind the current user id
	statementUsers.exec();//run the query

	if (database.changes() > 0) { // if changes were made to the database
		app_mgr.msgbox(L"Account successfully deleted.");
	}
	else {
		app_mgr.msgbox(L"There was a problem deleting your account.\n\n Please try again.");
	}
}





void EventManagerClass::event_deleteSelectedTransaction() {
	/*
	Method used to delete the currently selected transaction in the listbox
	*/

	//firatly, check if they mean to delete
	if (app_mgr.msgbox("This will permanently delete the selected transaction. Are you sure?", ICN_QUESTION) == IDOK) {
		int selected_index = 0;

		//get the selected index of the listview
		selected_index = SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

		if (selected_index == -1)
		{
			//none was selected, alert user	
			app_mgr.msgbox("You must select an item before you can delete it.");
		}
		else {

			//Get the text from the first column so we can find it in the database and delete it
			LV_ITEM lvw_item;      // CONTROL SPECIFIC - List view item struct
			vector<wchar_t> buffer(1024); // temp store
			string converted_string;

			//reset the structure
			lvw_item = { 0 };
			lvw_item.iSubItem = 0; // first column
			lvw_item.pszText = buffer.data(); //clear the structure value and set it to point to the from_text var
			lvw_item.cchTextMax = static_cast<int>(buffer.size()); // max characters


			//get the text into the LVW_item structure - from_text now contains the text we need
			SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);

			DatabaseClass database(APP_DATABASE);  //open the database
			StatementClass statement(database, L"DELETE FROM tblTransactions WHERE userID = ? AND transID = ?;");
			//bind the current user id
			statement.bind().text(1, app_mgr.encrypt(to_string(app_mgr.getCurrentUser())));
			//bind the transaction ID
			statement.bind().int32(2, stoi(&buffer[0]));
			statement.exec(); // run the query


			if (database.changes() > 0) { // Delete was successful, remove it from the listbox and let the user know
				//delete from the listview
				ListView_DeleteItem(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), selected_index);

				app_mgr.msgbox(L"Transaction successfully deleted.");

			}
			else {
				app_mgr.msgbox(L"There was a problem deleting the transaction.\n\n Please try again.");
			}
		}
	}
}


void EventManagerClass::event_editSelectedTransaction() {
	/*
	Method used to update the database with the edited transaction details
	*/
	if (app_mgr.windows[WIN_DIALOG].validateForm()) {// validate the form before submitting
		DatabaseClass database(APP_DATABASE);//open the database
		// prepare the SQL statement
		StatementClass statement(database, "UPDATE tblTransactions SET transDate = ?, transType = ?, transDescription = ?, transAmount=? WHERE transID = ? and userID = ?;");
		// bind the parameters
		
		SYSTEMTIME temp_date;  // systemtime struct to store a systemtime
		ostringstream ossDate; // formatting the date

		//get the date from the datepicker control
		DateTime_GetSystemtime(app_mgr.windows[WIN_DIALOG].controls[DPK_DIALOG_DATE].getHandle(), &temp_date);
		//convert to the correct format
		ossDate << temp_date.wYear << "-" << setw(2) << setfill('0') << temp_date.wMonth << "-" << setw(2) << setfill('0') << temp_date.wDay;
		
			
		//bind the values to statement parameters
		statement.bind().text(1, ossDate.str());
		int const cbobox_index{ SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_GETCURSEL, NULL, NULL) };  // get the selected index of the combobox
		switch (cbobox_index) {
			case 0:
				statement.bind().text(2, encrypt("BAC"));
				break;
			case 1:
				statement.bind().text(2, encrypt("C/L"));
				break;
			case 2:
				statement.bind().text(2, encrypt("DPC"));
				break;
			case 3:
				statement.bind().text(2, encrypt("EDC"));
				break;
			case 4:
				statement.bind().text(2, encrypt("POS"));
				break;
			case 5:
				statement.bind().text(2, encrypt("FOOD"));
				break;
			case 6:
				statement.bind().text(2, encrypt("BILL"));
				break;
			case 7:
				statement.bind().text(2, encrypt("GIFT-OUT"));
				break;
			case 8:
				statement.bind().text(2, encrypt("GIFT-IN"));
				break;
			case 9:
				statement.bind().text(2, encrypt("TRAVEL"));
				break;
		}
		statement.bind().text(3, encrypt(app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_DESCRIPTION].getControlText()));
		statement.bind().text(4, encrypt(app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_AMOUNT].getControlText()));
		statement.bind().int32(5, stoi(app_mgr.windows[WIN_DIALOG].controls[LBL_DIALOG_TRANSACTION].getControlText()));
		statement.bind().text(6, encrypt(to_string(app_mgr.getCurrentUser())));
		statement.exec(); // run the query


		if (database.changes() > 0) {// success
			//update the  listview
			app_mgr.populateListview();

			//notify the user and re-enable the window
			app_mgr.msgbox("This trancastion has been successfully updated.");
			app_mgr.windows[WIN_DIALOG].visibility(false);
			EnableWindow(app_mgr.windows[WIN_BUDGET].getHandle(), true);
		}
		else {
			app_mgr.msgbox("Unable to update this transaction. Please try again.");
		}
	
	}
}


void EventManagerClass::event_handleClick( HWND window_handle, WPARAM wParam, LPARAM lParam) {
	/*
	Method used to handle click events from the windows message loop
	*/

	if (HIWORD(wParam) == 0) {  //a MENU OR BUTTON has been clicked
		switch (wParam)
		{
		/*=========================================
					 MENU CLICKS
		===========================================*/
		case MNU_FILE_IMPORT_ID:  //File->Import menu clicked
			event_importCSV();
			break;
		case MNU_FILE_EXPORT_ID:  //File->Export menu clicked
			event_createCSV();
			break;
		case MNU_FILE_EXIT_ID:  //File->Exit menu clicked
			if (event_checkClose(window_handle)) DestroyWindow(window_handle);
			break;
		case MNU_ACCOUNT_UPDATE_ID:  //Account->Update menu clicked
			event_transitionWindows(WIN_BUDGET, WIN_ACCOUNT);
			event_toggleAccount();
			break;
		case MNU_ACCOUNT_DELETE_ID:  //Account->Delete menu clicked
			if (app_mgr.msgbox(L"Are you sure you want to delete your account?\n\nThis will delete all data and is permanent.", ICN_QUESTION) == IDOK) {  // 
				event_deleteAccount();
				event_logout(WIN_LOGIN);			}
			break;
		case MNU_ACCOUNT_LOGOUT_ID:  //Account->Logout menu clicked
			event_logout(WIN_LOGIN);
			break;
		
		/*=========================================
					BUTTON CLICKS
		===========================================*/
			//=========  LOGIN WINDOW EVENTS  =========
		case BTN_LOGIN_LOGIN_ID://Login button was clicked
			event_attemptLogin(WIN_LOGIN, WIN_BUDGET); //pass a pointer to the window class instance so the event manager can access and handle the event
			break;

		case BTN_LOGIN_CREATE_ACCOUNT_ID://Create Account button was clicked
			event_transitionWindows(WIN_LOGIN, WIN_ACCOUNT); // hide one window, show another
			break;

			//=========  CREATE/UPDATE ACCOUNT WINDOW EVENTS  =========
		case BTN_ACCOUNT_NEW_ACCOUNT_ID: //Login button was clicked
			if (app_mgr.getCurrentUser() == 0) {  // no user logged in so use the window as CREATE an account
				event_createAccount(WIN_ACCOUNT, WIN_LOGIN); //pass a pointer to the window class instance so the event manager can access and handle the event
			}
			else {// user logged in so use the window as UPDATE an account
				event_updateAccount(WIN_ACCOUNT, WIN_BUDGET);
			}
			break;

		case  BTN_ACCOUNT_RETURN_LOGIN_ID:
			if (app_mgr.getCurrentUser() == 0) {  // no user logged in so return to the LOGIN screen
				event_transitionWindows(WIN_ACCOUNT, WIN_LOGIN); //pass a pointer to the window class instance so the event manager can access and handle the event
			}
			else {  // no user logged in so return to the BUDGET screen
				event_transitionWindows(WIN_ACCOUNT, WIN_BUDGET); //pass a pointer to the window class instance so the event manager can access and handle the event
			}
			break;

			//=========  BUDGET WINDOW EVENTS  =========
		case BTN_BUDGET_ADDNEW_ID://Add new transaction button was clicked
			app_mgr.windows[WIN_DIALOG].controls[BTN_DIALOG_EDIT].visibility(false);
			app_mgr.windows[WIN_DIALOG].controls[BTN_DIALOG_ADD].visibility(true);
			event_showNewTransaction();
			break;

		case BTN_BUDGET_EDIT_ID://Edit transaction button clicked
			app_mgr.windows[WIN_DIALOG].controls[BTN_DIALOG_EDIT].visibility(true);
			app_mgr.windows[WIN_DIALOG].controls[BTN_DIALOG_ADD].visibility(false);

			event_showSelectedTransaction();
			break;

		case BTN_BUDGET_DELETE_ID://Delete Transaction button clicked
			event_deleteSelectedTransaction();
			break;

			//=========  DIALOG WINDOW EVENTS  =========
		case BTN_DIALOG_CANCEL_ID://Add new transaction button was clicked
			app_mgr.windows[WIN_DIALOG].visibility(false);
			EnableWindow(app_mgr.windows[WIN_BUDGET].getHandle(),true);
			break;

		case BTN_DIALOG_EDIT_ID://Add new transaction button was clicked
			event_editSelectedTransaction();
			break;
			
		case BTN_DIALOG_ADD_ID://Add new transaction button was clicked
			event_addNewTransaction();
			break;

		}
	}
}






LRESULT EventManagerClass::event_handleNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//Handler for the WM_NOTIFY messages (LISTVIEW AND DATEPICKER)

	if (wParam == LVW_BUDGET_TRANSACTIONS_ID) {
		//listview specific notify events

		LV_DISPINFO *pLvdi = (LV_DISPINFO *)lParam;  //message code information
		NM_LISTVIEW *pNm = (NM_LISTVIEW *)lParam; //listview information
		switch (pLvdi->hdr.code)
		{

		case NM_DBLCLK:

			//double click event edits
			event_showSelectedTransaction();
			break;

		case LVN_COLUMNCLICK:

			// The user clicked on one of the column headings - sort by
			// this column.
			switch ((LPARAM)(pNm->iSubItem)) {
			case 2: //sort by Date Column
				if (app_mgr.sorting.sort_desc_date) {
					app_mgr.current_sort = "ORDER BY transDate DESC";
					app_mgr.populateListview();
				}
				else {
					app_mgr.current_sort = "ORDER BY transDate ASC";
					app_mgr.populateListview();
				};
				app_mgr.sorting.sort_desc_date = !app_mgr.sorting.sort_desc_date;
				break;
			case 3://sort by Type Column
				if (app_mgr.sorting.sort_desc_type) {
					app_mgr.current_sort = "ORDER BY transDate ASC";
					app_mgr.populateListview();
				}
				else {
					app_mgr.current_sort = "ORDER BY transType ASC";
					app_mgr.populateListview();
				};
				app_mgr.sorting.sort_desc_type = !app_mgr.sorting.sort_desc_type;
				break;
			case 4://sort by Description Column
				if (app_mgr.sorting.sort_desc_description) {
					app_mgr.current_sort = "ORDER BY transDescription DESC";
					app_mgr.populateListview();
				}
				else {
					app_mgr.current_sort = "ORDER BY transDescription ASC";
					app_mgr.populateListview();
				};
				app_mgr.sorting.sort_desc_description = !app_mgr.sorting.sort_desc_description;
				break;
			case 5://sort by Amount Column
				if (app_mgr.sorting.sort_desc_amount) {
					app_mgr.current_sort = "ORDER BY CAST(transAmount AS real) DESC";
					app_mgr.populateListview();
				}
				else {
					app_mgr.current_sort = "ORDER BY CAST(transAmount AS real) ASC";
					app_mgr.populateListview();
				};
				app_mgr.sorting.sort_desc_amount = !app_mgr.sorting.sort_desc_amount;
				break;
			}
			break;
		}

	}
	if (wParam == DPK_BUDGET_FROM_ID || wParam == DPK_BUDGET_TO_ID) {

		//DATEPICKER CONTROLS

		LPNMHDR hdr = (LPNMHDR)lParam;
		if (hdr->code == DTN_DATETIMECHANGE)
		{
			LPNMDATETIMECHANGE lpChange = (LPNMDATETIMECHANGE)lParam; //Struct returned when the datepicker changes
			SYSTEMTIME new_date = lpChange->st;

			ostringstream ossDate; // formatting the date
			//convert to the correct format
			ossDate << new_date.wYear << "-" << setw(2) << setfill('0') << new_date.wMonth << "-" << setw(2) << setfill('0') << new_date.wDay;
			//set the string in the app_mgr
			if (wParam == DPK_BUDGET_FROM_ID) {
				app_mgr.date_from = ossDate.str();
			}
			else {
				app_mgr.date_to = ossDate.str();
			}
			//repopulate the listview
			app_mgr.populateListview();
		}
	}
	return 0L;
}




void EventManagerClass::event_importCSV() {
	/*====================
	Method to export a CSV
	======================*/

	//Get system date to append to filename

	wchar_t szFile[MAX_PATH] = L"MyFinancesBACKUP.csv";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"CSV FIles\0*.CSV\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


	if (GetOpenFileName(&ofn)) {
		bool do_backup = false;
		bool backup_complete = false;
		ifstream inFile(ofn.lpstrFile);
		if (inFile)
		{
			int backup_choice = app_mgr.msgbox("Do you want this backup to overwrite information in the database?\n\n Click \"Yes\" to clear all data\n\nClick \"No\" to add the contents of this backup to existing data", ICN_YESNO);
			DatabaseClass database(APP_DATABASE);//open the database

			if (backup_choice == IDYES) {
				//DOUBLE CHECK
				if (app_mgr.msgbox("This will OVERWRITE ALL DATA, are you sure you wish to do this?", ICN_QUESTION) == IDOK) {
					//DELETE AND OVERWRITE DATA - THIS INCLUDES transID
					StatementClass statement(database, "DELETE FROM tblTransactions WHERE userID = ?;");
					statement.bind().text(1, encrypt(to_string(app_mgr.getCurrentUser())));
					statement.exec();
					do_backup = true;
				}
				else {
					app_mgr.msgbox("Import Cancelled");
				}
			}
			else if (backup_choice == IDNO) {
				do_backup = true;
			}
			else {
				app_mgr.msgbox("Import Cancelled");
			}

			if (do_backup) {
				string line;
				vector<vector<string>> parsedCsv;
				while (getline(inFile, line))
				{
					stringstream lineStream(line);
					string cell;
					vector<string> parsedRow;
					while (getline(lineStream, cell, '|'))
					{
						parsedRow.push_back(cell);
					}

					parsedCsv.push_back(parsedRow);
				}
				string SQLString = "";

				for (int loopcount = 1; loopcount < parsedCsv.size(); loopcount++)
				{
					for (int loopcount_inner = 1; loopcount_inner < parsedCsv[loopcount].size(); loopcount_inner++)
					{
						parsedCsv[loopcount][loopcount_inner].replace(0, 1, "");
						parsedCsv[loopcount][loopcount_inner].replace(parsedCsv[loopcount][loopcount_inner].length() - 1, 1, "");
					}

					SQLString = "INSERT INTO tblTransactions (userID,transDate,transType,transDescription,transAmount) VALUES('" + encrypt(parsedCsv[loopcount][1]) + "', '" + parsedCsv[loopcount][2] + "', '" + encrypt(parsedCsv[loopcount][3]) + "', '" + encrypt(parsedCsv[loopcount][4]) + "', '" + encrypt(parsedCsv[loopcount][5]) + "')";

					//app_mgr.msgbox(SQLString);
					database.exec(SQLString);
					if (database.changes() > 0) {
						backup_complete = true;
					}
				}
				if (backup_complete) {
					app_mgr.msgbox("The import was successful");
					app_mgr.populateListview();
				}
			}
		}
		else {
			app_mgr.msgbox("There was an issue trying to open your selected file");
		}
		
	}
};






void EventManagerClass::event_logout(WindowNames redirect_window) {
	/*
Method used to logout the current user
*/
	app_mgr.setCurrentUser(0);
	for (vector<WindowClass>::iterator this_window = app_mgr.windows.begin(); this_window != app_mgr.windows.end(); ++this_window)  // iterate throught he windows vector
	{
		if (this_window->getHandle() != app_mgr.getAppHandle()) {
			this_window->visibility(false);  // hide all windows
		}
	}
	event_toggleAccount(); //logout
	app_mgr.windows[redirect_window].clearControls();
	app_mgr.windows[redirect_window].visibility(true); //load the login screen
}





void EventManagerClass::event_showNewTransaction() {
	/*
	Method used to show the DIALOG for a user to ADD a new transaction
	*/

	//show the dialog and disable focus inputs on the current window
	EnableWindow(app_mgr.windows[WIN_BUDGET].getHandle(), false);
	app_mgr.centerDialog();
	app_mgr.windows[WIN_DIALOG].visibility(true);
	
	//update the header, button text and clear the controls
	app_mgr.windows[WIN_DIALOG].controls[LBL_DIALOG_HEADING].setControlText("Add a Transaction");
	app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_DESCRIPTION].setControlText("");
	app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_AMOUNT].setControlText("");


	//create a systemtime struct
	SYSTEMTIME temp_date;
	//set it to NOW
	GetSystemTime(&temp_date);
	//update the datepicker with the default value
	DateTime_SetSystemtime(app_mgr.windows[WIN_DIALOG].controls[DPK_DIALOG_DATE].getHandle(), GDT_VALID, &temp_date);

	//reset the combobox to it's default value
	SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0); 
	
}






void EventManagerClass::event_showSelectedTransaction() {
	/*
	Method used to delete the currently selected transaction in the listbox
	*/
	int selected_index = 0;

	//get the selected index of the listview
	selected_index = SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETNEXTITEM, -1, LVNI_FOCUSED);

	if (selected_index == -1)
	{
		//none was selected, alert user	
		app_mgr.msgbox("You must select an item before you can edit it.");
	}
	else {
		//show the dialog and disable focus inputs on the current window
		EnableWindow(app_mgr.windows[WIN_BUDGET].getHandle(), false);
		app_mgr.centerDialog();
		app_mgr.windows[WIN_DIALOG].visibility(true);		


		//set the dialog box up for editing
		app_mgr.windows[WIN_DIALOG].controls[LBL_DIALOG_HEADING].setControlText("Edit a Transaction");

		//Get the text from the first column so we can send it to the dialog
		LV_ITEM lvw_item;      // CONTROL SPECIFIC - List view item struct
		vector<wchar_t> buffer(1024); // temp store
		string converted_string;

		//reset the structure
		lvw_item = { 0 };
		lvw_item.iSubItem = 0; // first column
		lvw_item.pszText = buffer.data(); //clear the structure value and set it to point to the from_text var
		lvw_item.cchTextMax = static_cast<int>(buffer.size()); // max characters


		//get the text into the LVW_item structure - from_text now contains the text we need
		SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);
		//update the contents of the corresponding control
		converted_string = app_mgr.convertStrings(&buffer[0]);  //conversion
		app_mgr.windows[WIN_DIALOG].controls[LBL_DIALOG_TRANSACTION].setControlText(converted_string);


		//DATE
		lvw_item.iSubItem = 2;
		//get the text into the LVW_item structure - from_text now contains the text we need
		SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);
		SYSTEMTIME temp_date = app_mgr.makeSystemTime(app_mgr.convertStrings(&buffer[0]).c_str());
		DateTime_SetSystemtime(app_mgr.windows[WIN_DIALOG].controls[DPK_DIALOG_DATE].getHandle(),GDT_VALID, &temp_date);


		//TYPE
		lvw_item.iSubItem = 3;
		//get the text into the LVW_item structure - from_text now contains the text we need
		SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);


		//since we're dealing with a string we have to use if's to match to an int
		converted_string = app_mgr.convertStrings(&buffer[0]);
		if (strcmp(converted_string.c_str(), "BAC") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "C/L") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)1, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "DPC") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)2, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "EDC") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)3, (LPARAM)0); }
		if (strcmp(converted_string.c_str(),"POS") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)4, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "FOOD") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)5, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "BILL") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)6, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "GIFT-OUT") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)7, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "GIFT-IN") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)8, (LPARAM)0); }
		if (strcmp(converted_string.c_str(), "TRAVEL") == 0) { SendMessage(app_mgr.windows[WIN_DIALOG].controls[CBO_DIALOG_TYPE].getHandle(), CB_SETCURSEL, (WPARAM)9, (LPARAM)0); }



		//DESCRIPTION
		lvw_item.iSubItem = 4;
		//get the text into the LVW_item structure - from_text now contains the text we need
		SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);
		converted_string = app_mgr.convertStrings(&buffer[0]);  //conversion
		//update the contents of the corresponding control
		app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_DESCRIPTION].setControlText(converted_string);


		//DESCRIPTION
		lvw_item.iSubItem = 5;
		//get the text into the LVW_item structure - from_text now contains the text we need
		SendMessage(app_mgr.windows[WIN_BUDGET].controls[LVW_BUDGET_TRANSACTIONS].getHandle(), LVM_GETITEMTEXT, selected_index, (LPARAM)&lvw_item);
		converted_string = app_mgr.convertStrings(&buffer[0]);  //conversion
		//update the contents of the corresponding control
		app_mgr.windows[WIN_DIALOG].controls[TXT_DIALOG_AMOUNT].setControlText(converted_string);
	}
}




void EventManagerClass::event_toggleAccount()
{
/*
Method used to toggle the WIN_ACCOUNT window from being a "CREATE" account page to "UPDATE" account page
*/
	if (app_mgr.getCurrentUser()==0) {  
		//change the text on controls to make it look like a Create an Account page
		app_mgr.windows[WIN_ACCOUNT].controls[LBL_ACCOUNT_HEADING].setControlText("Create New Account");
		app_mgr.windows[WIN_ACCOUNT].controls[BTN_ACCOUNT_RETURN_LOGIN].setControlText("Return to Login");
		app_mgr.windows[WIN_ACCOUNT].controls[BTN_ACCOUNT_NEW_ACCOUNT].setControlText("Create Account");
	}
	else {
		//change the text on controls to make it look like Update an Account page
		app_mgr.windows[WIN_ACCOUNT].controls[LBL_ACCOUNT_HEADING].setControlText("Update Account Details");
		app_mgr.windows[WIN_ACCOUNT].controls[BTN_ACCOUNT_RETURN_LOGIN].setControlText("Return to Budget");
		app_mgr.windows[WIN_ACCOUNT].controls[BTN_ACCOUNT_NEW_ACCOUNT].setControlText("Update Account");

		//get the logged in user's information from the database
		DatabaseClass database(APP_DATABASE);
		StatementClass statement(database, "SELECT userFullName, userUsername, userPassword, userEmail, userSecretQuestion, userSecretAnswer FROM tblUsers WHERE userID = ? LIMIT 1;");
		statement.bind().int32(1, app_mgr.getCurrentUser());

		string decrypted_val;
		while (statement.step()) {
			//decrypt the value stored in the database
			decrypted_val = decrypt(statement.row().text(0));
			//change the control's text to the decrypted text'
			app_mgr.windows[WIN_ACCOUNT].controls[TXT_ACCOUNT_FULLNAME].setControlText(decrypted_val);
			decrypted_val = decrypt(statement.row().text(1));
			app_mgr.windows[WIN_ACCOUNT].controls[TXT_ACCOUNT_USERNAME].setControlText(decrypted_val);
			decrypted_val = decrypt(statement.row().text(2));
			app_mgr.windows[WIN_ACCOUNT].controls[TXT_ACCOUNT_PASSWORD].setControlText(decrypted_val);
			decrypted_val = decrypt(statement.row().text(3));
			app_mgr.windows[WIN_ACCOUNT].controls[TXT_ACCOUNT_EMAIL].setControlText(decrypted_val);
			decrypted_val = decrypt(statement.row().text(4));
			// Set the current selection in the combobox  use std::stoi() (String to int)
			SendMessage(app_mgr.windows[WIN_ACCOUNT].controls[CBO_ACCOUNT_SECRET_QUESTION].getHandle(), CB_SETCURSEL, stoi(decrypted_val), 0);
			decrypted_val = decrypt(statement.row().text(5));
			app_mgr.windows[WIN_ACCOUNT].controls[TXT_ACCOUNT_SECRET_ANSWER].setControlText(decrypt(statement.row().text(5)));
		}
	}
}





void EventManagerClass::event_transitionWindows(WindowNames hide_this, WindowNames show_this)
{
	/*
	Method to transition between 2 windows
	*/
	app_mgr.windows[hide_this].visibility(false); //hide the first window
	app_mgr.windows[show_this].clearControls(); //reset focus and control contents on the second page
	app_mgr.windows[show_this].visibility(true); //show the second page
}





void EventManagerClass::event_updateAccount(WindowNames current_window, WindowNames redirect_window)
{
	/*
Method used to update the user details when the window is submitted
*/
	
	if (app_mgr.windows[current_window].validateForm()) {  // validate the form before submitting
		DatabaseClass database(APP_DATABASE);  //open the database
		int const cbobox_index{ SendMessage(app_mgr.windows[current_window].controls[CBO_ACCOUNT_SECRET_QUESTION].getHandle(), CB_GETCURSEL, NULL, NULL) };  // get the selected index of the combobox
		// prepare the SQL statement
		StatementClass statement(database, L"UPDATE tblUsers SET userFullName = ?, userUsername = ?, userPassword = ?, userEmail = ?, userSecretQuestion = ?, userSecretAnswer = ? where userID = ?;");
		
		string encrypted_val;
		//encrypt the contents of the control
		encrypted_val = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_FULLNAME].getControlText());
		//encrypt the contents of the control
		statement.bind().text(1, encrypted_val);// Bind the parameters to the sql statement
		//send encrypted text to the database
		encrypted_val = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_USERNAME].getControlText());
		statement.bind().text(2, encrypted_val);
		encrypted_val = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_PASSWORD].getControlText());
		statement.bind().text(3, encrypted_val);
		encrypted_val = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_EMAIL].getControlText());
		statement.bind().text(4, encrypted_val);
		encrypted_val = encrypt(to_string(cbobox_index));
		statement.bind().text(5, encrypted_val);
		encrypted_val = encrypt(app_mgr.windows[current_window].controls[TXT_ACCOUNT_SECRET_ANSWER].getControlText());
		statement.bind().text(6, encrypted_val);
		//userid for the WHERE clause
		statement.bind().int32(7, app_mgr.getCurrentUser());
		statement.exec(); // execute the SQL statement
		if (database.changes() > 0) { // if changes were made to the database
			app_mgr.msgbox(L"Account updated successfully.");
			event_transitionWindows(current_window, redirect_window);
		}
		else {
			app_mgr.msgbox(L"There was a problem updating your account.\n\n Please try again.");
		}
	}
}
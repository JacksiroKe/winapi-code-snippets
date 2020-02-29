
#include <windows.h>

//IDENTIFIERS

#define		IDC_CHECKBOX1				100			///<Identifier For 1st Check Box 
#define		IDC_CHECKBOX2				101			///<Identifier For 2nd Check Box 
#define		IDC_CHECKBOX3				102			///<Identifier For 3rd Check Box 
#define		IDC_CHECKBOX4				103			///<Identifier For 4th Check Box
#define		IDC_RADIO11					104			///<Identifier For 1st Radio Button   
#define		IDC_RADIO12					105			///<Identifier For 2nd Radio Button 
#define		IDC_RADIO13					106			///<Identifier For 3rd Radio Button  
#define		IDC_RADIO21					107			///<Identifier For 1st Radio Button  
#define		IDC_RADIO22					108			///<Identifier For 2nd Radio Button  
#define		IDC_RADIO23					109			///<Identifier For 3rd Radio Button 
#define		IDC_EDITBOX	                110			///<Identifier For Edit Control 
#define		IDC_LISTBOX					111			///<Identifier For List Box 
#define		IDC_SUBMIT				    112			///<Identifier For Submit Button 
#define		IDC_GROUP1		            113			///<Identifier For 1st Group 
#define		IDC_GROUP2		            114			///<Identifier For 2nd Group 
#define		IDC_GROUP3		            115			///<Identifier For 3rd Group 
#define		IDC_RESET		            116			///<Identifier For Reset Button 
#define		IDC_COMBOBOX		        117			///<Identifier For Reset Button 	
#define      ONE                         1           ///<Macro Declaration.
#define      TWO                         2           ///<Macro Declaration.
#define      THREE                       3           ///<Macro Declaration.
#define      FOUR						4          ///<Macro Declaration.
#define      ZERO                        0           ///<Macro Declaration.
#define      FIVE						5          ///<Macro Declaration.

//Global Variables

HANDLE		ghInstance;	///< Contains the instance of the Application
int gCheckonce=ONE,i;      ///<used for the submit button to allow only one submission. 

LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
void CreateControls(HWND);
void AddItemsInListBox(HWND);
void CreateComboBox(HWND);
void CreateRadioButtonfirstgroup(HWND,int);
void CreateLabels(HWND);
void CreateGroups(HWND);
void CreateEditBox(HWND);
void CreateRadioButtonsecondgroup(HWND,int);
void CreateCheckBox(HWND,int);
void Createopbutton(HWND);
void CreateListBox(HWND);
char* RetrieveText(HWND);
void checkbox(HWND, HWND);
void EditText(HWND);
void radiocheck(int,HWND);
/**
 * WinMain() to register our window class
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]	 hInstance:- Handle to the programs executable module (the .exe file in memory)
 * @param [in]	 hPrevInstance:- Always NULL for Win32 programs.Handle to the previous instance of the application
 * @param [in]	 lpszCmdLine:- Pointer to a null-terminated string of 8-bit characters.specifying the command line 
					           for the application,excluding the program name
 * @param [in]	 nCmdShow:- How the window is to be shown
 *
 * @return		msg.wParam	it returns wParam value of each message.		
 *
 */

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow )
{

		WNDCLASS	wc;		///<contains the window class attributes that are registered by the RegisterClass function
		MSG			msg;    ///<structure contains message information from a thread's message queue
		HWND		hWnd;   ///<handle to window
		BOOL		bRet;

    LoadLibrary("riched32.dll");
	if( !hPrevInstance ){

		wc.lpszClassName = "GenericAppClass";			///<Name to identify the window class.
		wc.lpfnWndProc = WindowProc;					///<Pointer to the window procedure for this window class
		wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;	///<Specifies the class style(s)
		wc.hInstance = hInstance;                        
		wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );   ///<returns the handle to an icon to be loaded on the window
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );     ///<returns the handle to the cursor that has to be displayed
		wc.hbrBackground = (HBRUSH)( COLOR_WINDOW+11);  ///<set the background color
		wc.lpszMenuName = NULL;                         ///<class have no default menu
		wc.cbClsExtra = 0;                            
		wc.cbWndExtra = 0;
		RegisterClass( &wc );                           ///<this will register the class
	}

	ghInstance = hInstance;

	//CreateWindow will return the handle to a new window
	hWnd = CreateWindow( "GenericAppClass",					///<name of the window class
						"EXAMPLE OF TAB CONTROL",			///<window name
						WS_OVERLAPPEDWINDOW|WS_HSCROLL|
						WS_VSCROLL|WS_CLIPSIBLINGS,			///<specifies the window style
						0,									///<set the horizontal position of the window
						0,                                  ///<set the vertical position of the window
						CW_USEDEFAULT,                      ///<set the width of the window
						CW_USEDEFAULT,						///<set the height of the window
						NULL,                               ///<contains valid pointer for child window 
						NULL,								///<class menu is used
						hInstance,							///<Handle to the instance associated with the window
						NULL );  

	
	//function to show the window
	ShowWindow( hWnd, SW_MAXIMIZE );                        

	//it will be executed if function will return nonzero value
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0 )
	{
		//execute if there is an error otherwise 'else' part is executed
		if (bRet == -1) {                                
			
			return 0;
		
		} else {										
	
			TranslateMessage( &msg );		///<translates virtual-key messages into character messages.
			DispatchMessage( &msg );		///<dispatches a message to a window procedure
		}
	}										///<loops ended
	return (int)msg.wParam;					///< returns integer that indicates the additional information of the msg

}   //  End of WinMain() 

/**
 * MainWndProc procedure 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 * @param [in]     msg		specifies the msg
 * 
 * @return			0		returns zero.     
 *
 */

LRESULT CALLBACK WindowProc( HWND phWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
	switch( uMsg ){
	//call function to create all controls.
	case WM_CREATE:

		CreateControls(phWnd);		///<creates all the controls.
		break;
		
	//used to close the window
	case WM_CLOSE:

		if((MessageBox(phWnd, "DO YOU REALLY WANT TO EXIT ?", "Message",MB_OKCANCEL))==IDOK){
			//for final exit
			DestroyWindow(phWnd);	///< POSTS THE MESSAGE WM_DESTROY TO DESTROY THE CREATED WINDOW.
		}
		break;

	case WM_COMMAND:	

		//Handling Window Controllers 
		switch(LOWORD(wParam)){

		case IDC_SUBMIT: 

			///<handles the submission of the details into the list box on clicking submit 

			if(HIWORD(wParam)==BN_CLICKED && gCheckonce==ONE){  ///< hiword(wParam) takes the notification

				AddItemsInListBox(phWnd); ///<displays all the details in the list box.

				gCheckonce=TWO;
			}
			break;

		case IDC_RESET:
			/*
			if(HIWORD(wParam)==BN_CLICKED){

				for(i=ZERO; i<FOUR; i++){

					//sends message to uncheck the radio buttons of the first group.
					SendMessage(GetDlgItem(phWnd,(IDC_RADIO11+i)),BM_SETCHECK,BST_UNCHECKED,0);

					//sends message to uncheck the radio buttons of the second group
					SendMessage(GetDlgItem(phWnd,(IDC_RADIO21+i)),BM_SETCHECK,BST_UNCHECKED,0);

					//sends message to uncheck the check boxes.
					SendMessage(GetDlgItem(phWnd,(IDC_CHECKBOX1+i)),BM_SETCHECK,BST_UNCHECKED,0);
				}

				//sends message to reset the edit box.
				SendMessage(GetDlgItem(phWnd,IDC_EDITBOX),WM_SETTEXT,0,(LPARAM)" ");

				//sends message to reset the combo box.
				SendMessage(GetDlgItem(phWnd,IDC_COMBOBOX),CB_SETCURSEL,0,"");

				//sends message to reset the list box.
				SendMessage(GetDlgItem(phWnd,IDC_LISTBOX),LB_RESETCONTENT,0,0);

				//resets the flag of the submit button to 1.
				gCheckonce=ONE;
			}*/
			break;
		}
		break;

		//USED TO CONTROL WINDOW
	case WM_DESTROY:

		PostQuitMessage(0); ///< posts the message wm_quit to quit the window.

		break;

		//TO PROVIDE DEFAULT PROCESSING ON ANY WINDOW MESSAGES
	default:

		return( DefWindowProc( phWnd, uMsg, wParam, lParam ));				
	}																	 
	return 0;
}

/**
 * CreateLabels() used to create labels for all the controls 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Parent Window
 * @param [in]     hwndCheckBox		contains the address of the check box control
 * 
 */

void CreateLabels(HWND phWnd)
{
	//LABEL FOR THE COMBO BOX
	CreateWindow("Static", "Enter the field of Study:",WS_CHILD|WS_VISIBLE|SS_LEFT,40,80,200,
		20,phWnd,NULL,ghInstance,NULL);

	//LABEL FOR THE BADIOBUTTONS OF FIRST GROUP
	CreateWindow("Static","Enter the Students Year Of Study:",WS_CHILD|WS_VISIBLE|SS_LEFT,40,160,250									 ,20,phWnd,NULL,ghInstance,NULL);

	//LABEL FOR THE EDIT BOX
	CreateWindow("Static","Enter the Students Name:",WS_CHILD|WS_VISIBLE|SS_LEFT,510,80,180,20,									phWnd,NULL,ghInstance,NULL);

	//LABEL FOR THE RADIOBUTTONS OF THE SECOND GROUP
	CreateWindow("Static","Enter the Students Initials:",WS_CHILD|WS_VISIBLE|SS_LEFT,510,160,180,20,
		phWnd,NULL,ghInstance,NULL);
	
	//LABEL FOR THE CHECK BOX
	CreateWindow("Static","Enter the Students Hobbies:",WS_CHILD|WS_VISIBLE|SS_LEFT,220,350,280,20,										  phWnd,NULL,ghInstance,NULL);
}



/**
 * CreateControls() used to create Controls for all the controls 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Parent Window
 * @param [in]     hwndCheckBox		contains the address of the check box control
 * 
 */

void CreateControls( HWND phWnd )
{
	//TO CREATE COMBO BOX
	CreateComboBox(phWnd);

	//TO CREATE RADIOBUTTON
	CreateRadioButtonfirstgroup(phWnd,180);

	//TO CREATE LABELS FOR ALL THE CONTROLS
    CreateLabels(phWnd);

	//TO CREATE THE GROUP STRUCTURE
	CreateGroups(phWnd);
    
	// CreateEditBox(phWnd);
    CreateEditBox(phWnd);	

	//TO CREATE SECOND SET OF RADIOBUTTONS
	CreateRadioButtonsecondgroup(phWnd,180);

	//TO CREATE CHECK BOX
	CreateCheckBox(phWnd,340);

	//TO CREATE THE SUBMIT AND RESET BUTTONS FOR THE 1ST TAB
	Createopbutton(phWnd);
							
	//TO CREATE LIST BOX CONTROL 
	CreateListBox(phWnd);

	//used to setfocus on ComboBox
	SetFocus(GetDlgItem(phWnd,IDC_COMBOBOX));	
}

/**
 * CreateGroups() used to create groups for all the controls 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Parent Window
 * @param [in]     hwndCheckBox		contains the address of the check box control
 * 
 */
void CreateGroups(HWND phWnd)
{
	//CREATES THE OUTLINE FOR THE FIRST GROUP
	CreateWindow("Button", "SUBJECT DETAILS" , WS_CHILD | WS_VISIBLE |BS_GROUPBOX,30,50,400,250,phWnd,
		(HMENU)IDC_GROUP1,ghInstance,NULL); 

	//CREATES THE OUTLINE FOR THE SECOND GROUP
	CreateWindow("Button", "STUDENT DETAILS" , WS_CHILD | WS_VISIBLE |BS_GROUPBOX,500,50,400,
		250,phWnd,(HMENU)IDC_GROUP2,ghInstance,NULL);	

	//CREATES THE OUTLINE FOR THE THIRD GROUP
	CreateWindow("Button", "HOBBIES" , WS_CHILD | WS_VISIBLE |BS_GROUPBOX,30,320,670,
		120,phWnd,(HMENU)IDC_GROUP3,ghInstance,NULL); 

}


/**
 * CreateRadioButtonfirstgroup() used to create Radio Button for first Group Box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Window
 * @param [in]     button_no		Contains the Radio button no.
 *   
 */

void CreateRadioButtonfirstgroup(HWND phWnd,int y_axis)
{

		int i,x_axis;
		char *radiobuttonnames[]={"I","II","III"};

	//this loop creates the three radiobuttons.
	for(i=ZERO,x_axis=50;i<THREE;i++,x_axis+=90){

		CreateWindow("Button",radiobuttonnames[i],WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTORADIOBUTTON,

			x_axis,y_axis,50,100,phWnd,(HMENU)(IDC_RADIO11+i),ghInstance,NULL);
	}   	
}

/**
 * CreateRadioButtonsecondgroup() used to create Radio Button for second Group Box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Window
 * @param [in]     button_no		Contains the Radio button no.
 *   
 */

void CreateRadioButtonsecondgroup(HWND hWnd,int y_axis)
{

		int i,x_axis;
		char *radiobuttonnames1[]={"Mr","Miss","Mrs"};

	//this loop creates the three radiobuttons.
	for(i=ZERO,x_axis=520;i<THREE;i++,x_axis+=90){

		CreateWindow("Button",radiobuttonnames1[i],WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTORADIOBUTTON,

			x_axis,y_axis,50,100,hWnd,(HMENU)(IDC_RADIO21+i),ghInstance,NULL);
	}
}

/**
 * Createopbutton() used to create submit and reset button 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Window
 * @param [in]     button_no		Contains the Radio button no.
 *   
 */

void Createopbutton(HWND phWnd)
{    
	//CREATES THE SUBMIT BUTTON
	CreateWindow("Button","Submit",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_TABSTOP ,350,600,100,30,
		phWnd,(HMENU)IDC_SUBMIT,ghInstance,NULL);

	//CREATES THE RESET BUTTON
	CreateWindow("Button","Reset",WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_TABSTOP ,550,600,100,30,
		phWnd,(HMENU)IDC_RESET,ghInstance,NULL);

}

/**
 * CreateCheckBox() creates the check boxes
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Parent Window
 * @param [in]     hwndCheckBox		contains the address of the check box control
 * 
 */

void CreateCheckBox(HWND phWnd,int y_axis)
{

		int x_axis,i;
		char *checkboxnames[]={"Singing","Dancing","Sketching","Drawing"};


	//this loop creates the four checkboxes.
	for(i=ZERO,x_axis=50;i<FOUR;i++,x_axis+=150){

		CreateWindow("Button",checkboxnames[i],WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,
			x_axis,390,100,15,phWnd,(HMENU)(IDC_CHECKBOX1+i),ghInstance,NULL);

	}
}

/**
 * CreateComboBox() used to create Combo Box For first Group Box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 *  
 */

void CreateComboBox(HWND phWnd)
{
	
		char	*comboitem[] = {	"SELECT",
									"MATHS",
									"BIOLOGY",
									"COMMERCE",
		                            "ARTS"      };
		int		i;

	CreateWindow(	"COMBOBOX",
					"",
					CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE |WS_TABSTOP|WS_GROUP,				
					70,
					100,
					200,
					500,
					phWnd,
					(HMENU) IDC_COMBOBOX,
					ghInstance,
					NULL);

	//Add items in the Combo Box
	for(i=ZERO;i<FIVE;i++)
		SendMessage(GetDlgItem(phWnd,IDC_COMBOBOX), CB_ADDSTRING, 0, (LPARAM) comboitem[i]); 

}

/**
 * CreateEditBox() used to create submit and reset button 
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Window
 * @param [in]     button_no		Contains the Radio button no.
 *   
 */

void CreateEditBox(HWND phWnd)
{    
	//CREATES THE EDIT BOX
	CreateWindow("Edit",NULL,WS_CHILD|WS_VISIBLE |WS_BORDER|WS_GROUP|WS_TABSTOP,700,70,180,40,
		phWnd,(HMENU)IDC_EDITBOX,  ghInstance,NULL);
}

/**
 * CreateListBox() used to create List Box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Window
 *   
 */

void CreateListBox(HWND phWnd)
{
	//CREATES THE LIST BOX
	CreateWindow("ListBox",NULL,WS_CHILD|WS_VISIBLE |WS_BORDER |WS_VSCROLL ,										              200,480,200,50,phWnd,(HMENU)IDC_LISTBOX,ghInstance,NULL);
}

/**
 * AddItemsToListBox() used to retrieve text from all the controls and add those in List Box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 */

void AddItemsInListBox(HWND phWnd)
{
	//CALL FUNCTION TO RETRIEVE THE SELECTED STRING FROM THE COMBO BOX
	RetrieveTextFromComboBox(phWnd);						

	//CALL FUNCTION TO CHECK THAT WEATHER THE CHECK BOX 1 SELECTD OR N0T
	checkbox(GetDlgItem(phWnd,IDC_CHECKBOX1),phWnd);	

	//CALL FUNCTION TO CHECK THAT WEATHER THE CHECK BOX 2 IS SELECTD OR N0T
	checkbox(GetDlgItem(phWnd,IDC_CHECKBOX2),phWnd);	

	//CALL FUNCTION TO CHECK THAT WEATHER THE CHECK BOX 3 IS SELECTD OR N0T
	checkbox(GetDlgItem(phWnd,IDC_CHECKBOX3),phWnd);		

	//CALL FUNCTION TO CHECK THAT WEATHER THE CHECK BOX 3 IS SELECTD OR N0T
	checkbox(GetDlgItem(phWnd,IDC_CHECKBOX4),phWnd);		
												
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 1 OF RADIO GROUP 1 IS SELECTD OR N0T
	radiocheck(IDC_RADIO11,phWnd);	
	
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 2 OF RADIO GROUP 1 IS SELECTD OR N0T
	radiocheck(IDC_RADIO12,phWnd);		
					
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 3 OF RADIO GROUP 1 IS SELECTD OR N0T
	radiocheck(IDC_RADIO13,phWnd);		
						
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 1 OF RADIO GROUP 2 IS SELECTD OR N0T
	radiocheck(IDC_RADIO21,phWnd);		
								
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 2 OF RADIO GROUP 2 IS SELECTD OR N0T
	radiocheck(IDC_RADIO22,phWnd);	
				
	//CALL FUNCTION TO CHECK THAT WEATHER THE RADIO BUTTON 3 OF RADIO GROUP 2 IS SELECTD OR N0T
	radiocheck(IDC_RADIO23,phWnd);
								
	//CALL FUNCTION TO CHECK THAT WEATHER THE TEXT IS AVILABLE IN EDIT CONTROL OR N0T
	EditText(phWnd);
}
/**
 * radiocheck() used to check whether or not Radio Buttons are checked
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd			Contains the address of the Window
 * @param [in]     Button_no	Identifier of Radio Button.
 * 
 */

void radiocheck(int pButton_no,HWND phWnd)
{
	
		char*		edittext;     
		int			i;

	//RETURN TRUE IF RADIO BUTTON IS SELECTED
	if(SendMessage( GetDlgItem(phWnd,pButton_no),BM_GETCHECK,0,0) == BST_CHECKED)	{				
	
		//Retrieve the length of the text associated with the CheckBox 
		i = GetWindowTextLength( GetDlgItem(phWnd,pButton_no));
		
		//Allocate memory of i length
		edittext = (char*)calloc(10,(i+1)*sizeof(char));
		
		//Retrieve the string associated with the Radio Buttons 
		GetWindowText( GetDlgItem(phWnd,pButton_no), edittext,i+1);
		
		//Add that string in to the List box
		SendMessage(GetDlgItem(phWnd,IDC_LISTBOX),LB_ADDSTRING,0,(LPARAM) edittext);
		free(edittext);
	}	
}

/**
 * EditText() used to retrieve text from Edit Text control of first group box and add it to the list box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 *  
 */



void EditText(HWND phWnd)
{
	
		char	*edittext;				///<contains the address of the Buffer										
		
	//call RetrieveText() to retrieve text from the Edit Control of First Group Box
	edittext=RetrieveText( GetDlgItem(phWnd,IDC_EDITBOX));

	//add that string in the List box
	SendMessage(GetDlgItem(phWnd,IDC_LISTBOX),LB_ADDSTRING,0,(LPARAM) edittext);
	free(edittext);	
}
/**
 * RetrieveTextFromComboBox() used to retrieve selected text from combo box
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 *  
 */
void RetrieveTextFromComboBox(HWND phWnd)
{
	
		char	*edittext;
		int		CBIndex;

	(LRESULT) CBIndex = SendMessage(GetDlgItem(phWnd,IDC_COMBOBOX),CB_GETCURSEL,0,0);
	
	//checks whether or not any value is selected in the combo box if not retuns -1
	if (CBIndex != -1) {
		
		edittext= RetrieveText( GetDlgItem(phWnd,IDC_COMBOBOX));
		//ADD THE RETRIEVED TEXT TO THE LIST BOX 
		SendMessage(GetDlgItem(phWnd,IDC_LISTBOX),LB_ADDSTRING,0,(LPARAM) edittext);
		
		free(edittext);
	}
}

/**
 * RetrieveText() used to retireve text from th edit control
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd		Contains the address of the Window
 *
 */

char* RetrieveText(HWND hWnd)
{
		char		*edittext;
		int			textlength;

	
	//Retrieve text length 
	(LRESULT) textlength = SendMessage( hWnd,WM_GETTEXTLENGTH,0,0);
	edittext=(char*)calloc(textlength+1,sizeof(char));
	
	//retrieve the string entered in the Edit Control 
	SendMessage( hWnd,WM_GETTEXT,textlength+1,(LPARAM) edittext);

	return edittext;
}
/**
 * checkbox() used to check whether or not check boxes are checked
 *
 * This continues into the more detailed description of the function.
 * 
 * @param [in]     hWnd				Contains the address of the Parent Window
 * @param [in]     hwndCheckBox		contains the address of the check box control
 * 
 */

void checkbox(HWND phwndCheckBox , HWND phWnd)
{
		
		char *		edittext;
		int			i;

	//RETURN TRUE IF CHECK BOX IS SELECTED
	if(SendMessage(phwndCheckBox,BM_GETCHECK,0,0)==BST_CHECKED)	{			
	
		//retrieve the length of the text associated with the CheckBox 
		i = GetWindowTextLength(phwndCheckBox);
		
		//Allocate memory of i length
		edittext = (char*)calloc(10,(i+1)*sizeof(char));
		
		//retrieve the string associated with the CheckBox 
		i = GetWindowText( phwndCheckBox, edittext,(i+1) );
		
		//add that string in to the List box
		SendMessage(GetDlgItem( phWnd, IDC_LISTBOX ),LB_ADDSTRING,0,(LPARAM) edittext);
		
		free(edittext);
	}
}

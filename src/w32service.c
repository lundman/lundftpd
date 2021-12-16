
#include <windows.h>
//#include <iostream.h>
#include <stdio.h>


// Source contributed by Nameless. Don't blame me, Lund.

/*

  in main, if switch is whatever, call 'InstallService' or 
'UninstallService'. if there are no switches, call 'StartedByService'

add the ftpd start code to  'ServiceThread'

and things should hopefully work.

  */


// thingies
FILE*		               g_pLog = NULL;							
LPTSTR		               g_pzServiceName = "LundFTPd\0";
SERVICE_STATUS_HANDLE   g_serviceStatusHandle = NULL;
BOOL		               g_runningService = FALSE;
BOOL		               g_pauseService = FALSE;
HANDLE                  g_terminateEvent = NULL;
HANDLE		            g_threadHandle = 0;				// Thread for the actual work

// crappy c style forward declaration. 
BOOL lftpdSetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);
void	LftpdServiceMain(DWORD argc, LPTSTR *argv);
void StopService();
DWORD WINAPI ServiceThread(LPDWORD lParam);

int    realmain(int argc, char **argv);

struct ErrEntry {
	int code;
	const char* msg;
};
struct ErrEntry ErrList[] = {
   // error codes stolen from :
   // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/debug/base/system_error_codes.asp
	{ 0,	"No error" },
	{ 1055,	"The service database is locked." },
	{ 1056,	"An instance of the service is already running." },
	{ 1060, "The service does not exist as an installed service." },
	{ 1061,	"The service cannot accept control messages at this time." },
	{ 1062, "The service has not been started." },
	{ 1063, "The service process could not connect to the service controller." },
	{ 1064,	"An exception occurred in the service when handling the control request." },
	{ 1065,	"The database specified does not exist." },
	{ 1066,	"The service has returned a service-specific error code." },
	{ 1067,	"The process terminated unexpectedly." },
	{ 1068,	"The dependency service or group failed to start." },
	{ 1069,	"The service did not start due to a logon failure." },
	{ 1070,	"After starting, the service hung in a start-pending state." },
	{ 1071,	"The specified service database lock is invalid." },
	{ 1072, "The service marked for deletion." },
	{ 1073, "The service already exists." },
	{ 1078,	"The name is already in use as either a service name or a service display name." },
};
const int nErrList = sizeof(ErrList) / sizeof(struct ErrEntry);

void ErrorHandler(char *s, int err)
{
	int i;
	for (i = 0; i < nErrList; ++i) {
		if (ErrList[i].code == err) {
         // report error message
			break;
		}
	}
	if (i == nErrList) {
      // report unknown error
	}

	if (!g_pLog)
		g_pLog = fopen("lftpdserver.log","a");

	if (g_pLog) {
		fprintf(g_pLog, "%s failed, error code = %d\n",s , err); 
		fclose(g_pLog);
	}

	ExitProcess(err);
}


void StartedByService()
{
   BOOL success;
   SERVICE_TABLE_ENTRY serviceTable[] =
   {
	   { g_pzServiceName, (LPSERVICE_MAIN_FUNCTION) LftpdServiceMain},
	   { NULL, NULL}
   };

   success = StartServiceCtrlDispatcher(serviceTable);
	if (!success)
		ErrorHandler("StartServiceCtrlDispatcher",GetLastError());
}

void ResumeService()
{
	g_pauseService = FALSE;
	ResumeThread(g_threadHandle);
}

//pauses service
void PauseService()
{
	g_pauseService = TRUE;
	SuspendThread(g_threadHandle);
}

void ServiceCtrlHandler(DWORD controlCode)
{
	DWORD currentState = SERVICE_RUNNING;
	BOOL success;

	switch(controlCode)
	{
		case SERVICE_CONTROL_STOP:
			currentState = SERVICE_STOP_PENDING;
			//notify SCM
			success = lftpdSetServiceStatus(
				SERVICE_STOP_PENDING,
				NO_ERROR,
				0,
				1,
				5000);
			StopService();
			return;
		case SERVICE_CONTROL_PAUSE:
			if (g_runningService && !g_pauseService)
			{
				//notify SCM
				success = lftpdSetServiceStatus(
					SERVICE_PAUSE_PENDING,
					NO_ERROR,
					0,
					1,
					1000);
				PauseService();
				currentState = SERVICE_PAUSED;
			}
			break;
		case SERVICE_CONTROL_CONTINUE:
			if (g_runningService && g_pauseService)
			{
				success = lftpdSetServiceStatus(
					SERVICE_CONTINUE_PENDING,
					NO_ERROR,
					0,
					1,
					1000);
				ResumeService();
				currentState = SERVICE_RUNNING;
			}
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
			
		case SERVICE_CONTROL_SHUTDOWN:
			//do nothing
			return;
		default:
			break;
	}
	//notify SCM current state
	lftpdSetServiceStatus(currentState, NO_ERROR, 0, 0, 0);
}
void Terminate(DWORD error)
{
	//close event handle
	if (g_terminateEvent)
		CloseHandle(g_terminateEvent);

	//notify SCM service stopped
	if (g_serviceStatusHandle)
		lftpdSetServiceStatus(SERVICE_STOPPED, error, 0, 0, 0);

	//close thread handle
	if (g_threadHandle)
		CloseHandle(g_threadHandle);
}

BOOL InitService()
{
	DWORD id;

	// Start the service's thread
	g_threadHandle = CreateThread(
	NULL,
	0,
	(LPTHREAD_START_ROUTINE) ServiceThread,
	NULL,
	0,
	&id);
	
	if (g_threadHandle == 0)
		return FALSE;
	else
	{
		g_runningService = TRUE;
		return TRUE;
	}
}

void LftpdServiceMain(DWORD argc, LPTSTR *argv)
{
	BOOL success;


	g_serviceStatusHandle = RegisterServiceCtrlHandler(g_pzServiceName, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
	if (!g_serviceStatusHandle)
	{
		Terminate(GetLastError());
		return;
	}


	success = lftpdSetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 0 , 1, 5000);
	if (!success)
	{ 
		Terminate(GetLastError());
		return;
	}

	//create termination event
	g_terminateEvent = CreateEvent (0, TRUE, FALSE, 0);
	if (!g_terminateEvent)
	{
		Terminate(GetLastError());
		return;
	}

	success = lftpdSetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 0 , 2, 1000);
	if (!success)
	{ 
		Terminate(GetLastError());
		return;
	}

	success = lftpdSetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 0 , 3, 5000);
	if (!success)
	{ 
		Terminate(GetLastError());
		return;
	}

	//start service
	success = InitService();
	if (!success)
	{ 
		Terminate(GetLastError());
		return;
	}

	//notify SCM service is runnning
	success = lftpdSetServiceStatus(SERVICE_RUNNING, NO_ERROR, 0 , 0, 0);
	if (!success)
	{ 
		Terminate(GetLastError());
		return;
	}

	//wait for stop signal and then terminate
	WaitForSingleObject(g_terminateEvent, INFINITE);

	Terminate(0);
}

void GetStatus(SC_HANDLE service)
{
	SERVICE_STATUS status;	
	DWORD dwCurrentState;


	QueryServiceStatus(service, &status);


	switch(status.dwCurrentState)
	{
		case SERVICE_RUNNING:
			dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_STOPPED:
			dwCurrentState = SERVICE_STOPPED;
			break;
		case SERVICE_PAUSED:
			dwCurrentState = SERVICE_PAUSED;
			break;
		case SERVICE_CONTINUE_PENDING:
			dwCurrentState = SERVICE_CONTINUE_PENDING;
			break;
		case SERVICE_PAUSE_PENDING:
			dwCurrentState = SERVICE_PAUSE_PENDING;
			break;
		case SERVICE_START_PENDING:
			dwCurrentState = SERVICE_START_PENDING;
			break;
		case SERVICE_STOP_PENDING:
			dwCurrentState = SERVICE_STOP_PENDING;
			break;
		default:
         // oops
			break;
	}
	lftpdSetServiceStatus(dwCurrentState, NO_ERROR, 0, 0, 0);
}

BOOL ServiceRun() 
{ 
    SC_HANDLE scm, Service;
	 SERVICE_STATUS ssStatus; 
    DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwStatus;
 	
	
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!scm)
		ErrorHandler("OpenSCManager", GetLastError());

	


	
	Service = OpenService(scm, g_pzServiceName, SERVICE_ALL_ACCESS);
	if(!Service)
	{
		ErrorHandler("OpenService", GetLastError());
		return FALSE;
	}
	else
	{
		//start service
		StartService(Service, 0, NULL);
		GetStatus(Service);

		// Check the status until the service is no longer start pending. 
		if (!QueryServiceStatus( Service, &ssStatus) )
			ErrorHandler("QueryServiceStatus", GetLastError());
		dwStartTickCount = GetTickCount();
		dwOldCheckPoint = ssStatus.dwCheckPoint;



		while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
		{ 
         // no less than 1 second, no more than 10
			dwWaitTime = ssStatus.dwWaitHint / 10;

			if( dwWaitTime < 1000 )
				dwWaitTime = 1000;
			else if ( dwWaitTime > 10000 )
				dwWaitTime = 10000;



			Sleep( dwWaitTime );

			// Check the status again. 
			if (!QueryServiceStatus(Service, &ssStatus) )
				break; 

			if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
			{
				// The service is making progress.
				dwStartTickCount = GetTickCount();
				dwOldCheckPoint = ssStatus.dwCheckPoint;
			}
			else
			{
				if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
				{
					// No progress made within the wait hint
					break;
				}
			}
		}

		
		if (ssStatus.dwCurrentState == SERVICE_RUNNING) 
		{
			GetStatus(Service);
			dwStatus = NO_ERROR;
		}
		else 
		{ 
			dwStatus = GetLastError();
         // report error?
		} 	
	}


	CloseServiceHandle(scm);
   CloseServiceHandle(Service); 
   return TRUE;
}

BOOL InstallService(unsigned long addy)
{
	SC_HANDLE newService;
	SC_HANDLE scm; 
    char szBuffer[MAX_PATH];
    char szPath[MAX_PATH];
    char szCwd[MAX_PATH];


	GetModuleFileName( GetModuleHandle(NULL), szPath, MAX_PATH );

	getcwd(szCwd, sizeof(szCwd));


	if (addy)
	    snprintf(szBuffer, sizeof(szBuffer), "\"%s\" -r \"%s\" -S \"%s\"",
			szPath, lion_ntoa(addy), szCwd);
	else
	    snprintf(szBuffer, sizeof(szBuffer), "\"%s\" -S \"%s\"",
			szPath, szCwd);


	//get SCM
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
		ErrorHandler("OpenSCManager", GetLastError());



	//install service
	newService = CreateService(
		scm,
		g_pzServiceName,
		g_pzServiceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		szBuffer,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	if(!newService)
	{
		ErrorHandler("CreateService", GetLastError());
		return FALSE;
	}
	else
	{
		ServiceRun();
	}

	//clean up
	CloseServiceHandle(newService);
	CloseServiceHandle(scm);

	return TRUE;
}

DWORD WINAPI ServiceThread(LPDWORD lParam)
{
	
//	_asm{int 3};
	
	// start the ftpd thingie here
	realmain(0, NULL);
	//loop();
	return 0;
}

BOOL UninstallService()
{
	SC_HANDLE service;
	SC_HANDLE scm;
	BOOL success;
	SERVICE_STATUS status;

	//get SCM
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
		ErrorHandler("OpenSCManager", GetLastError());

	//Get service's handle
	service = OpenService(scm, g_pzServiceName, SERVICE_ALL_ACCESS | DELETE);
	if (!service)
		ErrorHandler("OpenService", GetLastError());

	success= QueryServiceStatus(service, &status);
	if (!success)
		ErrorHandler("QueryServiceStatus", GetLastError());
	
	//Stop service if necessary		
	if (status.dwCurrentState != SERVICE_STOPPED)
	{
		success= ControlService(service, SERVICE_CONTROL_STOP, &status);
		if (!success)
			ErrorHandler("ControlService", GetLastError());
		Sleep(500);
	}

	//Delete service
	success= DeleteService(service);
	if (!success)
		ErrorHandler("DeleteService", GetLastError());

	//Clean up
	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return TRUE;
}



void StopService()
{
	g_runningService = FALSE;
	//set the event that is holding ServiceMain
	SetEvent(g_terminateEvent);
}

BOOL lftpdSetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	BOOL success;
	SERVICE_STATUS serviceStatus;

	//fill in all of the SERVICE_STATUS fields
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;

	//if in the process of something, then accept
	//no control events, else accept anything
	if (dwCurrentState == SERVICE_START_PENDING)
		serviceStatus.dwControlsAccepted = 0;
	else
		serviceStatus.dwControlsAccepted = 
			SERVICE_ACCEPT_STOP | 
			SERVICE_ACCEPT_PAUSE_CONTINUE |
			SERVICE_ACCEPT_SHUTDOWN;

	//if a specific exit code is defines, set up the win32 exit code properly
	if (dwServiceSpecificExitCode == 0)
		serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
	else
		serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
	
	serviceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
	serviceStatus.dwCheckPoint = dwCheckPoint;
	serviceStatus.dwWaitHint = dwWaitHint;
	
	g_serviceStatusHandle = RegisterServiceCtrlHandler(g_pzServiceName, (LPHANDLER_FUNCTION)ServiceCtrlHandler);
	if (!g_serviceStatusHandle)
	{
		Terminate(GetLastError());
		return -1;
	}
	success = SetServiceStatus (g_serviceStatusHandle, &serviceStatus);

	
	if (!success)
		StopService();

	return success;
}


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

LPCWSTR szSvcName;
LPCWSTR szPath;
SC_HANDLE schSCManager;
SC_HANDLE schService;
CHAR failureOption[50];

LPCWSTR binpath = NULL;
LPCWSTR password = NULL;
LPCWSTR displayname = NULL;
CHAR type[50] = "";
CHAR state[50] = "";
CHAR start2[50];
CHAR error[50];
CHAR reset[50] = "";
CHAR reboot[100] = "";
CHAR action[100] = "";
CHAR command[100] = "";


BOOL StopDependentServices();

void create() {

	schSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == schSCManager) 
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	schService = CreateService(
		schSCManager,
		szSvcName,
		szSvcName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		binpath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (schService == NULL)
	{
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service installed sucessfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

void qdescription()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    LPSERVICE_DESCRIPTION lpsd;
    lpsd = NULL;
    DWORD dwBytesNeeded;
    DWORD cbBufSize;
    DWORD dwError;

    schSCManager = OpenSCManager(
        NULL,                    
        NULL,                    
        SC_MANAGER_ALL_ACCESS);  

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,           
        szSvcName,             
        SERVICE_QUERY_CONFIG);  

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!QueryServiceConfig2(
        schService,
        SERVICE_CONFIG_DESCRIPTION,
        NULL,
        0,
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig2 failed (%d)", dwError);
            goto cleanup;
        }
    }

    if (!QueryServiceConfig2(
        schService,
        SERVICE_CONFIG_DESCRIPTION,
        (LPBYTE)lpsd,
        cbBufSize,
        &dwBytesNeeded))
    {
        printf("QueryServiceConfig2 failed (%d)", GetLastError());
        goto cleanup;
    }

    _tprintf(TEXT("\n\nSERVICE_NAME: %s  \n"), szSvcName);
    if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
        _tprintf(TEXT("Description: %s\n"), lpsd->lpDescription);

    LocalFree(lpsd);

cleanup:
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void query()
{
    DWORD dwBytesNeeded;
    SERVICE_STATUS_PROCESS ssStatus;

    schSCManager = OpenSCManager(
        NULL,                    
        NULL,                    
        SC_MANAGER_ALL_ACCESS);   

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,         
        szSvcName,             
        SERVICE_ALL_ACCESS);  

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!QueryServiceStatusEx(
        schService,                     
        SC_STATUS_PROCESS_INFO,         
        (LPBYTE)&ssStatus,             
        sizeof(SERVICE_STATUS_PROCESS), 
        &dwBytesNeeded))              
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    _tprintf(TEXT("\nSERVICE_NAME: %s  \n"), szSvcName);
    printf("        TYPE               : %x\n", ssStatus.dwServiceType);
    printf("        STATE              : %d  (%s) \n", ssStatus.dwCurrentState,
        (ssStatus.dwCurrentState == SERVICE_STOPPED) ? "STOPPED" :
        (ssStatus.dwCurrentState == SERVICE_RUNNING) ? "RUNNING" :
        (ssStatus.dwCurrentState == SERVICE_PAUSED) ? "PAUSED" :
        (ssStatus.dwCurrentState == SERVICE_START_PENDING) ? "START_PENDING" :
        (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) ? "STOP_PENDING" :
        (ssStatus.dwCurrentState == SERVICE_CONTINUE_PENDING) ? "CONTINUE_PENDING" :
        (ssStatus.dwCurrentState == SERVICE_PAUSE_PENDING) ? "PAUSE_PENDING" : "UNKNOWN");
    printf("        WIN32_EXIT_CODE    : %d  (0x%x) \n", ssStatus.dwWin32ExitCode, ssStatus.dwWin32ExitCode);
    printf("        SERVICE_EXIT_CODE  : %d  (0x%x) \n", ssStatus.dwServiceSpecificExitCode, ssStatus.dwServiceSpecificExitCode);
    printf("        CHECKPOINT         : 0x%x \n", ssStatus.dwCheckPoint);
    printf("        WAIT_HINT          : 0x%x \n", ssStatus.dwWaitHint);


    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}
VOID config()
{

    //get the current service configuration
    DWORD dwBytesNeeded, cbBufSize, dwError;
    LPQUERY_SERVICE_CONFIG lpsc = NULL;

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,         // SCM database 
        szSvcName,            // name of service 
        SERVICE_QUERY_CONFIG);  // full access 

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    if (!QueryServiceConfig(
        schService,
        NULL,
        0,
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if (ERROR_INSUFFICIENT_BUFFER == dwError)
        {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig failed (%d)", dwError);
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }
    }

    if (!QueryServiceConfig(
        schService,
        lpsc,
        cbBufSize,
        &dwBytesNeeded))
    {
        printf("QueryServiceConfig failed (%d)", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }


    CloseServiceHandle(schService);

    //initialize the current configuration in variables
    DWORD     ServiceType = lpsc->dwServiceType;
    DWORD     StartType = lpsc->dwStartType;
    DWORD     Error = lpsc->dwErrorControl;
    LPCWSTR   BinaryPath = lpsc->lpBinaryPathName;
    LPCWSTR   Password = NULL;
    LPCWSTR   Display = lpsc->lpDisplayName;

    //set binary path
    if (binpath != NULL)
    {
        BinaryPath = binpath;
    }

    //set password
    if (password != NULL)
    {
        Password = password;
    }

    //set displayname
    if (displayname != NULL)
    {
        Display = displayname;
    }

    //set service type
    if (strcmp(type, "own") == 0)
    {
        ServiceType = SERVICE_WIN32_OWN_PROCESS;
    }
    else if (strcmp(type, "share") == 0)
    {
        ServiceType = SERVICE_WIN32_SHARE_PROCESS;
    }
    else if (strcmp(type, "kernel") == 0)
    {
        ServiceType = SERVICE_KERNEL_DRIVER;
    }
    else if (strcmp(type, "filesys") == 0)
    {
        ServiceType = SERVICE_FILE_SYSTEM_DRIVER;
    }
    else if (strcmp(type, "rec") == 0)
    {
        ServiceType = SERVICE_RECOGNIZER_DRIVER;
    }

    //set start type of service
    if (strcmp(start2, "boot") == 0)
    {
        StartType = SERVICE_BOOT_START;
    }
    else if (strcmp(start2, "system") == 0)
    {
        StartType = SERVICE_SYSTEM_START;
    }
    else if (strcmp(start2, "auto") == 0)
    {
        StartType = SERVICE_AUTO_START;
    }
    else if (strcmp(start2, "demand") == 0)
    {
        StartType = SERVICE_DEMAND_START;
    }
    else if (strcmp(start2, "disabled") == 0)
    {
        StartType = SERVICE_DISABLED;
    }


    //set error type
    if (strcmp(error, "normal") == 0)
    {
        Error = SERVICE_ERROR_NORMAL;
    }
    else if (strcmp(error, "severe") == 0)
    {
        Error = SERVICE_ERROR_SEVERE;
    }
    else if (strcmp(error, "critical") == 0)
    {
        Error = SERVICE_ERROR_CRITICAL;
    }
    else if (strcmp(error, "ignore") == 0)
    {
        Error = SERVICE_ERROR_IGNORE;
    }


    // Get a handle to the service.

    schService = OpenService(
        schSCManager,            // SCM database 
        szSvcName,               // name of service 
        SERVICE_CHANGE_CONFIG);  // need change config access 

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // Change the service sconfiguration

    if (!ChangeServiceConfig(
        schService,        // handle of service 
        ServiceType, // service type:  
        StartType,  // service start type 
        Error, // error control: 
        BinaryPath,              // binary path:
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change 
        Password,              // password: 
        Display))            // display name: 
    {
        printf("ChangeServiceConfig failed (%d)\n", GetLastError());
    }
    else printf("Service configuration changed successfully.\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void  start()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;


    schSCManager = OpenSCManager(
        NULL,                    
        NULL,                     
        SC_MANAGER_ALL_ACCESS);  

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,         
        szSvcName,            
        SERVICE_ALL_ACCESS);   

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    } 

    if (!QueryServiceStatusEx(
        schService,                      
        SC_STATUS_PROCESS_INFO,         
        (LPBYTE)&ssStatus,             
        sizeof(SERVICE_STATUS_PROCESS), 
        &dwBytesNeeded))              
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        printf("Cannot start the service because it is already running\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;


    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,                     
            SC_STATUS_PROCESS_INFO,         
            (LPBYTE)&ssStatus,             
            sizeof(SERVICE_STATUS_PROCESS), 
            &dwBytesNeeded))              
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                printf("Timeout waiting for service to stop\n");
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return;
            }
        }
    }

    if (!StartService(
        schService,  
        0,            
        NULL))    
    {
        printf("StartService failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service start pending...\n");

    if (!QueryServiceStatusEx(
        schService,                      
        SC_STATUS_PROCESS_INFO,         
        (LPBYTE)&ssStatus,             
        sizeof(SERVICE_STATUS_PROCESS), 
        &dwBytesNeeded))              
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }


    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);


        if (!QueryServiceStatusEx(
            schService,             
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssStatus,             
            sizeof(SERVICE_STATUS_PROCESS), 
            &dwBytesNeeded))             
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                break;
            }
        }
    }

    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        printf("Service started successfully.\n");
    }
    else
    {
        printf("Service not started. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void stop()
{
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000;
    DWORD dwWaitTime;

    schSCManager = OpenSCManager(
        NULL,                    
        NULL,                     
        SC_MANAGER_ALL_ACCESS);  

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,          
        szSvcName,             
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
    {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        goto stop_cleanup;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
    {
        printf("Service is already stopped.\n");
        goto stop_cleanup;
    }

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
    {
        printf("Service stop pending...\n"); 

        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            printf("Service stopped successfully.\n");
            goto stop_cleanup;
        }

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            printf("Service stop timed out.\n");
            goto stop_cleanup;
        }
    }

    StopDependentServices();

    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp))
    {
        printf("ControlService failed (%d)\n", GetLastError());
        goto stop_cleanup;
    }

    while (ssp.dwCurrentState != SERVICE_STOPPED)
    {
        Sleep(ssp.dwWaitHint);
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            printf("Wait timed out\n");
            goto stop_cleanup;
        }
    }
    printf("Service stopped successfully\n");

stop_cleanup:
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

BOOL StopDependentServices()
{
    DWORD i;
    DWORD dwBytesNeeded;
    DWORD dwCount;

    LPENUM_SERVICE_STATUS   lpDependencies = NULL;
    ENUM_SERVICE_STATUS     ess;
    SC_HANDLE               hDepService;
    SERVICE_STATUS_PROCESS  ssp;

    DWORD dwStartTime = GetTickCount();
    DWORD dwTimeout = 30000; 

    if (EnumDependentServices(schService, SERVICE_ACTIVE,
        lpDependencies, 0, &dwBytesNeeded, &dwCount))
    {
        return TRUE;
    }
    else
    {
        if (GetLastError() != ERROR_MORE_DATA)
            return FALSE;

        lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

        if (!lpDependencies)
            return FALSE;

        __try {
            if (!EnumDependentServices(schService, SERVICE_ACTIVE,
                lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                &dwCount))
                return FALSE;

            for (i = 0; i < dwCount; i++)
            {
                ess = *(lpDependencies + i);
                hDepService = OpenService(schSCManager,
                    ess.lpServiceName,
                    SERVICE_STOP | SERVICE_QUERY_STATUS);

                if (!hDepService)
                    return FALSE;

                __try {
                    if (!ControlService(hDepService,
                        SERVICE_CONTROL_STOP,
                        (LPSERVICE_STATUS)&ssp))
                        return FALSE;

                    while (ssp.dwCurrentState != SERVICE_STOPPED)
                    {
                        Sleep(ssp.dwWaitHint);
                        if (!QueryServiceStatusEx(
                            hDepService,
                            SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&ssp,
                            sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded))
                            return FALSE;

                        if (ssp.dwCurrentState == SERVICE_STOPPED)
                            break;

                        if (GetTickCount() - dwStartTime > dwTimeout)
                            return FALSE;
                    }
                }
                __finally
                {
                    CloseServiceHandle(hDepService);
                }
            }
        }
        __finally
        {
            HeapFree(GetProcessHeap(), 0, lpDependencies);
        }
    }
    return TRUE;
}


void deleteSvc() 
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ssStatus;

    schSCManager = OpenSCManager(
        NULL,                    
        NULL,                     
        SC_MANAGER_ALL_ACCESS);  

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,       
        szSvcName,         
        DELETE);            

    if (schService == NULL)
    {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    if (!DeleteService(schService))
    {
        printf("DeleteService failed (%d)\n", GetLastError());
    }
    else printf("Service deleted successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

}

void failure() {
    schSCManager = OpenSCManager(
        NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    schService = OpenService(
        schSCManager,
        szSvcName,
        SERVICE_ALL_ACCESS);

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }

    // Configure the failure actions
    SERVICE_FAILURE_ACTIONS failureActions = { 0 };
    failureActions.dwResetPeriod = 60; // Reset the failure counter after 60 seconds

    if (strcmp(failureOption, "restart") == 0) {
        failureActions.cActions = 1;  // Take 1 action on failure
        SC_ACTION restartAction;
        restartAction.Type = SC_ACTION_RESTART; // Restart the service
        restartAction.Delay = 5000;             // Wait 5 seconds before restarting
        failureActions.lpsaActions = &restartAction;
    }
    else if (strcmp(failureOption, "reboot") == 0) {
        failureActions.cActions = 1;  // Take 1 action on failure
        SC_ACTION rebootAction;
        rebootAction.Type = SC_ACTION_REBOOT; // Reboot the computer
        rebootAction.Delay = 5000;            // Wait 5 seconds before rebooting
        failureActions.lpsaActions = &rebootAction;
    }
    else {
        printf("Invalid failure action option.\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return;
    }

    // Update the failure configuration
    if (!ChangeServiceConfig2(
        schService,                         // handle to service
        SERVICE_CONFIG_FAILURE_ACTIONS,     // change: failure actions
        &failureActions)                    // new failure actions
        ) {
        printf("ChangeServiceConfig2 failed (%d)\n", GetLastError());
    }
    else {
        printf("Service failure actions updated successfully.\n");
    }

    // Cleanup
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

void main(int argc, CHAR* argv[])
{

    //return if arguments are less
    if (argc < 2)
    {
        printf("Invalid command\n");
        return;

    }

    wchar_t wtext[100];
    size_t outSize;

    wchar_t wtext1[100];
    size_t outSize1;

    wchar_t wtext2[100];
    size_t outSize2;

    wchar_t wtext3[100];
    size_t outSize3;

    int i;

    // sc query
    if (strcmp(argv[1], "query") == 0)
    {

        if (argc == 3)
        {
            //query specific service
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;
            query();
        }
        return;

    }

    // sc query
    if (strcmp(argv[1], "qdescription") == 0)
    {

        if (argc == 3)
        {
            //query specific service
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;
            qdescription();
        }
        return;

    }

    if (strcmp(argv[1], "create") == 0)
    {
        if (argc <= 4)
        {
            return;
        }
        else if (_strcmpi(argv[3], "binPath=") != 0)
        {
            //checking if binarypath parameter does not exist
            return;
        }
        else
        {
            //initiliaze the service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;

            //initiliaze the binary path
            mbstowcs_s(&outSize1, wtext1, strlen(argv[4]) + 1, argv[4], strlen(argv[4]));
            binpath = wtext1;
            create();
            return;
        }
    }

    //sc start
    if (strcmp(argv[1], "start") == 0)
    {
        if (argc == 3)
        {
            //initiliase service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;
            start();
            return;
        }
        printf("Invalid command\n");;
        return;
    }

    //sc stop
    if (strcmp(argv[1], "stop") == 0)
    {
        if (argc == 3)
        {
            //initiliase service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;
            stop();
            return;
        }
        printf("Invalid command\n");
        return;
    }

    //sc delete
    if (strcmp(argv[1], "delete") == 0)
    {
        if (argc == 3)
        {
            //initiliase service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;
            deleteSvc();
            return;
        }
        printf("Invalid command\n");
        return;
    }


    //sc failure
    if (strcmp(argv[1], "failure") == 0)
    {

        if (argc <= 3)
        {
            //return if insufficient parameters
            return;
        }
        else
        {
            //initiliase service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;

            for (i = 3; i < argc; i = i + 2)
            {

                //check  different flags
                if (strcmp(argv[i], "option=") == 0)
                {
                    strcpy_s(failureOption, sizeof(failureOption), argv[i + 1]);
                }
            }
            failure();
            return;
        }
    }

    //sc config
    if (strcmp(argv[1], "config") == 0)
    {

        if (argc <= 3)
        {
            //return if insufficient parameters
            return;
        }
        else
        {
            //initiliase service name
            mbstowcs_s(&outSize, wtext, strlen(argv[2]) + 1, argv[2], strlen(argv[2]));
            szSvcName = wtext;

            for (i = 3; i < argc; i = i + 2)
            {

                //check  different flags
                if (strcmp(argv[i], "start=") == 0)
                {
                    strcpy_s(start2, sizeof(start2), argv[i + 1]);
                }
                else if (strcmp(argv[i], "type=") == 0)
                {
                    strcpy_s(type, sizeof(type), argv[i + 1]);
                }
                else if (strcmp(argv[i], "error=") == 0)
                {
                    strcpy_s(error, sizeof(error), argv[i + 1]);
                }
                else if (_strcmpi(argv[i], "DisplayName=") == 0)
                {
                    mbstowcs_s(&outSize2, wtext2, strlen(argv[i + 1]) + 1, argv[i + 1], strlen(argv[i + 1]));
                    displayname = wtext2;
                }
                else if (_strcmpi(argv[i], "password=") == 0)
                {
                    mbstowcs_s(&outSize3, wtext3, strlen(argv[i + 1]) + 1, argv[i + 1], strlen(argv[i + 1]));
                    password = wtext3;
                }
                else if (_strcmpi(argv[i], "binPath=") == 0)
                {
                    mbstowcs_s(&outSize1, wtext1, strlen(argv[i + 1]) + 1, argv[i + 1], strlen(argv[i + 1]));
                    binpath = wtext1;
                }
            }
            config();
            return;
        }
    }



    }

   
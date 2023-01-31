// WorkbenchOS
// Copyright (c) 2022 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#include <Heap/Kmalloc.h>
#include <Memory/MemoryManager.h>
#include <UnbufferedConsole.h>

extern "C" {
#include <acpi.h>
}

static uint64_t divide_rounded_up(uint64_t value, uint64_t divisor) {
    return (value + (divisor - 1)) / divisor;
}

void *AcpiOsAllocate(ACPI_SIZE size) {
    auto result = g_malloc_heap.allocate(size);
    if(result.is_error()) {
        println("AcpiOsAllocate: Failed to allocate {} bytes", size);
        return nullptr;
    }
    return result.get().as_ptr();
}

void AcpiOsFree(void *address) {
    auto result = g_malloc_heap.free(VirtualAddress(address));
    if(result.is_error()) {
        println("AcpiOsFree: Failed to free {}", address);
    }
}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS where, ACPI_SIZE length) {
    auto &memory_manager = Kernel::MemoryManager::get_instance();
    auto size_in_pages = divide_rounded_up(length, 0x1000);


    return nullptr;
}

void AcpiOsUnmapMemory(void *logical_address, ACPI_SIZE size) {
}

ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress) {
    return AE_OK;
}

ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX *OutHandle) {
    return AE_OK;
}

void AcpiOsDeleteMutex(ACPI_MUTEX Handle) {
}

ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX Handle, UINT16 Timeout) {
    return AE_OK;
}

void AcpiOsReleaseMutex(ACPI_MUTEX Handle) {
}

ACPI_STATUS AcpiOsSignal(UINT32 Function, void *Info) {
    return AE_OK;
}

ACPI_THREAD_ID AcpiOsGetThreadId() {
    return 0;
}

UINT64 AcpiOsGetTimer() {
    return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *OutHandle) {
    return AE_OK;
}

void AcpiOsDeleteLock(ACPI_SPINLOCK Handle) {
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {
    return 0;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {
}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE *OutHandle) {
    return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {
    return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout) {
    return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {
    return AE_OK;
}

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 *Value, UINT32 Width) {
    return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS Address, UINT64 Value, UINT32 Width) {
    return AE_OK;
}

void AcpiOsPrintf(const char *Format, ...) {
}

void AcpiOsVprintf(const char *Format, va_list Args) {
}

ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *PciId, UINT32 Reg, UINT64 *Value, UINT32 Width) {
    return AE_OK;
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *PciId, UINT32 Reg, UINT64 Value, UINT32 Width) {
    return AE_OK;
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context) {
    return AE_OK;
}

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER ServiceRoutine, void *Context) {
    return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 InterruptNumber, ACPI_OSD_HANDLER ServiceRoutine) {
    return AE_OK;
}

void AcpiOsWaitEventsComplete() {
}

void AcpiOsSleep(UINT64 Milliseconds) {
}

void AcpiOsStall(UINT32 Microseconds) {
}

ACPI_STATUS AcpiOsEnterSleep(UINT8 SleepState, UINT32 RegaValue, UINT32 RegbValue) {
    return AE_OK;
}

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS Address, UINT32 *Value, UINT32 Width) {
    return AE_OK;
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS Address, UINT32 Value, UINT32 Width) {
    return AE_OK;
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *InitVal, ACPI_STRING *NewVal) {
    return AE_OK;
}

void *AcpiOsAcquireObject(ACPI_CACHE_T *Cache) {
    return nullptr;
}

ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T *Cache, void *Object) {
    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable) {
    return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_PHYSICAL_ADDRESS *NewAddress, UINT32 *NewTableLength) {
    return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer() {
    return AE_OK;
}

ACPI_STATUS AcpiOsCreateCache(char *CacheName, UINT16 ObjectSize, UINT16 MaxDepth, ACPI_CACHE_T **ReturnCache) {
    return AE_OK;
}

ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T *Cache) {
    return AE_OK;
}

ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T *Cache) {
    return AE_OK;
}

ACPI_STATUS AcpiOsInitialize() {
    return AE_OK;
}

ACPI_STATUS AcpiOsTerminate() {
    return AE_OK;
}
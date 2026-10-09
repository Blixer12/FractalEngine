#pragma once

#include "Defines.h"

// Holds a handle to a file.
typedef struct FileHandle {
    // Opaque handle to internal file handle.
    void* Handle;
    Bool8 IsValid;
} FileHandle;

typedef enum FileModes {
    FILE_MODE_READ      = 0x1,  // 0001 - Read access
    FILE_MODE_WRITE     = 0x2,  // 0010 - Write access (overwrites existing file)
    FILE_MODE_APPEND    = 0x4,  // 0100 - Append access (preserves file, writes to end)
    FILE_MODE_UPDATE    = 0x8,  // 1000 - Open existing file for read/write without truncating
} FileModes;

/**
 * Checks if a file with the given path exists.
 * @param Path The path of the file to be checked.
 * @returns True if exists; otherwise false.
 */
FAPI Bool8 FilesystemExists(const char* Path);

/** 
 * Attempt to open file located at path.
 * @param Path The path of the file to be opened.
 * @param Mode Mode flags for the file when opened (read/write). See FileModes enum in filesystem.h.
 * @param Binary Indicates if the file should be opened in binary mode.
 * @param Handle A pointer to a FileHandle structure which holds the handle information.
 * @returns True if opened successfully; otherwise false.
 */
FAPI Bool8 FilesystemOpen(const char* Path, FileModes Mode, Bool8 Binary, FileHandle* Handle);

/** 
 * Closes the provided handle to a file.
 * @param Handle A pointer to a FileHandle structure which holds the handle to be closed.
 */
FAPI void FilesystemClose(FileHandle* Handle);

/** 
 * Reads up to a newline or EOF
 * @param Handle A pointer to a FileHandle structure.
 * @param MaxLength the maximum length to be read from the line
 * @param LineBuffer A pointer to a character array which will be allocated and populated by this method.
 * @param LineLength a Pointer to hold the line length read from the file
 * @returns True if successful; otherwise false.
 */
FAPI Bool8 FilesystemReadLine(FileHandle* Handle, UInt64 MaxLength, char** LineBuffer, UInt64* LineLength);

/** 
 * Writes text to the provided file, appending a '\n' afterward.
 * @param Handle A pointer to a FileHandle structure.
 * @param Text The text to be written.
 * @returns True if successful; otherwise false.
 */
FAPI Bool8 FilesystemWriteLine(FileHandle* Handle, const char* Text);

/** 
 * Reads up to DataSize bytes of data into Bytes_read. 
 * Allocates *Data, which must be freed by the caller.
 * @param Handle A pointer to a FileHandle structure.
 * @param DataSize The number of bytes to read.
 * @param Data A pointer to a block of memory to be populated by this method.
 * @param BytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
 * @returns True if successful; otherwise false.
 */
FAPI Bool8 FilesystemRead(FileHandle* Handle, UInt64 DataSize, void* Data, UInt64* BytesRead);

/** 
 * Reads up to DataSize bytes of data into Bytes_read. 
 * Allocates *Bytes, which must be freed by the caller.
 * @param handle A pointer to a FileHandle structure.
 * @param Bytes A pointer to a byte array which will be allocated and populated by this method.
 * @param BytesRead A pointer to a number which will be populated with the number of bytes actually read from the file.
 * @returns True if successful; otherwise false.
 */
FAPI Bool8 FilesystemReadAllBytes(FileHandle* Handle, UInt8** Bytes, UInt64* BytesRead);

/** 
 * Writes provided data to the file.
 * @param Handle A pointer to a FileHandle structure.
 * @param DataSize The size of the data in bytes.
 * @param Data The data to be written.
 * @param BytesWritten A pointer to a number which will be populated with the number of bytes actually written to the file.
 * @returns True if successful; otherwise false.
 */
FAPI Bool8 FilesystemWrite(FileHandle* Handle, UInt64 DataSize, const void* Data, UInt64* BytesWritten);
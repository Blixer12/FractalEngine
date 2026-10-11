#include "Filesystem.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#if defined(_WIN32)
    typedef struct _stat PlatformStatStruct;
    #define PlatformStatFunc _stat
#else
    typedef struct stat PlatformStatStruct;
    #define PlatformStatFunc stat
#endif

Bool8 FilesystemExists(const char* path) {
    PlatformStatStruct Buffer;
    return PlatformStatFunc(path, &Buffer) == 0;
}

Bool8 FilesystemOpen(const char* Path, FileModes Mode, Bool8 Binary, FileHandle* Handle) 
{
    Handle->IsValid = false;
    Handle->Handle = 0;
    const char* ModeString;

    const UInt16 SwitchKey = (Mode & 0x0F) | (Binary ? 0x100 : 0);

    switch (SwitchKey) {
        // --- READ ONLY ---
        case FILE_MODE_READ:                                  ModeString = "r";   break;
        case FILE_MODE_READ | 0x100:                          ModeString = "rb";  break;

        // --- WRITE ONLY (Truncate/Create) ---
        case FILE_MODE_WRITE:                                 ModeString = "w";   break;
        case FILE_MODE_WRITE | 0x100:                         ModeString = "wb";  break;

        // --- READ + WRITE (Truncate/Create) ---
        case FILE_MODE_READ | FILE_MODE_WRITE:                ModeString = "w+";  break;
        case FILE_MODE_READ | FILE_MODE_WRITE | 0x100:        ModeString = "w+b"; break;

        // --- APPEND ONLY (Preserve/Create, writes always seek to end) ---
        case FILE_MODE_APPEND:                                ModeString = "a";   break;
        case FILE_MODE_APPEND | 0x100:                        ModeString = "ab";  break;

        // --- READ + APPEND (Preserve/Create, reads anywhere, writes seek to end) ---
        case FILE_MODE_READ | FILE_MODE_APPEND:               ModeString = "a+";  break;
        case FILE_MODE_READ | FILE_MODE_APPEND | 0x100:       ModeString = "a+b"; break;

        // --- READ + WRITE EXISTING (Preserve contents, start at beginning) ---
        case FILE_MODE_READ | FILE_MODE_UPDATE:               ModeString = "r+";  break;
        case FILE_MODE_READ | FILE_MODE_UPDATE | 0x100:       ModeString = "r+b"; break;

        default:
            FLERROR("Invalid Mode flags (0x%X) passed while trying to open file: '%s'", Mode, Path);
            return false;
    }

    // Attempt to open the file.
    FILE* File = fopen(Path, ModeString);
    if (!File) {
        FLERROR("Error opening file: '%s'", Path);
        return false;
    }

    Handle->Handle = File;
    Handle->IsValid = true;

    return true;
}

void FilesystemClose(FileHandle* Handle) {
    if (Handle->Handle) {
        fclose((FILE*)Handle->Handle);
        Handle->Handle = 0;
        Handle->IsValid = false;
    }
}

Bool8 FilesystemSize(FileHandle* Handle, UInt64* Size)
{
    if (Handle->Handle)
    {
        fseek((FILE*)Handle->Handle, 0, SEEK_END);
        *Size = ftell((FILE*)Handle->Handle);
        rewind((FILE*)Handle->Handle);
        return true;
    }

    return false;
}

Bool8 FilesystemReadLine(FileHandle* Handle, UInt64 MaxLength, char** LineBuffer, UInt64* LineLength)
{
    if (Handle->Handle && LineBuffer && LineLength && MaxLength > 0)
    {
        char* Buffer = *LineBuffer;
        if (fgets(Buffer, MaxLength, (FILE*)Handle->Handle) != 0)
        {
            *LineLength = strlen(*LineBuffer);
            return true;
        }
    }
    return false;
}

Bool8 FilesystemWriteLine(FileHandle* Handle, const char* Text) {
    if (Handle->Handle) {
        Int32 Result = fputs(Text, (FILE*)Handle->Handle);
        if (Result != EOF) {
            Result = fputc('\n', (FILE*)Handle->Handle);
        }

        // Make sure to flush the stream so it is written to the file immediately.
        // This prevents data loss in the event of a crash.
        fflush((FILE*)Handle->Handle);
        return Result != EOF;
    }
    return false;
}

Bool8 FilesystemRead(FileHandle* Handle, UInt64 DataSize, void* Data, UInt64* BytesRead) {
    if (Handle->Handle && Data) {
        *BytesRead = fread(Data, 1, DataSize, (FILE*)Handle->Handle);
        if (*BytesRead != DataSize) {
            return false;
        }
        return true;
    }
    return false;
}

Bool8 FilesystemReadAllBytes(FileHandle* Handle, UInt8* Bytes, UInt64* BytesRead) {
    if (Handle->Handle && Bytes && BytesRead) {
        // File size
        UInt64 Size = 0;
        if (!FilesystemSize(Handle, &Size))
        {
            return false;
        }

        *BytesRead = fread(Bytes, 1, Size, (FILE*)Handle->Handle);
        return *BytesRead == Size;
    }
    return false;
}

Bool8 FilesystemReadAllText(FileHandle* Handle, char* Text, UInt64* BytesRead) {
    if (Handle->Handle && Text && BytesRead) {
        // File size
        UInt64 Size = 0;
        if (!FilesystemSize(Handle, &Size))
        {
            return false;
        }

        *BytesRead = fread(Text, 1, Size, (FILE*)Handle->Handle);
        return *BytesRead == Size;
    }
    return false;
}

Bool8 FilesystemWrite(FileHandle* Handle, UInt64 DataSize, const void* data, UInt64* Bytes_written) {
    if (Handle->Handle) {
        *Bytes_written = fwrite(data, 1, DataSize, (FILE*)Handle->Handle);
        if (*Bytes_written != DataSize) {
            return false;
        }
        fflush((FILE*)Handle->Handle);
        return true;
    }
    return false;
}
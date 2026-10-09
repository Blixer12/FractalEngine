#include "HashtableTests.h"
#include "../TestManager.h"
#include "../Expect.h"

#include <Defines.h>
#include <Containers/Hashtable.h>

UInt8 HashtableShouldCreateAndDestroy() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(UInt64);
    UInt64 ElementCount = 3;
    UInt64 Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, false, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(UInt64), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableShouldSetAndGetSuccessfully() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(UInt64);
    UInt64 ElementCount = 3;
    UInt64 Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, false, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(UInt64), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    UInt64 TestValue1 = 23;
    HashtableSet(&Table, "Test1", &TestValue1);
    UInt64 GetTestValue1 = 0;
    HashtableGet(&Table, "Test1", &GetTestValue1);
    ExpectShouldBe(TestValue1, GetTestValue1);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

typedef struct HashtableTestStruct {
    Bool8 BoolValue;
    Float32 FloatValue;
    UInt64 UIntValue;
} HashtableTestStruct;

UInt8 HashtableShouldSetAndGetPtrSuccessfully() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct*);
    UInt64 ElementCount = 3;
    HashtableTestStruct* Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, true, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct*), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    HashtableTestStruct T;
    HashtableTestStruct* TestValue1 = &T;
    TestValue1->BoolValue = true;
    TestValue1->UIntValue = 63;
    TestValue1->FloatValue = 3.1415f;
    HashtableSetPtr(&Table, "Test1", (void**)&TestValue1);

    HashtableTestStruct* GetTestValue1 = 0;
    HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue1);

    ExpectShouldBe(TestValue1->BoolValue, GetTestValue1->BoolValue);
    ExpectShouldBe(TestValue1->UIntValue, GetTestValue1->UIntValue);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableShouldSetAndGetNonexistant() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(UInt64);
    UInt64 ElementCount = 3;
    UInt64 Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, false, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(UInt64), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    UInt64 TestValue1 = 23;
    HashtableSet(&Table, "Test1", &TestValue1);
    UInt64 GetTestValue1 = 0;
    HashtableGet(&Table, "Test2", &GetTestValue1);
    ExpectShouldBe(0, GetTestValue1);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableShouldSetAndGetPtrNonexistant() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct*);
    UInt64 ElementCount = 3;
    HashtableTestStruct* Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, true, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct*), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    HashtableTestStruct T;
    HashtableTestStruct* TestValue1 = &T;
    TestValue1->BoolValue = true;
    TestValue1->UIntValue = 63;
    TestValue1->FloatValue = 3.1415f;
    Bool8 Result = HashtableSetPtr(&Table, "Test1", (void**)&TestValue1);
    ExpectToBeTrue(Result);

    HashtableTestStruct* GetTestValue1 = 0;
    Result = HashtableGetPtr(&Table, "Test2", (void**)&GetTestValue1);
    ExpectToBeFalse(Result);
    ExpectShouldBe(0, GetTestValue1);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableShouldSetAndUnsetPtr() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct*);
    UInt64 ElementCount = 3;
    HashtableTestStruct* Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, true, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct*), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    HashtableTestStruct T;
    HashtableTestStruct* TestValue1 = &T;
    TestValue1->BoolValue = true;
    TestValue1->UIntValue = 63;
    TestValue1->FloatValue = 3.1415f;
    // Set it
    Bool8 Result = HashtableSetPtr(&Table, "Test1", (void**)&TestValue1);
    ExpectToBeTrue(Result);

    // Check that it exists and is correct.
    HashtableTestStruct* GetTestValue1 = 0;
    HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue1);
    ExpectShouldBe(TestValue1->BoolValue, GetTestValue1->BoolValue);
    ExpectShouldBe(TestValue1->UIntValue, GetTestValue1->UIntValue);

    // Unset it
    Result = HashtableSetPtr(&Table, "Test1", 0);
    ExpectToBeTrue(Result);

    // Should no longer be found.
    HashtableTestStruct* GetTestValue2 = 0;
    Result = HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue2);
    ExpectToBeFalse(Result);
    ExpectShouldBe(0, GetTestValue2);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableTryCallNonPtrOnPtrTable() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct*);
    UInt64 ElementCount = 3;
    HashtableTestStruct* Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, true, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct*), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    FLDEBUG("The following 2 error messages are intentional.");

    HashtableTestStruct T;
    T.BoolValue = true;
    T.UIntValue = 63;
    T.FloatValue = 3.1415f;
    // Try setting the record
    Bool8 Result = HashtableSet(&Table, "Test1", &T);
    ExpectToBeFalse(Result);

    // Try getting the record.
    HashtableTestStruct* GetTestValue1 = 0;
    Result = HashtableGet(&Table, "Test1", (void**)&GetTestValue1);
    ExpectToBeFalse(Result);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableTryCallPtrOnNonPtrTable() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct);
    UInt64 ElementCount = 3;
    HashtableTestStruct Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, false, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    FLDEBUG("The following 2 error messages are intentional.");

    HashtableTestStruct T;
    HashtableTestStruct* TestValue1 = &T;
    TestValue1->BoolValue = true;
    TestValue1->UIntValue = 63;
    TestValue1->FloatValue = 3.1415f;
    // Attempt to call pointer functions.
    Bool8 Result = HashtableSetPtr(&Table, "Test1", (void**)&TestValue1);
    ExpectToBeFalse(Result);

    // Try to call pointer function.
    HashtableTestStruct* GetTestValue1 = 0;
    Result = HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue1);
    ExpectToBeFalse(Result);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

UInt8 HashtableShouldSetGetAndUpdatePtrSuccessfully() {
    Hashtable Table;
    UInt64 ElementSize = sizeof(HashtableTestStruct*);
    UInt64 ElementCount = 3;
    HashtableTestStruct* Memory[3];

    HashtableCreate(ElementSize, ElementCount, Memory, true, &Table);

    ExpectShouldNotBe(0, Table.Memory);
    ExpectShouldBe(sizeof(HashtableTestStruct*), Table.ElementSize);
    ExpectShouldBe(3, Table.ElementCount);

    HashtableTestStruct T;
    HashtableTestStruct* TestValue1 = &T;
    TestValue1->BoolValue = true;
    TestValue1->UIntValue = 63;
    TestValue1->FloatValue = 3.1415f;
    HashtableSetPtr(&Table, "Test1", (void**)&TestValue1);

    HashtableTestStruct* GetTestValue1 = 0;
    HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue1);
    ExpectShouldBe(TestValue1->BoolValue, GetTestValue1->BoolValue);
    ExpectShouldBe(TestValue1->UIntValue, GetTestValue1->UIntValue);

    // Update pointed-to values
    GetTestValue1->BoolValue = false;
    GetTestValue1->UIntValue = 99;
    GetTestValue1->FloatValue = 6.69f;

    // Get the pointer again and confirm correct values
    HashtableTestStruct* GetTestValue2 = 0;
    HashtableGetPtr(&Table, "Test1", (void**)&GetTestValue2);
    ExpectToBeFalse(GetTestValue2->BoolValue);
    ExpectShouldBe(99, GetTestValue2->UIntValue);
    ExpectFloatToBe(6.69f, GetTestValue2->FloatValue);

    HashtableDestroy(&Table);

    ExpectShouldBe(0, Table.Memory);
    ExpectShouldBe(0, Table.ElementSize);
    ExpectShouldBe(0, Table.ElementCount);

    return true;
}

void HashtableRegisterTests() {
    TestManagerRegisterTest(HashtableShouldCreateAndDestroy, "Hashtable should create and destroy");
    TestManagerRegisterTest(HashtableShouldSetAndGetSuccessfully, "Hashtable should set and get");
    TestManagerRegisterTest(HashtableShouldSetAndGetPtrSuccessfully, "Hashtable should set and get pointer");
    TestManagerRegisterTest(HashtableShouldSetAndGetNonexistant, "Hashtable should set and get non-existent entry as nothing.");
    TestManagerRegisterTest(HashtableShouldSetAndGetPtrNonexistant, "Hashtable should set and get non-existent pointer entry as nothing.");
    TestManagerRegisterTest(HashtableShouldSetAndUnsetPtr, "Hashtable should set and unset pointer entry as nothing.");
    TestManagerRegisterTest(HashtableTryCallNonPtrOnPtrTable, "Hashtable try calling non-pointer functions on pointer type Table.");
    TestManagerRegisterTest(HashtableTryCallPtrOnNonPtrTable, "Hashtable try calling pointer functions on non-pointer type Table.");
    TestManagerRegisterTest(HashtableShouldSetGetAndUpdatePtrSuccessfully, "Hashtable Should get pointer, update, and get again successfully.");
}
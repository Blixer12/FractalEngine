#include "GeometrySystem.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Renderer/CrystalFrontend.h"

#include "Systems/MaterialSystem.h"

typedef struct GeometryReference {
    UInt64 ReferneceCount;
    Geometry Geometry;
    Bool8 AutoRelease;
} GeometryReference;

typedef struct GeometrySystemState {
    GeometrySystemConfig Config;
    Geometry DefaultGeometry;

    // Array of registered Meshes.
    GeometryReference* RegisteredGeometries;
} GeometrySystemState;

static GeometrySystemState* StatePtr = 0;

Bool8 CreateDefaultGeometry(GeometrySystemState* State);
Bool8 CreateGeometry(GeometrySystemState* State, GeometryConfig Config, Geometry* G);
void DestroyGeometry(GeometrySystemState* State, Geometry* G);

Bool8 GeometrySystemInitialize(UInt64* MemoryRequirement, void* State, GeometrySystemConfig Config)
{
    if (Config.MaxGeometryCount == 0) {
        FLFATAL("GeometrySystemInitialize - Config.MaxGeometryCount must be > 0.");
        return false;
    }

    // Block of memory will contain state structure, then block for array, then block for hashtable.
    UInt64 StructRequirement = sizeof(GeometrySystemState);
    UInt64 ArrayRequirement = sizeof(Geometry) * Config.MaxGeometryCount;
    *MemoryRequirement = StructRequirement + ArrayRequirement;

    if (!State) {
        return true;
    }

    StatePtr = State;
    StatePtr->Config = Config;

    // The array block is after the state. Already allocated, so just set the pointer.
    void* ArrayBlock = State + StructRequirement;
    StatePtr->RegisteredGeometries = ArrayBlock;

    // Invalidate all Geometries in the array.
    UInt32 Count = StatePtr->Config.MaxGeometryCount;
    for (UInt32 i = 0; i < Count; ++i) {
        StatePtr->RegisteredGeometries[i].Geometry.ID = InvalidID;
        StatePtr->RegisteredGeometries[i].Geometry.InternalID = InvalidID;
        StatePtr->RegisteredGeometries[i].Geometry.Generation = InvalidID;
    }

    // Create default Geometry for use in the system.
    if (!CreateDefaultGeometry(StatePtr))
    {
        FLFATAL("Failed to create default Geometry, Application cannot run!");
        return false;
    }

    return true;
}

void GeometrySystemShutdown(void* State)
{
    (void)State;
    // Nothing to do, just derefrence i guess?
    // This function is useless...
    // So, how are you? good, great!
    // I dont know why i am here, i need to finish this bro ToT
}

Geometry* GeometrySystemAcquireByID(UInt32 ID)
{
    if (ID != InvalidID && StatePtr->RegisteredGeometries[ID].Geometry.ID != InvalidID)
    {
        StatePtr->RegisteredGeometries[ID].ReferneceCount++;
        return &StatePtr->RegisteredGeometries[ID].Geometry;
    }

    // Return default i guess? well... i need to make a function for it... nrn i guess.
    FLERROR("GeometrySystemAcquireByID cannot load invalid Geometry Id. returning nullptr");
    return 0;
}

Geometry* GeometrySystemAcquireFromConfig(GeometryConfig Config, Bool8 AutoRelease)
{
    Geometry* G = 0;
    for (UInt32 i = 0; i < StatePtr->Config.MaxGeometryCount; ++i)
    {
        if (StatePtr->RegisteredGeometries[i].Geometry.ID == InvalidID)
        {
            StatePtr->RegisteredGeometries[i].AutoRelease = AutoRelease;
            StatePtr->RegisteredGeometries[i].ReferneceCount = 1;
            G = &StatePtr->RegisteredGeometries[i].Geometry;
            G->ID = 1;
            break;
        }
    }

    if (!G)
    {
        FLERROR("Unable to obtain free slot for geometry. adjust config to allow more space, returning nullptr");
        return 0;
    }

    if (!CreateGeometry(StatePtr, Config, G))
    {
        FLERROR("Failed to create geometry. returning nullptr");
        return 0;
    }

    return G;
}

void GeometrySystemRelease(Geometry* Geometry)
{
    if (Geometry && Geometry->ID != InvalidID)
    {
        GeometryReference* Reference = &StatePtr->RegisteredGeometries[Geometry->ID];

        // Take a copy of the ID
        // UInt32 ID = Geometry->ID;
        if (Reference->Geometry.ID == Geometry->ID)
        {
            if (Reference->ReferneceCount > 0)
            {
                Reference->ReferneceCount--;
            }

            if (Reference->ReferneceCount < 1 && Reference->AutoRelease)
            {
                DestroyGeometry(StatePtr, &Reference->Geometry);
                Reference->ReferneceCount = 0;
                Reference->AutoRelease = false;
            }
        } else {
            FLFATAL("Geometry ID mismatch. Check Registration logic, should NEVER OCCUR!!!");
        }

        return;
    }

    FLWARN("GeometrySystemRelease cannot release invalid geometry IDs, Nothing was done!");
}

Geometry* GeometrySystemGetDefault()
{
    if (StatePtr)
    {
        return &StatePtr->DefaultGeometry;
    }

    FLFATAL("GeometrySystemGetDefault called before system init!");
    return 0;
}

Bool8 CreateGeometry(GeometrySystemState* State, GeometryConfig Config, Geometry* G)
{
    if (!CrystalCreateGeometry(G, Config.VertexCount, Config.Vertices, Config.IndexCount, Config.Indices))
    {
        State->RegisteredGeometries[G->ID].ReferneceCount = 0;
        State->RegisteredGeometries[G->ID].AutoRelease = false;
        G->ID = InvalidID; 
        G->Generation = InvalidID;
        G->InternalID = InvalidID;
        
        return false;
    }

    if (StringLength(Config.MaterialName) > 0)
    {
        G->Material = MaterialSystemAcquire(Config.MaterialName);
        if (!G->Material)
        {
            G->Material = MaterialSystemGetDefault();
        }
    }

    return true;
}

void DestroyGeometry(GeometrySystemState* State, Geometry* G)
{
    (void)State;
    CrystalDestroyGeometry(G);
    G->InternalID = InvalidID;
    G->Generation = InvalidID;
    G->ID = InvalidID;

    StringEmpty(G->Name);

    if (G->Material && StringLength(G->Material->Name) > 0)
    {
        MaterialSystemRelease(G->Material->Name);
        G->Material = 0;
    }
}

Bool8 CreateDefaultGeometry(GeometrySystemState* State)
{
    Vertex3D Vertices[4];
    FMZeroMemory(Vertices, sizeof(Vertex3D) * 4);

    const Float32 Factor = 10.0f;

    Vertices[0].Position.x = -0.5 * Factor;
    Vertices[0].Position.y = -0.5 * Factor;
    Vertices[0].TextureCoordinates.u = 0.0f;
    Vertices[0].TextureCoordinates.v = 0.0f;

    Vertices[1].Position.x = 0.5 * Factor;
    Vertices[1].Position.y = 0.5 * Factor;
    Vertices[1].TextureCoordinates.u = 1.0f;
    Vertices[1].TextureCoordinates.v = 1.0f;

    Vertices[2].Position.x = -0.5 * Factor;
    Vertices[2].Position.y = 0.5 * Factor;
    Vertices[2].TextureCoordinates.u = 0.0f;
    Vertices[2].TextureCoordinates.v = 1.0f;

    Vertices[3].Position.x = 0.5 * Factor;
    Vertices[3].Position.y = -0.5 * Factor;
    Vertices[3].TextureCoordinates.u = 1.0f;
    Vertices[3].TextureCoordinates.v = 0.0f;

    UInt32 Indices[6] = {0, 1, 2, 0, 3, 1};

    if (!CrystalCreateGeometry(&State->DefaultGeometry, 4, Vertices, 6, Indices))
    {
        FLFATAL("Failed to create geometry. Application cannot continue");
        return false;
    }

    State->DefaultGeometry.Material = MaterialSystemGetDefault();
    return true;
}

GeometryConfig GeometrySystemGeneratePlaneConfig(Float32 Width, Float32 Height, UInt32 XSegmentCount, UInt32 YSegmentCount, Float32 TileX, Float32 TileY, const char* Name, const char* MaterialName)
{
    if (Width == 0.0f)
    {
        FLWARN("Width must be nonzero, defaulting to one");
        Width = 1.0f;
    }
    if (Height == 0.0f)
    {
        FLWARN("Height must be nonzero, defaulting to one");
        Height = 1.0f;
    }
    
    if (XSegmentCount < 1)
    {
        FLWARN("XSegmentCount must be positive, defaulting to one");
        XSegmentCount = 1;
    }

    if (YSegmentCount < 1)
    {
        FLWARN("YSegmentCount must be positive, defaulting to one");
        YSegmentCount = 1;
    }

    if (TileX == 0.0f)
    {
        FLWARN("TileX must be nonzero, defaulting to one");
        TileX = 1.0f;
    }

    if (TileY == 0.0f)
    {
        FLWARN("TileY must be nonzero, defaulting to one");
        TileY = 1.0f;
    }

    GeometryConfig Config;
    // Assuming GeometryConfig holds fixed-size strings or you use a string copy function here
    StringCopy(Config.Name, Name);
    StringCopy(Config.MaterialName, MaterialName);
    
    Config.VertexCount = (XSegmentCount + 1) * (YSegmentCount + 1);
    Config.IndexCount = XSegmentCount * YSegmentCount * 6;

    Config.Vertices = FMAllocate(sizeof(Vertex3D) * Config.VertexCount, MEMORY_TAG_ARRAY);
    Config.Indices = FMAllocate(sizeof(UInt32) * Config.IndexCount, MEMORY_TAG_ARRAY);

    Float32 HalfWidth = Width * 0.5f;
    Float32 HalfHeight = Height * 0.5f;
    Float32 SegmentWidth = Width / XSegmentCount;
    Float32 SegmentHeight = Height / YSegmentCount;

    // Generate Vertices
    for (UInt32 y = 0; y <= YSegmentCount; ++y)
    {
        for (UInt32 x = 0; x <= XSegmentCount; ++x)
        {
            UInt32 Index = y * (XSegmentCount + 1) + x;
            
            Config.Vertices[Index].Position.x = (x * SegmentWidth) - HalfWidth;
            Config.Vertices[Index].Position.y = (y * SegmentHeight) - HalfHeight;
            Config.Vertices[Index].Position.z = 0.0f;

            Config.Vertices[Index].TextureCoordinates.u = ((Float32)x / XSegmentCount) * TileX;
            Config.Vertices[Index].TextureCoordinates.v = ((Float32)y / YSegmentCount) * TileY;
        }
    }

    // Generate Indices
    UInt32 IndexOffset = 0;
    for (UInt32 y = 0; y < YSegmentCount; ++y)
    {
        for (UInt32 x = 0; x < XSegmentCount; ++x)
        {
            UInt32 Index = y * (XSegmentCount + 1) + x;

            // Triangle 1
            Config.Indices[IndexOffset++] = Index;
            Config.Indices[IndexOffset++] = Index + 1;
            Config.Indices[IndexOffset++] = Index + XSegmentCount + 1;

            // Triangle 2
            Config.Indices[IndexOffset++] = Index + XSegmentCount + 1;
            Config.Indices[IndexOffset++] = Index + 1;
            Config.Indices[IndexOffset++] = Index + XSegmentCount + 2;
        }
    }

    if (Name && StringLength(Name) > 0) {
        StringNcopy(Config.Name, Name, GeometryNameMaxLength);
    } else {
        StringNcopy(Config.Name, DefaultGeometryName, GeometryNameMaxLength);
    }

    if (MaterialName && StringLength(MaterialName) > 0) {
        StringNcopy(Config.MaterialName, MaterialName, GeometryNameMaxLength);
    } else {
        StringNcopy(Config.Name, DefaultMaterialName, MaterialNameMaxLength);
    }

    return Config;
}
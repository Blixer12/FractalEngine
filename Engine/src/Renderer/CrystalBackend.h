#pragma once

#include "CrystalDef.inl"

struct PlatformState;

Bool8 CrystalBackendCreate(CrystalBackendType Type, CrystalBackend* Backend);
void CrystalBackendDestroy(CrystalBackend* Backend);
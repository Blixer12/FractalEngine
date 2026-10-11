#pragma once

#include "Defines.h"
#include "MathDef.h"
#include "Core/Memory.h"

static constexpr Float32 F_PI = 3.14159265358979323846f;
static constexpr Float32 F_PI_2 = 2.0f * F_PI;
static constexpr Float32 F_HALF_PI = 0.5f * F_PI;
static constexpr Float32 F_QUARTER_PI = 0.25f * F_PI;
static constexpr Float32 F_ONE_OVER_PI = 1.0f / F_PI;
static constexpr Float32 F_ONE_OVER_TWO_PI = 1.0f / F_PI_2;
static constexpr Float32 F_SQRT_TWO = 1.41421356237309504880f;
static constexpr Float32 F_SQRT_THREE = 1.73205080756887729352f;
static constexpr Float32 F_SQRT_ONE_OVER_TWO = 0.70710678118654752440f;
static constexpr Float32 F_SQRT_ONE_OVER_THREE = 0.57735026918962576450f;
static constexpr Float32 F_DEG2RAD_MULTIPLIER = F_PI / 180.0f;
static constexpr Float32 F_RAD2DEG_MULTIPLIER = 180.0f / F_PI;

// The multiplier to convert seconds to milliseconds.
static constexpr Float32 F_SEC_TO_MS_MULTIPLIER = 1000.0f;

// The multiplier to convert milliseconds to seconds.
static constexpr Float32 F_MS_TO_SEC_MULTIPLIER = 0.001f;

// A huge number that should be larger than any valid number used.
static constexpr Float32 F_INFINITY = 1e30f;

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
static constexpr Float32 F_FLOAT_EPSILON = 1.192092896e-07f;

// ------------------------------------------
// General math functions
// ------------------------------------------
FAPI Float32 Fsin(Float32 x);
FAPI Float32 Fcos(Float32 x);
FAPI Float32 Ftan(Float32 x);
FAPI Float32 Facos(Float32 x);
FAPI Float32 Fsqrt(Float32 x);
FAPI Float32 Fabs(Float32 x);

/**
 * Indicates if the Value is a power of 2. 0 is considered _not_ a power of 2.
 * @param Value The Value to be interpreted.
 * @returns True if a power of 2, otherwise false.
 */
FINLINE Bool8 IsPowerOf2(UInt64 Value) {
    return (Value != 0) && ((Value & (Value - 1)) == 0);
}

FAPI Int32 FRandom();
FAPI Int32 FRandomInRange(Int32 min, Int32 max);

FAPI Float32 FloatFRandom();
FAPI Float32 FloatFRandomInRange(Float32 min, Float32 max);

// ------------------------------------------
// Vector 2
// ------------------------------------------

/**
 * @brief Creates and returns a new 2-element Vector using the supplied Values.
 * 
 * @param x The x Value.
 * @param y The y Value.
 * @return A new 2-element Vector.
 */
FINLINE Vec2 Vec2Create(Float32 x, Float32 y) {
    Vec2 Vector;
    Vector.x = x;
    Vector.y = y;
    return Vector;
}

/**
 * @brief Creates and returns a 2-component Vector with all components set to 0.0f.
 */
FINLINE Vec2 Vec2Zero() {
    return (Vec2){.x = 0.0f, .y = 0.0f};
}

/**
 * @brief Creates and returns a 2-component Vector with all components set to 1.0f.
 */
FINLINE Vec2 Vec2One() {
    return (Vec2){.x = 1.0f, .y = 1.0f};
}

/**
 * @brief Creates and returns a 2-component Vector pointing up (0, 1).
 */
FINLINE Vec2 Vec2Up() {
    return (Vec2){.x = 0.0f, .y = 1.0f};
}

/**
 * @brief Creates and returns a 2-component Vector pointing down (0, -1).
 */
FINLINE Vec2 Vec2Down() {
    return (Vec2){.x = 0.0f, .y = -1.0f};
}

/**
 * @brief Creates and returns a 2-component Vector pointing left (-1, 0).
 */
FINLINE Vec2 Vec2Left() {
    return (Vec2){.x = -1.0f, .y = 0.0f};
}

/**
 * @brief Creates and returns a 2-component Vector pointing right (1, 0).
 */
FINLINE Vec2 Vec2Right() {
    return (Vec2){.x = 1.0f, .y = 0.0f};
}

/**
 * @brief Adds Vector1 to Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec2 Vec2Add(Vec2 Vector0, Vec2 Vector1) {
    return (Vec2){
        .x = Vector0.x + Vector1.x,
        .y = Vector0.y + Vector1.y};
}

/**
 * @brief Subtracts Vector1 from Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec2 Vec2Sub(Vec2 Vector0, Vec2 Vector1) {
    return (Vec2){
        .x = Vector0.x - Vector1.x,
        .y = Vector0.y - Vector1.y};
}

/**
 * @brief Multiplies Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec2 Vec2Mul(Vec2 Vector0, Vec2 Vector1) {
    return (Vec2){
        .x = Vector0.x * Vector1.x,
        .y = Vector0.y * Vector1.y};
}

/**
 * @brief Divides Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec2 Vec2Div(Vec2 Vector0, Vec2 Vector1) {
    return (Vec2){
        .x = Vector0.x / Vector1.x,
        .y = Vector0.y / Vector1.y};
}

/**
 * @brief Returns the squared length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the squared length of.
 * @return The squared length.
 */
FINLINE Float32 Vec2LengthSquared(Vec2 Vector) {
    return Vector.x * Vector.x + Vector.y * Vector.y;
}

/**
 * @brief Returns the length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the length of.
 * @return The length.
 */
FINLINE Float32 Vec2Length(Vec2 Vector) {
    return Fsqrt(Vec2LengthSquared(Vector));
}

/**
 * @brief Normalizes the provided Vector in place to a unit Vector.
 * 
 * @param Vector A pointer to the Vector to be normalized.
 */
FINLINE void Vec2Normalize(Vec2* Vector) {
    const Float32 length = Vec2Length(*Vector);
    if (length > 0.0f) {
        Vector->x /= length;
        Vector->y /= length;
    }
}

/**
 * @brief Returns a normalized copy of the supplied Vector.
 * 
 * @param Vector The Vector to be normalized.
 * @return A normalized copy of the supplied Vector 
 */
FINLINE Vec2 Vec2Normalized(Vec2 Vector) {
    Vec2Normalize(&Vector);
    return Vector;
}

/**
 * @brief Compares all elements of Vector0 and Vector1 and ensures the difference
 * is less than Tolerance.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @param Tolerance The difference Tolerance. Typically F_FLOAT_EPSILON or similar.
 * @return True if within Tolerance; otherwise false. 
 */
FINLINE Bool8 Vec2Compare(Vec2 Vector0, Vec2 Vector1, Float32 Tolerance) {
    if (Fabs(Vector0.x - Vector1.x) > Tolerance) {
        return false;
    }

    if (Fabs(Vector0.y - Vector1.y) > Tolerance) {
        return false;
    }

    return true;
}

/**
 * @brief Returns the distance between Vector0 and Vector1.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The distance between Vector0 and Vector1.
 */
FINLINE Float32 Vec2Distance(Vec2 Vector0, Vec2 Vector1) {
    Vec2 d = (Vec2){
        .x = Vector0.x - Vector1.x,
        .y = Vector0.y - Vector1.y};
    return Vec2Length(d);
}

// ------------------------------------------
// Vector 3
// ------------------------------------------

/**
 * @brief Creates and returns a new 3-element Vector using the supplied Values.
 * 
 * @param x The x Value.
 * @param y The y Value.
 * @param z The z Value.
 * @return A new 3-element Vector.
 */
FINLINE Vec3 Vec3Create(Float32 x, Float32 y, Float32 z) {
    return (Vec3){.x = x, .y = y, .z = z};
}

/**
 * @brief Returns a new Vec3 containing the x, y and z components of the 
 * supplied Vec4, essentially dropping the w component.
 * 
 * @param Vector The 4-component Vector to extract from.
 * @return A new Vec3 
 */
FINLINE Vec3 Vec3FromVec4(Vec4 Vector) {
    return (Vec3){.x = Vector.x, .y = Vector.y, .z = Vector.z};
}

/**
 * @brief Returns a new Vec4 using Vector as the x, y and z components and w for w.
 * 
 * @param Vector The 3-component Vector.
 * @param w The w component.
 * @return A new Vec4 
 */
FINLINE Vec4 Vec3ToVec4(Vec3 Vector, Float32 w) {
    return (Vec4){.x = Vector.x, .y = Vector.y, .z = Vector.z, .w = w};
}

/**
 * @brief Creates and returns a 3-component Vector with all components set to 0.0f.
 */
FINLINE Vec3 Vec3Zero() {
    return (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
}

/**
 * @brief Creates and returns a 3-component Vector with all components set to 1.0f.
 */
FINLINE Vec3 Vec3One() {
    return (Vec3){.x = 1.0f, .y = 1.0f, .z = 1.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing up (0, 1, 0).
 */
FINLINE Vec3 Vec3Up() {
    return (Vec3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing down (0, -1, 0).
 */
FINLINE Vec3 Vec3Down() {
    return (Vec3){.x = 0.0f, .y = -1.0f, .z = 0.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing left (-1, 0, 0).
 */
FINLINE Vec3 Vec3Left() {
    return (Vec3){.x = -1.0f, .y = 0.0f, .z = 0.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing right (1, 0, 0).
 */
FINLINE Vec3 Vec3Right() {
    return (Vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing forward (0, 0, -1).
 */
FINLINE Vec3 Vec3Forward() {
    return (Vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
}

/**
 * @brief Creates and returns a 3-component Vector pointing backward (0, 0, 1).
 */
FINLINE Vec3 Vec3Back() {
    return (Vec3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
}

/**
 * @brief Adds Vector1 to Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec3 Vec3Add(Vec3 Vector0, Vec3 Vector1) {
    return (Vec3){
        .x = Vector0.x + Vector1.x,
        .y = Vector0.y + Vector1.y,
        .z = Vector0.z + Vector1.z};
}

/**
 * @brief Subtracts Vector1 from Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec3 Vec3Sub(Vec3 Vector0, Vec3 Vector1) {
    return (Vec3){
        .x = Vector0.x - Vector1.x,
        .y = Vector0.y - Vector1.y,
        .z = Vector0.z - Vector1.z};
}

/**
 * @brief Multiplies Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec3 Vec3Mul(Vec3 Vector0, Vec3 Vector1) {
    return (Vec3){
        .x = Vector0.x * Vector1.x,
        .y = Vector0.y * Vector1.y,
        .z = Vector0.z * Vector1.z};
}

/**
 * @brief Multiplies all elements of Vector0 by Scalar and returns a copy of the Result.
 * 
 * @param Vector0 The Vector to be multiplied.
 * @param Scalar The Scalar Value.
 * @return A copy of the Resulting Vector.
 */
FINLINE Vec3 Vec3MulScalar(Vec3 Vector0, Float32 Scalar) {
    return (Vec3){
        .x = Vector0.x * Scalar,
        .y = Vector0.y * Scalar,
        .z = Vector0.z * Scalar};
}

/**
 * @brief Divides Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec3 Vec3Div(Vec3 Vector0, Vec3 Vector1) {
    return (Vec3){
        .x = Vector0.x / Vector1.x,
        .y = Vector0.y / Vector1.y,
        .z = Vector0.z / Vector1.z};
}

/**
 * @brief Returns the squared length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the squared length of.
 * @return The squared length.
 */
FINLINE Float32 Vec3LengthSquared(Vec3 Vector) {
    return Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z;
}

/**
 * @brief Returns the length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the length of.
 * @return The length.
 */
FINLINE Float32 Vec3Length(Vec3 Vector) {
    return Fsqrt(Vec3LengthSquared(Vector));
}

/**
 * @brief Normalizes the provided Vector in place to a unit Vector.
 * 
 * @param Vector A pointer to the Vector to be normalized.
 */
FINLINE void Vec3Normalize(Vec3* Vector) {
    const Float32 length = Vec3Length(*Vector);
    if (length > 0.0f) {
        Vector->x /= length;
        Vector->y /= length;
        Vector->z /= length;
    }
}

/**
 * @brief Returns a normalized copy of the supplied Vector.
 * 
 * @param Vector The Vector to be normalized.
 * @return A normalized copy of the supplied Vector 
 */
FINLINE Vec3 Vec3Normalized(Vec3 Vector) {
    Vec3Normalize(&Vector);
    return Vector;
}

/**
 * @brief Returns the dot product between the provided Vectors. Typically used
 * to calculate the difference in direction.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The dot product. 
 */
FINLINE Float32 Vec3Dot(Vec3 Vector0, Vec3 Vector1) {
    Float32 p = 0;
    p += Vector0.x * Vector1.x;
    p += Vector0.y * Vector1.y;
    p += Vector0.z * Vector1.z;
    return p;
}

/**
 * @brief Calculates and returns the cross product of the supplied Vectors.
 * The cross product is a new Vector which is orthoganal to both provided Vectors.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The cross product. 
 */
FINLINE Vec3 Vec3Cross(Vec3 Vector0, Vec3 Vector1) {
    return (Vec3){
        .x = Vector0.y * Vector1.z - Vector0.z * Vector1.y,
        .y = Vector0.z * Vector1.x - Vector0.x * Vector1.z,
        .z = Vector0.x * Vector1.y - Vector0.y * Vector1.x};
}

/**
 * @brief Compares all elements of Vector0 and Vector1 and ensures the difference
 * is less than Tolerance.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @param Tolerance The difference Tolerance. Typically F_FLOAT_EPSILON or similar.
 * @return True if within Tolerance; otherwise false. 
 */
FINLINE Bool8 Vec3Compare(Vec3 Vector0, Vec3 Vector1, Float32 Tolerance) {
    if (Fabs(Vector0.x - Vector1.x) > Tolerance) {
        return false;
    }

    if (Fabs(Vector0.y - Vector1.y) > Tolerance) {
        return false;
    }

    if (Fabs(Vector0.z - Vector1.z) > Tolerance) {
        return false;
    }

    return true;
}

/**
 * @brief Returns the distance between Vector0 and Vector1.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The distance between Vector0 and Vector1.
 */
FINLINE Float32 Vec3Distance(Vec3 Vector0, Vec3 Vector1) {
    Vec3 d = (Vec3){
        .x = Vector0.x - Vector1.x,
        .y = Vector0.y - Vector1.y,
        .z = Vector0.z - Vector1.z};
    return Vec3Length(d);
}

// ------------------------------------------
// Vector 4
// ------------------------------------------

/**
 * @brief Creates and returns a new 4-element Vector using the supplied Values.
 * 
 * @param x The x Value.
 * @param y The y Value.
 * @param z The z Value.
 * @param w The w Value.
 * @return A new 4-element Vector.
 */
FINLINE Vec4 Vec4Create(Float32 x, Float32 y, Float32 z, Float32 w) {
    Vec4 Vector;

    Vector.x = x;
    Vector.y = y;
    Vector.z = z;
    Vector.w = w;

    return Vector;
}

/**
 * @brief Returns a new Vec3 containing the x, y and z components of the 
 * supplied Vec4, essentially dropping the w component.
 * 
 * @param Vector The 4-component Vector to extract from.
 * @return A new Vec3 
 */
FINLINE Vec3 Vec4ToVec3(Vec4 Vector) {
    return (Vec3){.x = Vector.x, .y = Vector.y, .z = Vector.z};
}

/**
 * @brief Returns a new Vec4 using Vector as the x, y and z components and w for w.
 * 
 * @param Vector The 3-component Vector.
 * @param W The w component.
 * @return A new Vec4 
 */
FINLINE Vec4 Vec4FromVec3(Vec3 Vector, Float32 W) {
    return (Vec4){.x = Vector.x, .y = Vector.y, .z = Vector.z, .w = W};
}

/**
 * @brief Creates and returns a 4-component Vector with all components set to 0.0f.
 */
FINLINE Vec4 Vec4Zero() {
    return (Vec4){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f};
}

/**
 * @brief Creates and returns a 4-component Vector with all components set to 1.0f.
 */
FINLINE Vec4 Vec4One() {
    return (Vec4){.x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f};
}

/**
 * @brief Adds Vector1 to Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec4 Vec4Add(Vec4 Vector0, Vec4 Vector1) {
    return (Vec4){
        .x = Vector0.x + Vector1.x,
        .y = Vector0.y + Vector1.y,
        .z = Vector0.z + Vector1.z,
        .w = Vector0.w + Vector1.w};
}

/**
 * @brief Subtracts Vector1 from Vector0 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec4 Vec4Sub(Vec4 Vector0, Vec4 Vector1) {
    return (Vec4){
        .x = Vector0.x - Vector1.x,
        .y = Vector0.y - Vector1.y,
        .z = Vector0.z - Vector1.z,
        .w = Vector0.w - Vector1.w};
}

/**
 * @brief Multiplies Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec4 Vec4Mul(Vec4 Vector0, Vec4 Vector1) {
    return (Vec4){
        .x = Vector0.x * Vector1.x,
        .y = Vector0.y * Vector1.y,
        .z = Vector0.z * Vector1.z,
        .w = Vector0.w * Vector1.w};
}

/**
 * @brief Divides Vector0 by Vector1 and returns a copy of the Result.
 * 
 * @param Vector0 The first Vector.
 * @param Vector1 The second Vector.
 * @return The Resulting Vector. 
 */
FINLINE Vec4 Vec4Div(Vec4 Vector0, Vec4 Vector1) {
    return (Vec4){
        .x = Vector0.x / Vector1.x,
        .y = Vector0.y / Vector1.y,
        .z = Vector0.z / Vector1.z,
        .w = Vector0.w / Vector1.w};
}

/**
 * @brief Returns the squared length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the squared length of.
 * @return The squared length.
 */
FINLINE Float32 Vec4LengthSquared(Vec4 Vector) {
    return Vector.x * Vector.x + Vector.y * Vector.y + Vector.z * Vector.z + Vector.w * Vector.w;
}

/**
 * @brief Returns the length of the provided Vector.
 * 
 * @param Vector The Vector to retrieve the length of.
 * @return The length.
 */
FINLINE Float32 Vec4Length(Vec4 Vector) {
    return Fsqrt(Vec4LengthSquared(Vector));
}

/**
 * @brief Normalizes the provided Vector in place to a unit Vector.
 * 
 * @param Vector A pointer to the Vector to be normalized.
 */
FINLINE void Vec4Normalize(Vec4* Vector) {
    const Float32 Length = Vec4Length(*Vector);
    if (Length > 0.0f) {
        Vector->x /= Length;
        Vector->y /= Length;
        Vector->z /= Length;
        Vector->w /= Length;
    }
}

/**
 * @brief Returns a normalized copy of the supplied Vector.
 * 
 * @param Vector The Vector to be normalized.
 * @return A normalized copy of the supplied Vector 
 */
FINLINE Vec4 Vec4Normalized(Vec4 Vector) {
    Vec4Normalize(&Vector);
    return Vector;
}

FINLINE Float32 Vec4DotFloat32(
    Float32 a0, Float32 a1, Float32 a2, Float32 a3,
    Float32 b0, Float32 b1, Float32 b2, Float32 b3) {
    return a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
}

// ------------------------------------------
// Quaternion
// ------------------------------------------

/**
 * @brief Creates and returns an identity quaternion.
 * 
 * @return A new identity quaternion.
 */
FINLINE Quaternion QuaternionIdentity() {
    return (Quaternion){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

/**
 * @brief Returns the normal (length) of the provided quaternion[cite: 7].
 * 
 * @param Q The quaternion.
 * @return The normal/length value.
 */
FINLINE Float32 QuaternionNormal(Quaternion Q) {
    return Fsqrt(
        Q.x * Q.x +
        Q.y * Q.y +
        Q.z * Q.z +
        Q.w * Q.w);
}

/**
 * @brief Returns a normalized copy of the supplied quaternion[cite: 7].
 * 
 * @param Q The quaternion to normalize.
 * @return A normalized quaternion.
 */
FINLINE Quaternion QuaternionNormalize(Quaternion Q) {
    Float32 Normal = QuaternionNormal(Q);
    if (Normal > 0.0f) {
        return (Quaternion){
            .x = Q.x / Normal,
            .y = Q.y / Normal,
            .z = Q.z / Normal,
            .w = Q.w / Normal};
    }
    return QuaternionIdentity();
}

/**
 * @brief Returns the conjugate of the provided quaternion[cite: 7].
 * 
 * @param Q The quaternion.
 * @return The conjugate quaternion.
 */
FINLINE Quaternion QuaternionConjugate(Quaternion Q) {
    return (Quaternion){
        .x = -Q.x,
        .y = -Q.y,
        .z = -Q.z,
        .w = Q.w};
}

/**
 * @brief Returns the inverse of the provided quaternion[cite: 7].
 * 
 * @param Q The quaternion.
 * @return The inverse quaternion.
 */
FINLINE Quaternion QuaternionInverse(Quaternion Q) {
    return QuaternionNormalize(QuaternionConjugate(Q));
}

/**
 * @brief Multiplies Q0 by Q1 and returns the resulting quaternion[cite: 7].
 * 
 * @param Q0 The first quaternion.
 * @param Q1 The second quaternion.
 * @return The product quaternion.
 */
FINLINE Quaternion QuaternionMul(Quaternion Q0, Quaternion Q1) {
    Quaternion OutQuaternion;

    OutQuaternion.x = Q0.x * Q1.w +
                       Q0.y * Q1.z -
                       Q0.z * Q1.y +
                       Q0.w * Q1.x;

    OutQuaternion.y = -Q0.x * Q1.z +
                       Q0.y * Q1.w +
                       Q0.z * Q1.x +
                       Q0.w * Q1.y;

    OutQuaternion.z = Q0.x * Q1.y -
                      Q0.y * Q1.x +
                      Q0.z * Q1.w +
                      Q0.w * Q1.z;

    OutQuaternion.w = -Q0.x * Q1.x -
                       Q0.y * Q1.y -
                       Q0.z * Q1.z +
                       Q0.w * Q1.w;

    return OutQuaternion;
}

/**
 * @brief Returns the dot product between two quaternions[cite: 7].
 * 
 * @param Q0 The first quaternion.
 * @param Q1 The second quaternion.
 * @return The dot product value.
 */
FINLINE Float32 QuaternionDot(Quaternion Q0, Quaternion Q1) {
    return Q0.x * Q1.x +
           Q0.y * Q1.y +
           Q0.z * Q1.z +
           Q0.w * Q1.w;
}

/**
 * @brief Converts a quaternion to a 4x4 rotation matrix ( safely using an out-pointer )[cite: 7].
 * 
 * @param Q The quaternion.
 * @param OutMatrix A pointer to the destination matrix.
 * @return A pointer to the resulting matrix.
 */
FINLINE Mat4* QuaternionToMat4(Quaternion Q, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    Quaternion N = QuaternionNormalize(Q);

    OutMatrix->Data[0] = 1.0f - 2.0f * N.y * N.y - 2.0f * N.z * N.z;
    OutMatrix->Data[1] = 2.0f * N.x * N.y - 2.0f * N.z * N.w;
    OutMatrix->Data[2] = 2.0f * N.x * N.z + 2.0f * N.y * N.w;

    OutMatrix->Data[4] = 2.0f * N.x * N.y + 2.0f * N.z * N.w;
    OutMatrix->Data[5] = 1.0f - 2.0f * N.x * N.x - 2.0f * N.z * N.z;
    OutMatrix->Data[6] = 2.0f * N.y * N.z - 2.0f * N.x * N.w;

    OutMatrix->Data[8] = 2.0f * N.x * N.z - 2.0f * N.y * N.w;
    OutMatrix->Data[9] = 2.0f * N.y * N.z + 2.0f * N.x * N.w;
    OutMatrix->Data[10] = 1.0f - 2.0f * N.x * N.x - 2.0f * N.y * N.y;

    return OutMatrix;
}

/**
 * @brief Calculates a rotation matrix based on the quaternion and center point ( out-pointer version )[cite: 7].
 * 
 * @param Q The quaternion.
 * @param Center The center point vector.
 * @param OutMatrix A pointer to the destination matrix.
 * @return A pointer to the resulting rotation matrix.
 */
FINLINE Mat4* QuaternionToRotationMatrix(Quaternion Q, Vec3 Center, Mat4* OutMatrix) {
    Float32* O = OutMatrix->Data;
    O[0] = (Q.x * Q.x) - (Q.y * Q.y) - (Q.z * Q.z) + (Q.w * Q.w);
    O[1] = 2.0f * ((Q.x * Q.y) + (Q.z * Q.w));
    O[2] = 2.0f * ((Q.x * Q.z) - (Q.y * Q.w));
    O[3] = Center.x - Center.x * O[0] - Center.y * O[1] - Center.z * O[2];

    O[4] = 2.0f * ((Q.x * Q.y) - (Q.z * Q.w));
    O[5] = -(Q.x * Q.x) + (Q.y * Q.y) - (Q.z * Q.z) + (Q.w * Q.w);
    O[6] = 2.0f * ((Q.y * Q.z) + (Q.x * Q.w));
    O[7] = Center.y - Center.x * O[4] - Center.y * O[5] - Center.z * O[6];

    O[8] = 2.0f * ((Q.x * Q.z) + (Q.y * Q.w));
    O[9] = 2.0f * ((Q.y * Q.z) - (Q.x * Q.w));
    O[10] = -(Q.x * Q.x) - (Q.y * Q.y) + (Q.z * Q.z) + (Q.w * Q.w);
    O[11] = Center.z - Center.x * O[8] - Center.y * O[9] - Center.z * O[10];

    O[12] = 0.0f;
    O[13] = 0.0f;
    O[14] = 0.0f;
    O[15] = 1.0f;
    
    return OutMatrix;
}

/**
 * @brief Creates a quaternion from an axis and an angle[cite: 7].
 * 
 * @param Axis The axis vector.
 * @param Angle The angle in radians.
 * @param Normalize Whether to normalize the result.
 * @return The new quaternion.
 */
FINLINE Quaternion QuaternionFromAxisAngle(Vec3 Axis, Float32 Angle, Bool8 Normalize) {
    const Float32 HalfAngle = 0.5f * Angle;
    Float32 S = Fsin(HalfAngle);
    Float32 C = Fcos(HalfAngle);

    Quaternion Q = (Quaternion){.x = S * Axis.x, .y = S * Axis.y, .z = S * Axis.z, .w = C};
    if (Normalize) {
        return QuaternionNormalize(Q);
    }
    return Q;
}

/**
 * @brief Performs spherical linear interpolation between two quaternions[cite: 7].
 * 
 * @param Q0 The starting quaternion.
 * @param Q1 The ending quaternion.
 * @param Percentage The interpolation percentage (0.0 to 1.0).
 * @return The interpolated quaternion.
 */
FINLINE Quaternion QuaternionSlerp(Quaternion Q0, Quaternion Q1, Float32 Percentage) {
    Quaternion OutQuaternion;
    Quaternion V0 = QuaternionNormalize(Q0);
    Quaternion V1 = QuaternionNormalize(Q1);

    Float32 Dot = QuaternionDot(V0, V1);

    if (Dot < 0.0f) {
        V1.x = -V1.x;
        V1.y = -V1.y;
        V1.z = -V1.z;
        V1.w = -V1.w;
        Dot = -Dot;
    }

    const Float32 DotThreshold = 0.9995f;
    if (Dot > DotThreshold) {
        OutQuaternion = (Quaternion){
            .x = V0.x + ((V1.x - V0.x) * Percentage),
            .y = V0.y + ((V1.y - V0.y) * Percentage),
            .z = V0.z + ((V1.z - V0.z) * Percentage),
            .w = V0.w + ((V1.w - V0.w) * Percentage)};

        return QuaternionNormalize(OutQuaternion);
    }

    Float32 Theta0 = Facos(Dot);
    Float32 Theta = Theta0 * Percentage;
    Float32 SinTheta = Fsin(Theta);
    Float32 SinTheta0 = Fsin(Theta0);

    Float32 S0 = Fcos(Theta) - Dot * SinTheta / SinTheta0;
    Float32 S1 = SinTheta / SinTheta0;

    return (Quaternion){
        .x = (V0.x * S0) + (V1.x * S1),
        .y = (V0.y * S0) + (V1.y * S1),
        .z = (V0.z * S0) + (V1.z * S1),
        .w = (V0.w * S0) + (V1.w * S1)};
}

// ------------------------------------------
// Matrix 4x4 (Out-Pointer & PascalCase Refactor)
// ------------------------------------------

/**
 * @brief Creates and populates an identity matrix.
 * 
 * @param OutMatrix A pointer to the matrix to be filled.
 * @return A pointer to the populated identity matrix.
 */
FINLINE Mat4* Mat4Identity(Mat4* OutMatrix) {
    FMZeroMemory(OutMatrix->Data, sizeof(Float32) * 16);
    OutMatrix->Data[0] = 1.0f;
    OutMatrix->Data[5] = 1.0f;
    OutMatrix->Data[10] = 1.0f;
    OutMatrix->Data[15] = 1.0f;
    return OutMatrix;
}

/**
 * @brief Multiplies Matrix0 and Matrix1 and stores the result in OutMatrix.
 * 
 * @param Matrix0 The first matrix.
 * @param Matrix1 The second matrix.
 * @param OutMatrix A pointer to the destination matrix.
 * @return A pointer to the resulting matrix.
 */
FINLINE Mat4* Mat4Mul(Mat4* Matrix0, Mat4* Matrix1, Mat4* OutMatrix) {
    Mat4 Temp;
    Mat4Identity(&Temp);

    const Float32* M1Ptr = Matrix0->Data;
    const Float32* M2Ptr = Matrix1->Data;
    Float32* DstPtr = Temp.Data;

    for (Int32 i = 0; i < 4; ++i) {
        for (Int32 j = 0; j < 4; ++j) {
            *DstPtr =
                M1Ptr[0] * M2Ptr[0 + j] +
                M1Ptr[1] * M2Ptr[4 + j] +
                M1Ptr[2] * M2Ptr[8 + j] +
                M1Ptr[3] * M2Ptr[12 + j];
            DstPtr++;
        }
        M1Ptr += 4;
    }
    
    FMCopyMemory(OutMatrix->Data, Temp.Data, sizeof(Float32) * 16);
    return OutMatrix;
}

/**
 * @brief Creates an orthographic projection matrix.
 * 
 * @param Left The left side of the view frustum.
 * @param Right The right side of the view frustum.
 * @param Bottom The bottom side of the view frustum.
 * @param Top The top side of the view frustum.
 * @param NearClip The near clipping plane distance.
 * @param FarClip The far clipping plane distance.
 * @param OutMatrix A pointer to the matrix to be filled.
 * @return A pointer to the orthographic matrix. 
 */
FINLINE Mat4* Mat4Orthographic(Float32 Left, Float32 Right, Float32 Bottom, Float32 Top, Float32 NearClip, Float32 FarClip, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);

    Float32 Lr = 1.0f / (Left - Right);
    Float32 Bt = 1.0f / (Bottom - Top);
    Float32 Nf = 1.0f / (NearClip - FarClip);

    OutMatrix->Data[0] = -2.0f * Lr;
    OutMatrix->Data[5] = -2.0f * Bt;
    OutMatrix->Data[10] = 2.0f * Nf;

    OutMatrix->Data[12] = (Left + Right) * Lr;
    OutMatrix->Data[13] = (Top + Bottom) * Bt;
    OutMatrix->Data[14] = (FarClip + NearClip) * Nf;
    
    return OutMatrix;
}

/**
 * @brief Creates a perspective matrix.
 * 
 * @param FovRadians The field of view in radians.
 * @param AspectRatio The aspect ratio.
 * @param NearClip The near clipping plane distance.
 * @param FarClip The far clipping plane distance.
 * @param OutMatrix A pointer to the matrix to be filled.
 * @return A pointer to the perspective matrix. 
 */
FINLINE Mat4* Mat4Perspective(Float32 FovRadians, Float32 AspectRatio, Float32 NearClip, Float32 FarClip, Mat4* OutMatrix) {
    Float32 HalfTanFov = Ftan(FovRadians * 0.5f);
    FMZeroMemory(OutMatrix->Data, sizeof(Float32) * 16); 
    OutMatrix->Data[0] = 1.0f / (AspectRatio * HalfTanFov);
    OutMatrix->Data[5] = 1.0f / HalfTanFov;
    OutMatrix->Data[10] = -((FarClip + NearClip) / (FarClip - NearClip));
    OutMatrix->Data[11] = -1.0f;
    OutMatrix->Data[14] = -((2.0f * FarClip * NearClip) / (FarClip - NearClip));

    return OutMatrix;
}

/**
 * @brief Creates a look-at matrix.
 * 
 * @param Position The position of the matrix.
 * @param Target The position to look at.
 * @param Up The up vector.
 * @param OutMatrix A pointer to the matrix to be filled.
 * @return A pointer to the look-at matrix. 
 */
FINLINE Mat4* Mat4LookAt(Vec3 Position, Vec3 Target, Vec3 Up, Mat4* OutMatrix) {
    Vec3 ZAxis;
    ZAxis.x = Target.x - Position.x;
    ZAxis.y = Target.y - Position.y;
    ZAxis.z = Target.z - Position.z;

    ZAxis = Vec3Normalized(ZAxis);
    Vec3 XAxis = Vec3Normalized(Vec3Cross(ZAxis, Up));
    Vec3 YAxis = Vec3Cross(XAxis, ZAxis);

    OutMatrix->Data[0] = XAxis.x;
    OutMatrix->Data[1] = YAxis.x;
    OutMatrix->Data[2] = -ZAxis.x;
    OutMatrix->Data[3] = 0.0f;
    OutMatrix->Data[4] = XAxis.y;
    OutMatrix->Data[5] = YAxis.y;
    OutMatrix->Data[6] = -ZAxis.y;
    OutMatrix->Data[7] = 0.0f;
    OutMatrix->Data[8] = XAxis.z;
    OutMatrix->Data[9] = YAxis.z;
    OutMatrix->Data[10] = -ZAxis.z;
    OutMatrix->Data[11] = 0.0f;
    OutMatrix->Data[12] = -Vec3Dot(XAxis, Position);
    OutMatrix->Data[13] = -Vec3Dot(YAxis, Position);
    OutMatrix->Data[14] = Vec3Dot(ZAxis, Position);
    OutMatrix->Data[15] = 1.0f;

    return OutMatrix;
}

FINLINE Mat4* Mat4EulerX(Float32 AngleRadians, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    Float32 C = Fcos(AngleRadians);
    Float32 S = Fsin(AngleRadians);

    OutMatrix->Data[5] = C;
    OutMatrix->Data[6] = S;
    OutMatrix->Data[9] = -S;
    OutMatrix->Data[10] = C;
    return OutMatrix;
}

FINLINE Mat4* Mat4EulerY(Float32 AngleRadians, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    Float32 C = Fcos(AngleRadians);
    Float32 S = Fsin(AngleRadians);

    OutMatrix->Data[0] = C;
    OutMatrix->Data[2] = -S;
    OutMatrix->Data[8] = S;
    OutMatrix->Data[10] = C;
    return OutMatrix;
}

FINLINE Mat4* Mat4EulerZ(Float32 AngleRadians, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    Float32 C = Fcos(AngleRadians);
    Float32 S = Fsin(AngleRadians);

    OutMatrix->Data[0] = C;
    OutMatrix->Data[1] = S;
    OutMatrix->Data[4] = -S;
    OutMatrix->Data[5] = C;
    return OutMatrix;
}

FINLINE Mat4* Mat4EulerXYZ(Float32 XRadians, Float32 YRadians, Float32 ZRadians, Mat4* OutMatrix) {
    Mat4 Rx, Ry, Rz;
    Mat4EulerX(XRadians, &Rx);
    Mat4EulerY(YRadians, &Ry);
    Mat4EulerZ(ZRadians, &Rz);

    Mat4 Temp;
    Mat4Mul(&Rx, &Ry, &Temp);
    Mat4Mul(&Temp, &Rz, OutMatrix);
    return OutMatrix;
}

/**
 * @brief Returns a forward vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Forward(const Mat4* Matrix) {
    Vec3 Forward;
    Forward.x = -Matrix->Data[2];
    Forward.y = -Matrix->Data[6];
    Forward.z = -Matrix->Data[10];
    Vec3Normalize(&Forward);
    return Forward;
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Backward(const Mat4* Matrix) {
    Vec3 Backward;
    Backward.x = Matrix->Data[2];
    Backward.y = Matrix->Data[6];
    Backward.z = Matrix->Data[10];
    Vec3Normalize(&Backward);
    return Backward;
}

/**
 * @brief Returns an upward vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Up(const Mat4* Matrix) {
    Vec3 Up;
    Up.x = Matrix->Data[1];
    Up.y = Matrix->Data[5];
    Up.z = Matrix->Data[9];
    Vec3Normalize(&Up);
    return Up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Down(const Mat4* Matrix) {
    Vec3 Down;
    Down.x = -Matrix->Data[1];
    Down.y = -Matrix->Data[5];
    Down.z = -Matrix->Data[9];
    Vec3Normalize(&Down);
    return Down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Left(const Mat4* Matrix) {
    Vec3 Left;
    Left.x = -Matrix->Data[0];
    Left.y = -Matrix->Data[4];
    Left.z = -Matrix->Data[8];
    Vec3Normalize(&Left);
    return Left;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param Matrix A pointer to the base matrix.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4Right(const Mat4* Matrix) {
    Vec3 Right;
    Right.x = Matrix->Data[0];
    Right.y = Matrix->Data[4];
    Right.z = Matrix->Data[8];
    Vec3Normalize(&Right);
    return Right;
}

FINLINE Mat4* Mat4Translation(Vec3 Position, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    OutMatrix->Data[12] = Position.x;
    OutMatrix->Data[13] = Position.y;
    OutMatrix->Data[14] = Position.z;
    return OutMatrix;
}

FINLINE Mat4* Mat4Scale(Vec3 Scale, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    OutMatrix->Data[0] = Scale.x;
    OutMatrix->Data[5] = Scale.y;
    OutMatrix->Data[10] = Scale.z;
    return OutMatrix;
}

/**
 * @brief Inverts the provided matrix in place.
 * 
 * @param Matrix A pointer to the matrix to be inverted (modified in place).
 * @return A pointer to the inverted matrix. 
 */
FINLINE Mat4* Mat4Inverse(Mat4* Matrix) {
    // Copy the input values so we can safely read them while writing back
    Mat4 Temp = *Matrix;
    const Float32* m = Temp.Data;

    Float32 t0 = m[10] * m[15];
    Float32 t1 = m[14] * m[11];
    Float32 t2 = m[6] * m[15];
    Float32 t3 = m[14] * m[7];
    Float32 t4 = m[6] * m[11];
    Float32 t5 = m[10] * m[7];
    Float32 t6 = m[2] * m[15];
    Float32 t7 = m[14] * m[3];
    Float32 t8 = m[2] * m[11];
    Float32 t9 = m[10] * m[3];
    Float32 t10 = m[2] * m[7];
    Float32 t11 = m[6] * m[3];
    Float32 t12 = m[8] * m[13];
    Float32 t13 = m[12] * m[9];
    Float32 t14 = m[4] * m[13];
    Float32 t15 = m[12] * m[5];
    Float32 t16 = m[4] * m[9];
    Float32 t17 = m[8] * m[5];
    Float32 t18 = m[0] * m[13];
    Float32 t19 = m[12] * m[1];
    Float32 t20 = m[0] * m[9];
    Float32 t21 = m[8] * m[1];
    Float32 t22 = m[0] * m[5];
    Float32 t23 = m[4] * m[1];

    Float32* o = Matrix->Data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    Float32 det = m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3];
    if (Fabs(det) < F_FLOAT_EPSILON) {
        Mat4Identity(Matrix);
        return Matrix;
    }

    Float32 d = 1.0f / det;

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return Matrix;
}

FINLINE Mat4* Mat4Transposed(const Mat4* Matrix, Mat4* OutMatrix) {
    Mat4Identity(OutMatrix);
    OutMatrix->Data[0] = Matrix->Data[0];
    OutMatrix->Data[1] = Matrix->Data[4];
    OutMatrix->Data[2] = Matrix->Data[8];
    OutMatrix->Data[3] = Matrix->Data[12];
    OutMatrix->Data[4] = Matrix->Data[1];
    OutMatrix->Data[5] = Matrix->Data[5];
    OutMatrix->Data[6] = Matrix->Data[9];
    OutMatrix->Data[7] = Matrix->Data[13];
    OutMatrix->Data[8] = Matrix->Data[2];
    OutMatrix->Data[9] = Matrix->Data[6];
    OutMatrix->Data[10] = Matrix->Data[10];
    OutMatrix->Data[11] = Matrix->Data[14];
    OutMatrix->Data[12] = Matrix->Data[3];
    OutMatrix->Data[13] = Matrix->Data[7];
    OutMatrix->Data[14] = Matrix->Data[11];
    OutMatrix->Data[15] = Matrix->Data[15];
    return OutMatrix;
}

/**
 * @brief Converts provided degrees to radians.
 * 
 * @param Degrees The degrees to be converted.
 * @return The amount in radians.
 */
FINLINE Float32 DegreesToRadians(Float32 Degrees) {
    return Degrees * F_DEG2RAD_MULTIPLIER;
}

/**
 * @brief Converts provided radians to degrees.
 * 
 * @param Radians The radians to be converted.
 * @return The amount in degrees.
 */
FINLINE Float32 RadiansToDegrees(Float32 Radians) {
    return Radians * F_RAD2DEG_MULTIPLIER;
}
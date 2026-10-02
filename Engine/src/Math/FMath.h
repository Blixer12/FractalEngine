#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

#include "Defines.h"
#include "MathDef.h"
#include "Core/Memory.h"

#define F_PI 3.14159265358979323846f
#define F_PI_2 (2.0f * F_PI)
#define F_HALF_PI (0.5f * F_PI)
#define F_QUARTER_PI (0.25f * F_PI)
#define F_ONE_OVER_PI (1.0f / F_PI)
#define F_ONE_OVER_TWO_PI (1.0f / F_PI_2)
#define F_SQRT_TWO 1.41421356237309504880f
#define F_SQRT_THREE 1.73205080756887729352f
#define F_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define F_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define F_DEG2RAD_MULTIPLIER (F_PI / 180.0f)
#define F_RAD2DEG_MULTIPLIER (180.0f / F_PI)

// The multiplier to convert seconds to milliseconds.
#define F_SEC_TO_MS_MULTIPLIER 1000.0f

// The multiplier to convert milliseconds to seconds.
#define F_MS_TO_SEC_MULTIPLIER 0.001f

// A huge number that should be larger than any valid number used.
#define F_INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define F_FLOAT_EPSILON 1.192092896e-07f

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
// Matrix 4x4
// ------------------------------------------

/**
 * @brief Creates and returns an identity matrix.
 * 
 * @return A new identity matrix 
 */
FINLINE Mat4 Mat4Identity() {
    Mat4 Matrix;
    FMZeroMemory(Matrix.Data, sizeof(Float32) * 16);
    Matrix.Data[0] = 1.0f;
    Matrix.Data[5] = 1.0f;
    Matrix.Data[10] = 1.0f;
    Matrix.Data[15] = 1.0f;
    return Matrix;
}

/**
 * @brief Returns the result of multiplying matrix_0 and matrix_1.
 * 
 * @param matrix_0 The first matrix to be multiplied.
 * @param matrix_1 The second matrix to be multiplied.
 * @return The result of the matrix multiplication.
 */
FINLINE Mat4 Mat4Mul(Mat4 matrix_0, Mat4 matrix_1) {
    Mat4 Matrix = Mat4Identity();

    const Float32* m1_ptr = matrix_0.Data;
    const Float32* m2_ptr = matrix_1.Data;
    Float32* dst_ptr = Matrix.Data;

    for (Int32 i = 0; i < 4; ++i) {
        for (Int32 j = 0; j < 4; ++j) {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }
    return Matrix;
}

/**
 * @brief Creates and returns an orthographic projection matrix. Typically used to
 * render flat or 2D scenes.
 * 
 * @param left The left side of the view frustum.
 * @param right The right side of the view frustum.
 * @param bottom The bottom side of the view frustum.
 * @param top The top side of the view frustum.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new orthographic projection matrix. 
 */
FINLINE Mat4 Mat4_orthographic(Float32 left, Float32 right, Float32 bottom, Float32 top, Float32 near_clip, Float32 far_clip) {
    Mat4 Matrix = Mat4Identity();

    Float32 lr = 1.0f / (left - right);
    Float32 bt = 1.0f / (bottom - top);
    Float32 nf = 1.0f / (near_clip - far_clip);

    Matrix.Data[0] = -2.0f * lr;
    Matrix.Data[5] = -2.0f * bt;
    Matrix.Data[10] = 2.0f * nf;

    Matrix.Data[12] = (left + right) * lr;
    Matrix.Data[13] = (top + bottom) * bt;
    Matrix.Data[14] = (far_clip + near_clip) * nf;
    return Matrix;
}

/**
 * @brief Creates and returns a perspective matrix. Typically used to render 3d scenes.
 * 
 * @param fov_radians The field of view in radians.
 * @param aspect_ratio The aspect ratio.
 * @param near_clip The near clipping plane distance.
 * @param far_clip The far clipping plane distance.
 * @return A new perspective matrix. 
 */
FINLINE Mat4 Mat4_perspective(Float32 fov_radians, Float32 aspect_ratio, Float32 near_clip, Float32 far_clip) {
    Float32 half_tan_fov = Ftan(fov_radians * 0.5f);
    Mat4 Matrix;
    FMZeroMemory(Matrix.Data, sizeof(Float32) * 16);
    Matrix.Data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    Matrix.Data[5] = 1.0f / half_tan_fov;
    Matrix.Data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    Matrix.Data[11] = -1.0f;
    Matrix.Data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
    return Matrix;
}

/**
 * @brief Creates and returns a look-at matrix, or a matrix looking 
 * at target from the perspective of position.
 * 
 * @param position The position of the matrix.
 * @param target The position to "look at".
 * @param up The up vector.
 * @return A matrix looking at target from the perspective of position. 
 */
FINLINE Mat4 Mat4_look_at(Vec3 position, Vec3 target, Vec3 up) {
    Mat4 Matrix;
    Vec3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis = Vec3Normalized(z_axis);
    Vec3 x_axis = Vec3Normalized(Vec3Cross(z_axis, up));
    Vec3 y_axis = Vec3Cross(x_axis, z_axis);

    Matrix.Data[0] = x_axis.x;
    Matrix.Data[1] = y_axis.x;
    Matrix.Data[2] = -z_axis.x;
    Matrix.Data[3] = 0.0f;
    Matrix.Data[4] = x_axis.y;
    Matrix.Data[5] = y_axis.y;
    Matrix.Data[6] = -z_axis.y;
    Matrix.Data[7] = 0.0f;
    Matrix.Data[8] = x_axis.z;
    Matrix.Data[9] = y_axis.z;
    Matrix.Data[10] = -z_axis.z;
    Matrix.Data[11] = 0.0f;
    Matrix.Data[12] = -Vec3Dot(x_axis, position);
    Matrix.Data[13] = -Vec3Dot(y_axis, position);
    Matrix.Data[14] = Vec3Dot(z_axis, position);
    Matrix.Data[15] = 1.0f;

    return Matrix;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->columns)
 * 
 * @param matrix The matrix to be transposed.
 * @return A transposed copy of the provided matrix.
 */
FINLINE Mat4 Mat4_transposed(Mat4 matrix) {
    Mat4 Matrix = Mat4Identity();
    Matrix.Data[0] = matrix.Data[0];
    Matrix.Data[1] = matrix.Data[4];
    Matrix.Data[2] = matrix.Data[8];
    Matrix.Data[3] = matrix.Data[12];
    Matrix.Data[4] = matrix.Data[1];
    Matrix.Data[5] = matrix.Data[5];
    Matrix.Data[6] = matrix.Data[9];
    Matrix.Data[7] = matrix.Data[13];
    Matrix.Data[8] = matrix.Data[2];
    Matrix.Data[9] = matrix.Data[6];
    Matrix.Data[10] = matrix.Data[10];
    Matrix.Data[11] = matrix.Data[14];
    Matrix.Data[12] = matrix.Data[3];
    Matrix.Data[13] = matrix.Data[7];
    Matrix.Data[14] = matrix.Data[11];
    Matrix.Data[15] = matrix.Data[15];
    return Matrix;
}

/**
 * @brief Creates and returns an inverse of the provided matrix.
 * 
 * @param matrix The matrix to be inverted.
 * @return An inverted copy of the provided matrix. 
 */
FINLINE Mat4 Mat4_inverse(Mat4 matrix) {
    const Float32* m = matrix.Data;

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

    Mat4 Matrix;
    Float32* o = Matrix.Data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    Float32 det = m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3];
    if (Fabs(det) < F_FLOAT_EPSILON) {
        return Mat4Identity();
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

FINLINE Mat4 Mat4_translation(Vec3 position) {
    Mat4 Matrix = Mat4Identity();
    Matrix.Data[12] = position.x;
    Matrix.Data[13] = position.y;
    Matrix.Data[14] = position.z;
    return Matrix;
}

/**
 * @brief Returns a scale matrix using the provided scale.
 * 
 * @param scale The 3-component scale.
 * @return A scale matrix.
 */
FINLINE Mat4 Mat4_scale(Vec3 scale) {
    Mat4 Matrix = Mat4Identity();
    Matrix.Data[0] = scale.x;
    Matrix.Data[5] = scale.y;
    Matrix.Data[10] = scale.z;
    return Matrix;
}

FINLINE Mat4 Mat4_euler_x(Float32 angle_radians) {
    Mat4 Matrix = Mat4Identity();
    Float32 c = Fcos(angle_radians);
    Float32 s = Fsin(angle_radians);

    Matrix.Data[5] = c;
    Matrix.Data[6] = s;
    Matrix.Data[9] = -s;
    Matrix.Data[10] = c;
    return Matrix;
}

FINLINE Mat4 Mat4_euler_y(Float32 angle_radians) {
    Mat4 Matrix = Mat4Identity();
    Float32 c = Fcos(angle_radians);
    Float32 s = Fsin(angle_radians);

    Matrix.Data[0] = c;
    Matrix.Data[2] = -s;
    Matrix.Data[8] = s;
    Matrix.Data[10] = c;
    return Matrix;
}

FINLINE Mat4 Mat4_euler_z(Float32 angle_radians) {
    Mat4 Matrix = Mat4Identity();

    Float32 c = Fcos(angle_radians);
    Float32 s = Fsin(angle_radians);

    Matrix.Data[0] = c;
    Matrix.Data[1] = s;
    Matrix.Data[4] = -s;
    Matrix.Data[5] = c;
    return Matrix;
}

FINLINE Mat4 Mat4_euler_xyz(Float32 x_radians, Float32 y_radians, Float32 z_radians) {
    Mat4 rx = Mat4_euler_x(x_radians);
    Mat4 ry = Mat4_euler_y(y_radians);
    Mat4 rz = Mat4_euler_z(z_radians);
    Mat4 Matrix = Mat4Mul(rx, ry);
    Matrix = Mat4Mul(Matrix, rz);
    return Matrix;
}

/**
 * @brief Returns a forward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_forward(Mat4 matrix) {
    Vec3 forward;
    forward.x = -matrix.Data[2];
    forward.y = -matrix.Data[6];
    forward.z = -matrix.Data[10];
    Vec3Normalize(&forward);
    return forward;
}

/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_backward(Mat4 matrix) {
    Vec3 backward;
    backward.x = matrix.Data[2];
    backward.y = matrix.Data[6];
    backward.z = matrix.Data[10];
    Vec3Normalize(&backward);
    return backward;
}

/**
 * @brief Returns an upward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_up(Mat4 matrix) {
    Vec3 up;
    up.x = matrix.Data[1];
    up.y = matrix.Data[5];
    up.z = matrix.Data[9];
    Vec3Normalize(&up);
    return up;
}

/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_down(Mat4 matrix) {
    Vec3 down;
    down.x = -matrix.Data[1];
    down.y = -matrix.Data[5];
    down.z = -matrix.Data[9];
    Vec3Normalize(&down);
    return down;
}

/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_left(Mat4 matrix) {
    Vec3 left;
    left.x = -matrix.Data[0];
    left.y = -matrix.Data[4];
    left.z = -matrix.Data[8];
    Vec3Normalize(&left);
    return left;
}

/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * @return A 3-component directional vector.
 */
FINLINE Vec3 Mat4_right(Mat4 matrix) {
    Vec3 right;
    right.x = matrix.Data[0];
    right.y = matrix.Data[4];
    right.z = matrix.Data[8];
    Vec3Normalize(&right);
    return right;
}

// ------------------------------------------
// Quaternion
// ------------------------------------------

FINLINE Quaternion QuaternionIdentity() {
    return (Quaternion){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

FINLINE Float32 QuaternionNormal(Quaternion q) {
    return Fsqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w);
}

FINLINE Quaternion QuaternionNormalize(Quaternion q) {
    Float32 normal = QuaternionNormal(q);
    if (normal > 0.0f) {
        return (Quaternion){
            .x = q.x / normal,
            .y = q.y / normal,
            .z = q.z / normal,
            .w = q.w / normal};
    }
    return QuaternionIdentity();
}

FINLINE Quaternion QuaternionConjugate(Quaternion q) {
    return (Quaternion){
        .x = -q.x,
        .y = -q.y,
        .z = -q.z,
        .w = q.w};
}

FINLINE Quaternion QuaternionInverse(Quaternion q) {
    return QuaternionNormalize(QuaternionConjugate(q));
}

FINLINE Quaternion QuaternionMul(Quaternion q_0, Quaternion q_1) {
    Quaternion q;

    q.x = q_0.x * q_1.w +
          q_0.y * q_1.z -
          q_0.z * q_1.y +
          q_0.w * q_1.x;

    q.y = -q_0.x * q_1.z +
           q_0.y * q_1.w +
           q_0.z * q_1.x +
           q_0.w * q_1.y;

    q.z = q_0.x * q_1.y -
          q_0.y * q_1.x +
          q_0.z * q_1.w +
          q_0.w * q_1.z;

    q.w = -q_0.x * q_1.x -
           q_0.y * q_1.y -
           q_0.z * q_1.z +
           q_0.w * q_1.w;

    return q;
}

FINLINE Float32 QuaternionDot(Quaternion q_0, Quaternion q_1) {
    return q_0.x * q_1.x +
           q_0.y * q_1.y +
           q_0.z * q_1.z +
           q_0.w * q_1.w;
}

FINLINE Mat4 QuaternionToMat4(Quaternion q) {
    Mat4 Matrix = Mat4Identity();

    Quaternion n = QuaternionNormalize(q);

    Matrix.Data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    Matrix.Data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    Matrix.Data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    Matrix.Data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    Matrix.Data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    Matrix.Data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    Matrix.Data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    Matrix.Data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    Matrix.Data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return Matrix;
}

// Calculates a rotation matrix based on the Quaternion and the passed in center point.
FINLINE Mat4 QuaternionToRotationMatrix(Quaternion q, Vec3 center) {
    Mat4 Matrix;

    Float32* o = Matrix.Data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    return Matrix;
}

FINLINE Quaternion QuaternionFromAxisAngle(Vec3 axis, Float32 angle, Bool8 normalize) {
    const Float32 HalfAngle = 0.5f * angle;
    Float32 s = Fsin(HalfAngle);
    Float32 c = Fcos(HalfAngle);

    Quaternion q = (Quaternion){.x = s * axis.x, .y = s * axis.y, .z = s * axis.z, .w = c};
    if (normalize) {
        return QuaternionNormalize(q);
    }
    return q;
}

FINLINE Quaternion QuaternionSlerp(Quaternion q_0, Quaternion q_1, Float32 percentage) {
    Quaternion q_out;

    Quaternion v0 = QuaternionNormalize(q_0);
    Quaternion v1 = QuaternionNormalize(q_1);

    Float32 dot = QuaternionDot(v0, v1);

    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const Float32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        q_out = (Quaternion){
            .x = v0.x + ((v1.x - v0.x) * percentage),
            .y = v0.y + ((v1.y - v0.y) * percentage),
            .z = v0.z + ((v1.z - v0.z) * percentage),
            .w = v0.w + ((v1.w - v0.w) * percentage)};

        return QuaternionNormalize(q_out);
    }

    Float32 theta_0 = Facos(dot);
    Float32 theta = theta_0 * percentage;
    Float32 sin_theta = Fsin(theta);
    Float32 sin_theta_0 = Fsin(theta_0);

    Float32 s0 = Fcos(theta) - dot * sin_theta / sin_theta_0;
    Float32 s1 = sin_theta / sin_theta_0;

    return (Quaternion){
        .x = (v0.x * s0) + (v1.x * s1),
        .y = (v0.y * s0) + (v1.y * s1),
        .z = (v0.z * s0) + (v1.z * s1),
        .w = (v0.w * s0) + (v1.w * s1)};
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
FINLINE Float32 RadianToDegree(Float32 Radians) {
    return Radians * F_RAD2DEG_MULTIPLIER;
}

#pragma clang diagnostic pop
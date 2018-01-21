#pragma once
#include <stdint.h>

struct RotationMatrix;
struct Quaternion {
    float x;
    float y;
    float z;
    float w;
    Quaternion() noexcept;
    Quaternion(const RotationMatrix &matrix) noexcept;
    Quaternion(const Quaternion &copy) noexcept;
};

struct Vector3D {
    float x;
    float y;
    float z;
};

struct RotationMatrix {
    Vector3D v[3];
    RotationMatrix() noexcept;
    RotationMatrix(const Quaternion &quaternion) noexcept;
    RotationMatrix(const RotationMatrix &copy) noexcept;
};

/// Interpolate a quaternion.
void interpolate_quat(const Quaternion &in_before, const Quaternion &in_after, Quaternion &out, float scale) noexcept;

/// Interpolate a normalized 3D vector.
void interpolate_vector_rotation(const Vector3D &before, const Vector3D &after, Vector3D &output, float scale) noexcept;

typedef void (*interpolate_vector_fn)(const Vector3D&,const Vector3D&,Vector3D&,float);

/// Interpolate a 3D vector.
void interpolate_vector(const Vector3D &before, const Vector3D &after, Vector3D &output, float scale) noexcept;

/// Interpolate a 3D vector, but add the delta to the after vector instead of the before vector. This is to remove the
/// "one tick behind" effect that interpolate_vector normally causes, but at the cost of accuracy.
void interpolate_vector_predict(const Vector3D &before, const Vector3D &after, Vector3D &output, float scale) noexcept;

/// Calculate the distance between two 3D points.
float distance(float x1, float y1, float z1, float x2, float y2, float z2) noexcept;

/// Calculate the distance between two 3D points.
float distance(const Vector3D &a, const Vector3D &b) noexcept;

/// Calculate the distance between two 3D points without taking the square root.
float distance_squared(float x1, float y1, float z1, float x2, float y2, float z2) noexcept;

/// Calculate the distance between two 3D points without taking the square root.
float distance_squared(const Vector3D &a, const Vector3D &b) noexcept;

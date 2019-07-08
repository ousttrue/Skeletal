#include "dxm.h"

namespace dxm
{

const Vec3 Vec3::Zero = Vec3(0, 0, 0);
const Vec3 Vec3::One = Vec3(1, 1, 1);

const Vec4 Vec4::Zero = Vec4(0, 0, 0, 0);
const Vec4 Vec4::One = Vec4(1, 1, 1, 1);

const Quaternion Quaternion::Identity = Quaternion(0, 0, 0, 1);

const Matrix Matrix::Identity = Matrix(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1);

} // namespace dxm

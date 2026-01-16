
set(STELL_MATH_SRCS
    AxisAngle.h
    Constants.h
    Matrix33.h
    Matrix44.h
    Quaternion.h
    Vector3.h
    Vector4.h
    Vector.h
)

target_sources(stellarium PRIVATE ${STELL_MATH_SRCS})

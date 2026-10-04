#include <Core/Logger.h>
#include <Math/FMath.h>

/**
 * @brief Expects Expected to be equal to Actual.
 */
#define ExpectShouldBe(Expected, Actual)                                                                 \
    if (Actual != Expected) {                                                                            \
        FLERROR("--> Expected %lld, but got: %lld. File: %s:%d.", Expected, Actual, __FILE__, __LINE__); \
        return false;                                                                                    \
    }

/**
 * @brief Expects Expected to NOT be equal to Actual.
 */
#define ExpectShouldNotBe(Expected, Actual)                                                                      \
    if (Actual == Expected) {                                                                                    \
        FLERROR("--> Expected %d != %d, but they are equal. File: %s:%d.", Expected, Actual, __FILE__, __LINE__);\
        return false;                                                                                            \
    }

/**
 * @brief Expects Expected to be Actual given a tolerance of F_FLOAT_EPSILON.
 */
#define ExpectFloatToBe(Expected, Actual)                                                            \
    if (Fabs(Expected - Actual) > 0.001f) {                                                          \
        FLERROR("--> Expected %f, but got: %f. File: %s:%d.", Expected, Actual, __FILE__, __LINE__); \
        return false;                                                                                \
    }

/**
 * @brief Expects Actual to be true.
 */
#define ExpectToBeTrue(Actual)                                                          \
    if (Actual != true) {                                                               \
        FLERROR("--> Expected true, but got: false. File: %s:%d.", __FILE__, __LINE__); \
        return false;                                                                   \
    }

/**
 * @brief Expects Actual to be false.
 */
#define ExpectToBbeFalse(Actual)                                                        \
    if (Actual != false) {                                                              \
        FLERROR("--> Expected false, but got: true. File: %s:%d.", __FILE__, __LINE__); \
        return false;                                                                   \
    }
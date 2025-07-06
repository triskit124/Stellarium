#ifndef STELL_MATH_BASE_H
#define STELL_MATH_BASE_H

#include <stdexcept>

#include "Constants.h"

namespace Stellarium
{

    class MathBase
    {
        public:

            MathBase() = default;
            virtual ~MathBase() = default;

            /**
            * @brief Sets the epsilon value for floating point comparisons.
            * @param e The new epsilon value.
            */
            void setEpsilon(double e) { 
                if (e < 0.0) {
                    throw std::invalid_argument("Epsilon must be a non-negative value");
                }
                _epsilon = e; 
            };

            /**
            * @brief Gets the epsilon value for floating point comparisons.
            * @return The epsilon value.
            */
            double getEpsilon() const { return _epsilon; };

        protected:

            /**
            * @brief The epsilon value for floating point comparisons.
            */
            double _epsilon { STELL_EPSILON };

    };

}

#endif // STELL_MATH_BASE_H
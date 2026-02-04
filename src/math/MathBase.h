#pragma once

#include <stdexcept>

#include "Constants.h"

namespace Stellarium
{

    class MathBase
    {
        public:

            /**
            * @brief Default constructor.
            */
            MathBase() = default;

            /**
            * @brief Default destructor.
            */
            virtual ~MathBase() = default;

            /**
            * @brief Sets the epsilon value for floating point comparisons.
            * @param e The new epsilon value. Must be non-negative.
            * @throws std::invalid_argument if e is negative.
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

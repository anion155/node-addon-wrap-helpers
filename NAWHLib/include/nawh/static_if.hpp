#pragma once
//=======================================================================
// Copyright (c) 2013-2016 Baptiste Wicht.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <utility>
#include <nawh/utilities.hpp>

namespace nawh {
namespace static_if_detail {

/*!
 * \brief Identify functor
 */
struct identity {
    /*!
     * \brief Returns exactly what was passsed as argument
     */
    template <typename T>
    T operator()(T&& x) const {
        return std::forward<T>(x);
    }
};

/*!
 * \brief Helper for static if
 *
 * This base type is when the condition is true
 */
template <bool Cond>
struct statement {
    /*!
     * \brief Execute the if part of the statement
     * \param f The functor to execute
     */
    template <typename F>
    void then(const F& f) {
        f(identity());
    }

    /*!
     * \brief Execute the else part of the statement
     * \param f The functor to execute
     */
    template <typename F>
    void else_(const F& f) {
        nawh::UNUSED(f);
    }
};

/*!
 * \brief Helper for static if
 *
 * Specialization for condition is false
 */
template <>
struct statement<false> {
    /*!
     * \brief Execute the if part of the statement
     * \param f The functor to execute
     */
    template <typename F>
    void then(const F& f) {
        nawh::UNUSED(f);
    }

    /*!
     * \brief Execute the else part of the statement
     * \param f The functor to execute
     */
    template <typename F>
    void else_(const F& f) {
        f(identity());
    }
};

} //end of namespace static_if_detail

/*!
 * \brief Execute the lambda if the static condition is verified
 *
 * This should be usd to auto lambda to ensure instantiation is only made for
 * the "true" branch
 *
 * \tparam Cond The static condition
 * \param f The lambda to execute if true
 * \return a statement object to execute else_ if necessary
 */
template <bool Cond, typename F>
static_if_detail::statement<Cond> static_if(F const& f) {
    static_if_detail::statement<Cond> if_;
    if_.then(f);
    return if_;
}
}

#ifdef __cpp_if_constexpr
#  define STATIC_IF(expr) if constexpr (expr)
#  define STATIC_ELSE
#  define STATIC_END_IF
#else
#  define STATIC_IF(expr) nawh::static_if<(expr)>([&](const auto &)
#  define STATIC_ELSE ).else_([&](const auto &)
#  define STATIC_END_IF );
#endif

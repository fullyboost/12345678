# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.  Permission to copy, use,        *
#  *     modify, sell, and distribute this software is granted provided       *
#  *     this copyright notice appears in all copies.  This software is       *
#  *     provided "as is" without express or implied warranty, and with       *
#  *     no claim at to its suitability for any purpose.                      *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_SET_TRANSFORM_HPP
# define BOOST_PREPROCESSOR_SET_TRANSFORM_HPP
#
# include <boost/preprocessor/config/config.hpp>
# include <boost/preprocessor/set/fold_left.hpp>
# include <boost/preprocessor/set/set.hpp>
# include <boost/preprocessor/tuple/elem.hpp>
# include <boost/preprocessor/tuple/rem.hpp>
#
# /* BOOST_PP_SET_TRANSFORM */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_SET_TRANSFORM(op, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT(BOOST_PP_SET_TRANSFORM_O, (op, data, (nil)), set)))
# else
#    define BOOST_PP_SET_TRANSFORM(op, data, set) BOOST_PP_SET_TRANSFORM_I(op, data, set)
#    define BOOST_PP_SET_TRANSFORM_I(op, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT(BOOST_PP_SET_TRANSFORM_O, (op, data, (nil)), set)))
# endif
#
# if BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_STRICT()
#    define BOOST_PP_SET_TRANSFORM_O(s, state, elem) BOOST_PP_SET_TRANSFORM_O_IM(s, BOOST_PP_TUPLE_REM_3 state, elem)
#    define BOOST_PP_SET_TRANSFORM_O_IM(s, im, elem) BOOST_PP_SET_TRANSFORM_O_I(s, im, elem)
# else
#    define BOOST_PP_SET_TRANSFORM_O(s, state, elem) BOOST_PP_SET_TRANSFORM_O_I(s, BOOST_PP_TUPLE_ELEM(3, 0, state), BOOST_PP_TUPLE_ELEM(3, 1, state), BOOST_PP_TUPLE_ELEM(3, 2, state), elem)
# endif
#
# define BOOST_PP_SET_TRANSFORM_O_I(s, op, data, res, elem) (op, data, res (op(s, data, elem)))
#
# /* BOOST_PP_SET_TRANSFORM_S */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_SET_TRANSFORM_S(s, op, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT_ ## s(BOOST_PP_SET_TRANSFORM_O, (op, data, (nil)), set)))
# else
#    define BOOST_PP_SET_TRANSFORM_S(s, op, data, set) BOOST_PP_SET_TRANSFORM_S_I(s, op, data, set)
#    define BOOST_PP_SET_TRANSFORM_S_I(s, op, data, set) BOOST_PP_SET_TAIL(BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_SET_FOLD_LEFT_ ## s(BOOST_PP_SET_TRANSFORM_O, (op, data, (nil)), set)))
# endif
#
# endif

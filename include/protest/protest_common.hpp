/*!
 * \file   protest_common.hpp
 * \brief  Common types and utility functions.
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */
#pragma	once
#ifndef	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_
#define	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_

#include <iosfwd>
#include <type_traits>
#include <tuple>
// To get macros such as UINT64_MAX, define __STDC_LIMIT_MACROS.
// For detail, see P257 of ISO/IEC 9899:1999 (N1124),
// "7.18.2 Limits of specified-width integer types".
#define	__STDC_LIMIT_MACROS
#include <cstdint>

#ifdef	__has_include
#	if __has_include(<optional>)
#		include <optional>
#		define	HAVE_STD_OPTIONAL_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_optional = std;
}
}
#	elif __has_include(<experimental/optional>)
#		include <experimental/optional>
#		define	HAVE_STD_EXPERIMENTAL_OPTIONAL_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_optional = std::experimental;
}
}
#	elif __has_include(<boost/optional.hpp>
#		include <boost/optional.hpp>
#		define	HAVE_BOOST_OPTIONAL_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_optional = boost;
}
}
#	else
#		error	None of std(::experimental)::optional and boost::optional not available
#	endif
#	if __has_include(<any>)
#		include <any>
#		define	HAVE_STD_EXPERIMENTAL_ANY_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_any = std;
}
}
#	elif __has_include(<experimental/any>)
#		include <experimental/any>
#		define	HAVE_STD_EXPERIMENTAL_ANY_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_any = std::experimental;
}
}
#	elif __has_include(<boost/any.hpp>)
#		include <boost/any.hpp>
#		define	HAVE_BOOST_ANY_
namespace	nu11p0 {
namespace	protest {
	namespace	ns_any = boost;
}
}
#	else
#		error	None of std(::experimental)::any and boost::any not available
#	endif
#else
#	error	No __has_include feature test macro support.
#endif

namespace	nu11p0 {
namespace	protest {

/*
 * optional
 */
#ifdef	HAVE_BOOST_OPTIONAL_
#	define	PROTEST_NULLOPT_T	ns_optional::none_t
#	define	PROTEST_NULLOPT		ns_optional::none
#else
#	define	PROTEST_NULLOPT_T	ns_optional::nullopt_t
#	define	PROTEST_NULLOPT		ns_optional::nullopt
#endif

#if 0
namespace	detail {

	//! Check whether type T is printable with std::ostream.
	template <typename T, typename =void>
	struct	OstreamPrintable : public std::false_type {};
	template <typename T>
	struct	OstreamPrintable<
			T,
			std::enable_if_t<decltype(
				std::declval<std::ostream &>() << std::declval<T>(),
				std::true_type{}
			)::value>
		>
		: public std::true_type
	{};

} // namespace detail

//! Print object with std::ostream.
template <typename T, typename=void>
struct	Printer {
	template <typename U>
	auto			operator()(std::ostream &ost, const U &val)
		-> std::enable_if_t<detail::OstreamPrintable<U>::value>
	{
		ost << val;
	}
	template <typename U>
	auto			operator()(std::ostream &ost, const U &val)
		-> std::enable_if_t<
			!detail::OstreamPrintable<U>::value
			&& decltype(std::begin(val), void(0), std::true_type{}){}
		>
	{
		bool			commaRequired = false;
		Printer<decltype(*(std::begin(val)))>	printer;
		ost << '{';
		for(const auto &x : val) {
			if(std::exchange(commaRequired, true)) {
				ost << ',';
			}
			printer(ost, x);
		}
		ost << '}';
	}
	template <typename U>
	auto			operator()(std::ostream &ost, const U &val)
		-> std::enable_if_t<
			!detail::OstreamPrintable<U>::value
			&& !decltype(std::begin(val), void(0), std::true_type{}){}
		>
	{
		ost << "(unprintable)";
	}
}; // struct Printer
#endif

//! Test result for all checked cases.
/*!
 * テストの結果を、テスト自体とは独立して保持するためのクラス。
 */
struct	TestResult {
	//! テストの説明。
	std::string		abstract;
	//! 最後のテストで用いられたテストケースについての説明。
	std::string		lastCaseDescription;
	//! 最後に失敗したテストケース。
	ns_any::any		failedCase;
	//! テスト失敗の原因についての説明。
	ns_optional::optional<std::string>	reason;
	//! passの回数。
	size_t			passCount;
	//! skipの回数。
	size_t			skipCount;
	//TestResult() = default;
	TestResult()
	:abstract()
	,lastCaseDescription()
	,failedCase()
	,reason()
	,passCount()
	,skipCount()
	{}
	bool			isTestFailed(void) {
		return	!failedCase.empty();
	}
	// 失敗したテストについての情報のみをリセットする。
	void			clearError(void) {
		failedCase.clear();
		reason = PROTEST_NULLOPT;
	}
	// テスト結果のリセットであって、テストに関する情報の全てを削除するわけではないことに注意。
	// すなわち、テストの概要(m_abstract)はリセットされない。
	void			clearAll(void) {
		failedCase.clear();
		reason = PROTEST_NULLOPT;
		passCount = 0;
		skipCount = 0;
	}
}; // struct TestResult

//! Test result for a sequential test.
/*!
 * Sequential testやGeneric testなど、複数のSimpleTestが行われるようなテストの結果を表現する。
 */
struct	SequentialTestResult {
	//! テストの結果。
	TestResult		result;
	//! 失敗したテストの番号(順番)。
	//! テストが成功したとき、この値は意味を持たない。
	size_t			failedIndex;
	SequentialTestResult()
	:result()
	,failedIndex()
	{}
}; // struct SequentialTestResult

//! Print test result.
void			printResult(std::ostream &, const TestResult &);

namespace	detail {
	template <typename Tuple, size_t currentIndex, typename F>
	auto			passAsNthTypeImpl(F &f, const ns_any::any &obj, size_t targetIndex)
		-> std::enable_if_t<
			(
				!(currentIndex < std::tuple_size<Tuple>{}-1)
				&& !std::is_void<std::result_of_t<F(std::tuple_element_t<currentIndex, Tuple>)>>{}
			)
			, ns_any::any>
	{
		return	f(ns_any::any_cast<std::tuple_element_t<currentIndex, Tuple>>());
	}
	template <typename Tuple, size_t currentIndex, typename F>
	auto			passAsNthTypeImpl(F &f, const ns_any::any &obj, size_t targetIndex)
		-> std::enable_if_t<
			(
				!(currentIndex < std::tuple_size<Tuple>{}-1)
				&& std::is_void<std::result_of_t<F(std::tuple_element_t<currentIndex, Tuple>)>>{}
			)
			, ns_any::any>
	{
		f(ns_any::any_cast<std::tuple_element_t<currentIndex, Tuple>>(obj));
		return	{};
	}
	template <typename Tuple, size_t currentIndex, typename F>
	auto			passAsNthTypeImpl(F &f, const ns_any::any &obj, size_t targetIndex)
		-> std::enable_if_t<
			(
				(currentIndex < std::tuple_size<Tuple>{}-1)
				&& !std::is_void<std::result_of_t<F(std::tuple_element_t<currentIndex, Tuple>)>>{}
			)
			, ns_any::any>
	{
		if(currentIndex == targetIndex) {
			return	f(ns_any::any_cast<std::tuple_element_t<currentIndex, Tuple>>());
		} else {
			return	passAsNthTypeImpl<Tuple, currentIndex+1>(f, obj, targetIndex);
		}
	}
	template <typename Tuple, size_t currentIndex, typename F>
	auto			passAsNthTypeImpl(F &f, const ns_any::any &obj, size_t targetIndex)
		-> std::enable_if_t<
			(
				(currentIndex < std::tuple_size<Tuple>{}-1)
				&& std::is_void<std::result_of_t<F(std::tuple_element_t<currentIndex, Tuple>)>>{}
			)
			, ns_any::any>
	{
		if(currentIndex == targetIndex) {
			f(ns_any::any_cast<std::tuple_element_t<currentIndex, Tuple>>(obj));
			return	{};
		} else {
			return	passAsNthTypeImpl<Tuple, currentIndex+1>(f, obj, targetIndex);
		}
	}
}

//! Print test case for sequential test.
// FIXME: Should `obj' be const reference? Is there any problem if it is non-const reference?
template <typename Tuple, typename F>
ns_any::any		passAsNthType(F &&f, const ns_any::any &obj, size_t index)
{
	return	detail::passAsNthTypeImpl<Tuple, 0>(f, obj, index);
}

//! Test result for a single test case.
/*!
 * TestResultがテスト全体の結果を保持するのに対して、
 * CheckResultは各テストケースについての結果を保持する。
 */
struct	CheckResult {
	//! テスト失敗の原因についての説明。
	ns_optional::optional<std::string>	reason;
	CheckResult() = default;
	bool			isTestFailed(void) {
		return	static_cast<bool>(reason);
	}
	// True if failed.
	// for use as
	//    if(auto &&res = funWhichReturnsCheckResult()) {
	//      // if failed...
	//    }
	explicit		operator bool()
	{
		return	isTestFailed();
	}
}; // struct CheckResult


namespace	detail {
	template <typename F, typename=void>
	struct	is_callable_impl : std::false_type {};
#if	defined(__cpp_lib_result_of_sfinae) && __cpp_lib_result_of_sfinae
	// TODO: untested.
	//       std::result_of がSFINAEに使えるようになったら確かめること。
	template <typename F>
	struct	is_callable_impl<F, std::result_of_t<F>> : std::true_type {};
#else
	template <typename F>
	struct	is_callable_impl<F,
			std::enable_if_t<decltype(
				std::declval<std::result_of_t<F>>(),
				void(0), std::true_type{}
			){}>
		> : std::true_type {};
#endif
} // namespace detail

template <typename F, typename... Args>
struct is_callable : detail::is_callable_impl<F(Args...)> {};

namespace	detail {
	template <typename F, typename... Fs>
	struct	OverloadImpl : F, protected OverloadImpl<Fs...> {
		using	Rest = OverloadImpl<Fs...>;
		OverloadImpl(F &&f, Fs&&... fs)
		:F(std::forward<F>(f))
		,Rest(std::forward<Fs>(fs)...)
		{}
		//using	F::operator();
		template <typename... Args>
		auto			operator()(Args&&... args)
			-> std::enable_if_t<is_callable<F, Args...>{}, std::result_of_t<F(Args...)>>
		{
			return	(*static_cast<F *>(this))(std::forward<Args>(args)...);
		}
		template <typename... Args>
		auto			operator()(Args&&... args)
			-> std::enable_if_t<!is_callable<F, Args...>{}, std::result_of_t<Rest(Args...)>>
		{
			return	(*static_cast<Rest *>(this))(std::forward<Args>(args)...);
		}
		template <typename R, typename... Args>
		operator		std::enable_if_t<is_callable<F, Args...>{}, std::function<R(Args...)>>()
		{
			return	std::function<R(Args...)>(*static_cast<F *>(this));
		}
	};
	template <typename F>
	struct	OverloadImpl<F> : F {
		OverloadImpl(F &&f)
		:F(std::forward<F>(f))
		{}
		using	F::operator();
		template <typename R, typename... Args>
		operator		std::enable_if_t<is_callable<F, Args...>{}, std::function<R(Args...)>>()
		{
			return	std::function<R(Args...)>(*static_cast<F *>(this));
		}
	};
} // namespace detail

/*
 * NOTE: overload(Fs&&...)にすると、lvalueな変数で関数を渡したとき、
 *       "no known conversion from 'Foo' to 'Foo&&'"と言われる。
 *       moveできる必要のあるような関数は渡されないだろう、という前提のもと、
 *       引数はコピー渡しできることとする。
 *       どうせstd::functionに渡すやつもコピーで渡されるし、いいよね？
 */
template <typename... Fs>
auto			overload(Fs... fs)
	-> detail::OverloadImpl<
		std::decay_t<Fs>...
	>
{
	return	{std::move(fs)...};
}

template <typename... Tuples>
using	tuple_concat_t = decltype(std::tuple_cat(std::declval<Tuples&&>()...));

template <typename Tuple, typename... Ts>
using	tuple_append_t = decltype(std::tuple_cat(std::declval<Tuple&&>(), std::declval<std::tuple<Ts...>&&>()));

//! Integer types for all size.
using	Integers = std::tuple<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t
#ifdef	INT64_MAX
			, int64_t
#endif
#ifdef	UINT64_MAX
			, uint64_t
#endif
		>;
//! Signed integer types for all size.
using	SignedIntegers = std::tuple<int8_t, int16_t, int32_t
#ifdef	INT64_MAX
			, int64_t
#endif
		>;
//! Unsigned integer types for all size.
using	UnsignedIntegers = std::tuple<uint8_t, uint16_t, uint32_t
#ifdef	UINT64_MAX
			, uint64_t
#endif
		>;
//! Floating-point number types for all size.
using	Floats = std::tuple<float, double, long double>;

} // namespace protest
} // namespace nu11p0
#endif	// ifndef INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_

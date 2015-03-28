/*!
 * \file   protest_test.cpp
 * \brief  
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */

#include <iostream>
#include <functional>
#include <tuple>
#include <limits>
#include <cmath>

#include <typeinfo>

#include <protest/generic_test.hpp>
#include <protest/condition.hpp>

//using namespace	nu11p0::protest;
namespace	protest = nu11p0::protest;

int fun(int a)
{
	return	a;
}

//template <typename T, typename Less=std::less<T>, typename Negate=std::negate<T>>
//T			absolute_simple(const T &val)
//{
	//const T		negative = Negate()(val);
	//return	(Less()(val, negative) ? negative : val);
//}
template <typename T, typename Less=std::less<T>, typename Negate=std::negate<T>>
T			absolute(const T &val, Less l=Less(), Negate n=Negate())
{
	const T		negative = n(val);
	return	(l(val, negative) ? negative : val);
}

struct	AbsoluteWrapper {
	template <typename T>
	auto			operator()(const T &arg) {
		return	absolute<T>(arg);
	}
}; // struct TemplateFunctionWrapperAbsolute

template <typename F>
struct	TemplateIdempotent {
	template <typename T>
	protest::CheckResult	operator()(const T &arg)
	{
		protest::CheckResult	result;
		const auto		ret1 = F()(arg);
		if(!(ret1 == F()(ret1))) {
			result.reason.emplace("given function is not idempotent.");
		}
		return	result;
	}
};

template <typename F>
struct	TemplateFunctionPredicateWrapper {
	template <typename T>
	struct	type {
		protest::CheckResult	operator()(const T &arg) {
			return	F()(arg);
		}
	};
};

int				absoluteInt64Test(void)
{
	{
		protest::SimpleTest<int64_t>	test(
				"Idempotence test for absolute<int64_t>",
				//protest::Idempotent<int64_t>(absolute_simple<int64_t>),
				protest::Idempotent<int64_t>([](auto x){ return absolute<int64_t>(x); }),
				[](int64_t arg) {
					return (arg != std::numeric_limits<int64_t>::min());
				}
			);
		auto			result = test.runTest(
				"random case",
				protest::case_gen::Random<int64_t>(),
				20,
				std::cout);
		printResult(std::cout, result);
		if(result.isTestFailed()) {
			std::cout << "     | failed case: " << protest::ns_any::any_cast<int64_t>(result.failedCase) << std::endl;
			return	1;
		}
		test.clearAll();

		result = test.runTest(
				"edge case",
				protest::case_gen::Edge<int64_t>(),
				20,
				std::cout);

		printResult(std::cout, result);
		if(result.isTestFailed()) {
			std::cout << "     | failed case: " << protest::ns_any::any_cast<int64_t>(result.failedCase) << std::endl;
			return	2;
		}
	}
	{
		protest::SimpleTest<int64_t>	test(
				"Positivity test for absolute<int64_t>",
				[](int64_t arg) {
					return	protest::AssertResult(absolute<int64_t>(arg) >= 0, "return value is still negative");
				},
				[](int64_t arg) {
					return (arg != std::numeric_limits<int64_t>::min());
				}
			);

		auto			result = test.runTest(
				"random case",
				protest::case_gen::Random<int64_t>(),
				20,
				std::cout);
		printResult(std::cout, result);
		if(result.isTestFailed()) {
			std::cout << "     | failed case: " << protest::ns_any::any_cast<int64_t>(result.failedCase) << std::endl;
			return	3;
		}
		test.clearAll();

		result = test.runTest(
				"edge case",
				protest::case_gen::Edge<int64_t>(),
				20,
				std::cout);

		printResult(std::cout, result);
		if(result.isTestFailed()) {
			std::cout << "     | failed case: " << protest::ns_any::any_cast<int64_t>(result.failedCase) << std::endl;
			return	4;
		}
	}
	return	0;
}

int				absoluteTemplateNumTest(void)
{
	//using	Nums = protest::tuple_append_t<Integers, float, double, long double>;
	using	Nums = protest::tuple_concat_t<protest::Integers, protest::Floats>;

	auto			absolutePrecondition = protest::overload(
			// signedな整数の場合は、最小値でないことを確認する。
			[](auto x) -> std::enable_if_t<std::is_integral<decltype(x)>{} && std::is_signed<decltype(x)>{}, bool>
			{
				return	(x != std::numeric_limits<decltype(x)>::min());
			}
			// 浮動小数点数の場合は、NaNでないことを確認する。
			// (つまり、正規化数、非正規化数、ゼロ、無限大については処理を行う。)
			, [](auto x) -> std::enable_if_t<std::is_floating_point<decltype(x)>{}, bool>
			{
				return	!std::isnan(x);
			}
			, [](auto)
			{
				return	true;
			}
		);
	auto			casePrinter = protest::overload(
			[](auto x) -> std::enable_if_t<std::is_floating_point<decltype(x)>{}, void> {
				std::cout << "(floating point)(" << x << ')';
			}
			, [](auto x) -> std::enable_if_t<std::is_integral<decltype(x)>{} && std::is_signed<decltype(x)>{}, void> {
				std::cout << "(signed integral)(" << x << ')';
			}
			, [](auto x) {
				std::cout << "(unsigned integral)(" << x << ')';
			}
		);

	auto			result = protest::generic::test<
			protest::generic::Random
			, Nums
		>(
			"absolute<>() template function positivity test"
			, "random case"
			, [](auto x) {
				return	protest::AssertResult((absolute(x) >= 0), "return value is still negative");
			}
			, absolutePrecondition
			, 35, std::cout
			// You can specify function to print result. protest::printResult() is used if omitted.
			//, [](std::ostream &ost, const protest::TestResult &res) {
				//ost << "(generic test) ";
				//printResult(ost, res);
			//}
		);
	// std::coutを指定した時点で、デフォルトでprintResult()による結果出力がされるので、テスト後のprintResult()は不要。

	if(result.result.isTestFailed()) {
		std::cout << "     | failed case: ";
		protest::passAsNthType<Nums>(
				casePrinter
				, result.result.failedCase
				, result.failedIndex);
		std::cout << std::endl;
		return	2;
	}
	result.result.clearAll();

	result = protest::generic::test<
			protest::generic::Edge
			, Nums
		>(
			"absolute<>() template function positivity test"
			, "edge case"
			, [](auto x) {
				return	protest::AssertResult((absolute(x) >= 0), "return value is still negative");
			}
			, absolutePrecondition
			, 50, std::cout
		);

	if(result.result.isTestFailed()) {
		std::cout << "     | failed case: ";
		protest::passAsNthType<Nums>(
				casePrinter
				, result.result.failedCase
				, result.failedIndex);
		std::cout << std::endl;
		return	2;
	}
	result.result.clearAll();
	return	0;
}

int				protestGenericUtilTest(void)
{
	auto			fun1_fallback = [](const auto &) {
			return	"const auto &";
		};
	auto			fun1 = protest::overload(
				// #1
				[](auto x) -> std::enable_if_t<std::is_integral<decltype(x)>{} && std::is_signed<decltype(x)>{}, const char *>
				{
					return	"(signed integral)";
				}
				// #2
				, [](int *x) {
					return	"int *";
				}
				// #3
				, [](const char *x) {
					return	"const char *";
				}
				// #4
				, [](auto x) -> std::enable_if_t<std::is_integral<decltype(x)>{}, const char *>
				{
					return	"(integral, not signed)";
				}
				// #5
				,[](std::string &&x) {
					return	"std::string &&";
				}
				// #6
				// If you want only 'float', you should use 'auto' and SFINAE.
				, [](auto x) -> std::enable_if_t<std::is_same<decltype(x), float>{}, const char *>
				{
					return	"(auto, exactly float)";
				}
				// #7
				// This function is used for double, long double, and
				// all other types which can be converted implicitly to float.
				// Note that NOT this function but the above function ([](auto x)) is called for float.
				, [](float) {
					return	"float";
				}
				// #8
				, fun1_fallback
				// #9
				// The below will never called because the priority is lower than
				// fun1_fallback (which is [](auto){}).
				, [](double) {
					return	"double";
				}
		);

	// #3
	if(fun1("cstr") != std::string("const char *")) {
		std::cout<<"error:"<<fun1("cstr")<<" for const char *"<<std::endl;
		return	1;
	}
	// #5
	if(fun1(std::string("std::string")) != std::string("std::string &&")) {
		std::cout<<"error:"<<fun1(std::string("std::string"))<<" for std::string"<<std::endl;
		return	2;
	}
	// #7
	if(fun1(3.14) != std::string("float")) {
		std::cout<<"error:"<<fun1(3.14)<<" for double"<<std::endl;
		return	4;
	}
	// #6
	if(fun1(3.14f) != std::string("(auto, exactly float)")) {
		std::cout<<"error:"<<fun1(3.14f)<<" for float"<<std::endl;
		return	4;
	}
	// #1
	if(fun1(3) != std::string("(signed integral)")) {
		std::cout<<"error:"<<fun1(3)<<"for signed integer"<<std::endl;
		return	5;
	}
	// You can convert overloaded functions to std::function.
	std::function<const char *(unsigned long)>	fun1_cchar_ulong(fun1);
	// force #4 (with std::function cast).
	if(fun1_cchar_ulong(-3) != std::string("(integral, not signed)")) {
		std::cout<<"error:"<<fun1_cchar_ulong(-3)<<std::endl;
		return	6;
	}

	return	0;
}


template <uint64_t num>
struct	Factorial : std::integral_constant<uint64_t, num*Factorial<num-1>{}> {};
template <>
struct	Factorial<0> : std::integral_constant<uint64_t, 1> {};

int				mplTest(void)
{
	std::cout << "fact(5) = " << Factorial<5>{} << std::endl;
	std::cout << "fact(10) = " << Factorial<10>{} << std::endl;
	std::cout << "fact(15) = " << Factorial<15>{} << std::endl;
	std::cout << "fact(20) = " << Factorial<20>{} << std::endl;
	std::cout << "max      = " << std::numeric_limits<uint64_t>::max() << std::endl;
	std::cout << "fact(21) = " << Factorial<21>{} << std::endl;
	// FIXME: unimplemented.
	//        コンパイル時疑似乱数を使ってテストケース自動生成したい。
	return	0;
}

int				main(void)
{
	int				ret;
	if(( ret = absoluteInt64Test() )) {
		return	ret;
	}
	if(( ret = absoluteTemplateNumTest() )) {
		return	ret;
	}
	if(( ret = protestGenericUtilTest() )) {
		return	ret;
	}
	if(( ret = mplTest() )) {
		return	0;
	}

	return	0;
}


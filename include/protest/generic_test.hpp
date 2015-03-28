/*!
 * \file   generic_test.hpp
 * \brief  Test for generic functions.
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */
#pragma	once
#ifndef	INCLUDED__NU11P0__PROTEST__GENERIC_TEST_HPP_
#define	INCLUDED__NU11P0__PROTEST__GENERIC_TEST_HPP_

#ifndef	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_
#	include <protest/protest_common.hpp>
#endif

#ifndef	INCLUDED__NU11P0__PROTEST__SIMPLE_TEST_HPP_
#	include <protest/simple_test.hpp>
#endif

#include <tuple>

namespace	nu11p0 {
namespace	protest {

//! テンプレート関数(ジェネリック関数)のテストに関係する型が入る名前空間。
namespace	generic {

using	Integers = std::tuple<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t>;
using	Floats = std::tuple<float, double, long double>;

//template <typename U>
struct	PreconditionAlwaysTrue {
	template <typename T>
	inline bool		operator()(const T &) {
		return	true;
	}
}; // struct PreconditionAlwaysTrue

namespace	detail {
	template <typename param_type, typename TCG, typename F, typename PreCon>
	TestResult		testImpl(TCG &&testcaseGenerator, const std::string &abstract, const std::string &caseDescription, const F &f, const PreCon &precon, size_t maxCount, std::ostream &ost, std::function<void(std::ostream &, const TestResult &)> &printer)
	{
		bool			ostreamAvailable = std::addressof(ost);
		protest::SimpleTest<param_type>	test(
				std::string(abstract),
				f,
				precon
			);
		auto			result = test.runTest(
				std::string(caseDescription),
				std::forward<TCG>(testcaseGenerator),
				maxCount,
				ost);
		if(ostreamAvailable && printer) {
			printer(ost, result);
		}
		return	result;
	}
	// TODO: maxCountをtupleにして型ごとに指定できるようにする。
	template <template<class> class TCG, typename Params, typename F, typename PreCon, size_t I=0>
	auto			testImplInterface(const std::string &abstract, const std::string &caseDescription, const F &f, const PreCon &precon, size_t maxCount, std::ostream &ost, std::function<void(std::ostream &, const TestResult &)> &printer)
		-> std::enable_if_t<!(I < std::tuple_size<Params>{}-1), SequentialTestResult>
	{
		using	param_type = std::tuple_element_t<I, Params>;
		SequentialTestResult	result;
		result.result = testImpl<param_type>(
				TCG<param_type>(),
				abstract + " for " + std::to_string(I) + "th type",
				caseDescription,
				f,
				precon,
				maxCount,
				ost,
				printer);
		result.failedIndex = I;
		return	result;
	}
	template <template<class> class TCG, typename Params, typename F, typename PreCon, size_t I=0>
	auto			testImplInterface(const std::string &abstract, const std::string &caseDescription, const F &f, const PreCon &precon, size_t maxCount, std::ostream &ost, std::function<void(std::ostream &, const TestResult &)> &printer)
		-> std::enable_if_t<(I < std::tuple_size<Params>{}-1), SequentialTestResult>
	{
		using	param_type = std::tuple_element_t<I, Params>;
		SequentialTestResult	result;
		result.result = testImpl<param_type>(
				TCG<param_type>(),
				abstract + " for " + std::to_string(I) + "th type",
				caseDescription,
				f,
				precon,
				maxCount,
				ost,
				printer);
		result.failedIndex = I;
		return	result.result.isTestFailed() ? result : testImplInterface<TCG, Params, F, PreCon, I+1>(abstract, caseDescription, f, precon, maxCount, ost, printer);
	}
} // namespace detail

// FとPreConは複数の型に対するテストで使い回されるため、forwardやmoveは行わない。よってconst参照で受け取る。
template <template<class> class TCG, typename Params, typename F, typename PreCon, typename Counts>
SequentialTestResult	test(std::string &&abstract, const std::string &caseDescription, const F &f, const PreCon &precon, Counts maxCount, std::ostream &ost=*static_cast<std::ostream *>(nullptr), std::function<void(std::ostream &, const TestResult &)> printer=printResult)
{
	SequentialTestResult	result;
	result = detail::testImplInterface<TCG, Params>(std::forward<std::string>(abstract), caseDescription, f, precon, maxCount, ost, printer);
	return	result;
}

} // namespace generic

namespace	mpl {

// TODO: コンパイル時疑似乱数生成器を実装せよ

} // namespace	mpl

} // namespace protest
} // namespace nu11p0
#endif	// ifndef INCLUDED__NU11P0__PROTEST__GENERIC_TEST_HPP_

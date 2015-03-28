/*!
 * \file   simple_test.hpp
 * \brief  Basic test.
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */
#pragma	once
#ifndef	INCLUDED__NU11P0__PROTEST__SIMPLE_TEST_HPP_
#define	INCLUDED__NU11P0__PROTEST__SIMPLE_TEST_HPP_

#ifndef	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_
#	include <protest/protest_common.hpp>
#endif
#include <protest/test_base.hpp>

#include <functional>
#include <iosfwd>

namespace	nu11p0 {
namespace	protest {

template <typename T>
class	SimpleTest {
	public:
		using	param_type = T;
	private:
		TestResult			m_result;
		std::function<CheckResult(const param_type &)>	m_fun;
		std::function<bool(const param_type &)>	m_preCondition;
	public:
		template <typename F>
		SimpleTest(std::string &&a, F &&f)
		:m_result()
		,m_fun(std::forward<F>(f))
		,m_preCondition()
		{
			m_result.abstract = std::forward<std::string>(a);
		}
		template <typename F, typename PreCon>
		SimpleTest(std::string &&a, F &&f, PreCon &&precon)
		:m_result()
		,m_fun(std::forward<F>(f))
		,m_preCondition(precon)
		{
			m_result.abstract = std::forward<std::string>(a);
		}
		void			clearError(void)
		{
			m_result.clearError();
		}
		void			clearAll(void)
		{
			m_result.clearAll();
		}
		// maxCountはskipも含めての数。
		template <typename TCG>
		const TestResult	&runTest(std::string &&caseDescription, TCG &&testcaseGenerator, size_t maxCount, std::ostream &ost=*static_cast<std::ostream *>(nullptr))
		{
			bool			ostreamAvailable = std::addressof(ost);
			if(m_result.isTestFailed()) {
				// 以前のテストのエラー情報がクリアされていない。
				return	m_result;
			}
			m_result.lastCaseDescription = std::forward<std::string>(caseDescription);
			size_t			count = 0;
			auto			startTime = std::chrono::steady_clock::now();
			auto			prevTime = startTime;
			bool			progressPrinted = false;
			bool			hasPreCondition = static_cast<bool>(m_preCondition);
			auto			&passCount = m_result.passCount;
			auto			&skipCount = m_result.skipCount;
			if(maxCount) {
				while(auto &&c = testcaseGenerator()) {
					auto			&&param = c.value();
					if(hasPreCondition && !m_preCondition(param)) {
						++skipCount;
						continue;
					}
					if(auto &&res = checkCase(std::forward<param_type>(param))) {
						break;
					}
					++passCount;
					if(++count >= maxCount) {
						break;
					}
#include <protest/loligger_sgr_macro.h>
					if(ostreamAvailable) {
						auto			nowTime = std::chrono::steady_clock::now();
						if((nowTime - prevTime) >= std::chrono::milliseconds(500)) {
							prevTime = nowTime;
							ost << '\r' << "[" SGR(FG_YELLOW) "RUN" SGR(RESET) " ] pass: " << passCount << ", skip: " << skipCount << std::flush;
							progressPrinted = true;
						}
					}
				}
				if(progressPrinted) {
					ost << '\r'
						<< "[" SGR(FG_GREEN) "DONE" SGR(RESET) "] elapsed: "
						<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()
						<< "ms, pass: " << passCount << ", skip: " << skipCount << std::endl;
				}
#define	SGR_UNDEF
#include <protest/loligger_sgr_macro.h>
			}
			return	m_result;
		}
		CheckResult		checkCase(param_type &&arg)
		{
			CheckResult		res = m_fun(arg);
			if(res.isTestFailed()) {
				m_result.failedCase = std::forward<param_type>(arg);
				m_result.reason = res.reason.value();
			}
			return	res;
		}
}; // class SimpleTest

} // namespace protest
} // namespace nu11p0
#endif	// ifndef INCLUDED__NU11P0__PROTEST__SIMPLE_TEST_HPP_

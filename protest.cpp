/*!
 * \file   protest.cpp
 * \brief  
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */
#include <ostream>
#include <protest/protest_common.hpp>

#include <protest/loligger_sgr_macro.h>

namespace	nu11p0 {
namespace	protest {

void			printResult(std::ostream &ost, const TestResult &result)
{
	bool			success = result.failedCase.empty();
	if(success) {
		ost << "[" SGR(BOLD, FG_GREEN) "PASS" SGR(RESET) "] ";
	} else {
		ost << "[" SGR(BOLD, FG_RED) "FAIL" SGR(RESET) "] ";
	}
	ost << result.abstract << " (with test case: " << result.lastCaseDescription << ") (pass=" << result.passCount << ", skip=" << result.skipCount << ')' << std::endl;
	if(!success && result.reason) {
		ost << "     | reason: " << result.reason.value() << std::endl;
	}
}

} // namespace protest
} // namespace nu11p0

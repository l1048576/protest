/*!
 * \file   condition.hpp
 * \brief  Commonly used or convenient conditions for tests.
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/24
 * C++ version: C++14
 */
#pragma	once
#ifndef	INCLUDED__NU11P0__PROTEST__CONDITION_HPP_
#define	INCLUDED__NU11P0__PROTEST__CONDITION_HPP_

#ifndef	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_
#	include <protest/protest_common.hpp>
#endif

namespace	nu11p0 {
namespace	protest {

CheckResult		AssertResult(bool condition, std::string &&reason)
{
	CheckResult		ret;
	if(!condition) {
		ret.reason.emplace(std::forward<std::string>(reason));
	}
	return	ret;
}

template <typename Param>
class	Idempotent {
	public:
		using	param_type = Param;
	private:
		std::function<param_type(const param_type &)>	m_fun;
	public:
		template <typename F>
		Idempotent(F &&f)
		:m_fun(std::forward<F>(f))
		{}
		~Idempotent() = default;
		Idempotent(const Idempotent &) = default;
		Idempotent(Idempotent &&) = default;
		CheckResult		operator()(const param_type &arg)
		{
			const param_type	ret1 = m_fun(arg);
			return	AssertResult(ret1 == m_fun(ret1), "function is not idempotent");
		}
}; // class Idempotent

// ソートされていることを確認する。
// 標準ライブラリにはstd::lessの代わりにstd::less_equalも用意されていることに留意せよ。
template <typename Param, typename Less=std::less<typename Param::value_type>,
		typename=std::enable_if_t<decltype(cbegin(std::declval<Param>()), void(0), std::true_type{})::value>>
class	Sorted {
	public:
		using	param_type = Param;
	private:
		// TODO: 本当は戻り値は同じ型でなくてもいいはず。type erasureでなんとかしてみたい。
		std::function<param_type(const param_type &)>	m_fun;
	public:
		template <typename F>
		Sorted(F f)
		:m_fun(f)
		{}
		~Sorted() = default;
		Sorted(const Sorted &) = default;
		Sorted(Sorted &&) = default;
		CheckResult		operator()(const param_type &param)
		{
			CheckResult		ret;
			Less			less;
			auto			&&converted = m_fun(param);
			auto			it = std::cbegin(converted);
			auto			ite = std::cend(converted);
			// 要素が無ければ無条件に成功。
			if(it != ite) {
				auto			it2 = it;
				while(++it2 != ite) {
					if(less(*it, *it2)) {
						ret.reason.emplace("given data are not sorted");
						break;
					}
					++it;
				}
			}
			return	ret;
		}
}; // class Sorted

} // namespace protest
} // namespace nu11p0
#endif	// ifndef INCLUDED__NU11P0__PROTEST__CONDITION_HPP_

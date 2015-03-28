/*!
 * \file   case_gen.hpp
 * \brief  
 * \author Larry-o <nu11p0.6477@gmail.com>
 * \date   2015/03/28
 * C++ version: C++14
 */
#pragma	once
#ifndef	INCLUDED__NU11P0__PROTEST__CASE_GEN_HPP_
#define	INCLUDED__NU11P0__PROTEST__CASE_GEN_HPP_

#ifndef	INCLUDED__NU11P0__PROTEST__PROTEST_COMMON_HPP_
#	include <protest/protest_common.hpp>
#endif

#include <random>
#include <iterator>

namespace	nu11p0 {
namespace	protest {

namespace	case_gen {

template <typename T>
struct	None {
	ns_optional::optional<T>	operator()(void) {
		return	PROTEST_NULLOPT;
	}
}; // struct None

template <typename T, typename=void>
class	Random;

template <typename T>
class	Random<T, std::enable_if_t<std::is_integral<T>::value>> {
	private:
		std::function<T()>	m_rng;
	public:
		//! Use std::mt19937 or std::mt19937_64 as default pseudo-random number generator.
		Random()
		:m_rng(
				// std::mt19937 等の生成する値は、unsignedな型しか指定できない。
				// よって、signedな値も欲しい場合はキャスト等してやらなければならない。
				// ここでは、とりあえずキャストでなく分布を弄ることで対処した。
				[
					// デフォルトでは0からmax()までなので、明示的にmin()からにする
					dist=std::uniform_int_distribution<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())
					, engine=std::conditional_t<(sizeof(T) > 4), std::mt19937_64, std::mt19937>(std::random_device()())
				]() mutable {
					return	dist(engine);
				}
			)
		{}
		//! Use given (pseudo-)random number generator.
		/*!
		 * rngにはdistributionまで含めて指定してくんろー
		 */
		template <typename U>
		Random(U &&rng)
		:m_rng(std::forward<U>(rng))
		{}
		~Random() = default;
		ns_optional::optional<T>	operator()(void) {
			return	ns_optional::make_optional<T>(m_rng());
		}
}; // class Random

// FIXME: デフォルトで生成される数の種類(正規化数、非正規化数、[0.0, 1.0], [0.0, 1.0), NaN, etc...)の指定をさせるなり、
//        初めから範囲を明確にしておくなり、しっかりした仕様が必要かと。
//        特に、default ctorで初期化された場合に汎用的に使える必要がある(正規化数が良いか？)
//        とりあえず今のところは、[0.0, 1.0)で実装しとくけど、至急修正すること。
template <typename T>
class	Random<T, std::enable_if_t<std::is_floating_point<T>::value>> {
	private:
		std::function<T()>	m_rng;
	public:
		//! Use std::mt19937 or std::mt19937_64 as default pseudo-random number generator.
		Random()
		:m_rng(
				// std::mt19937 等の生成する値は、unsignedな整数型しか指定できない。
				// よって、浮動小数点数が欲しい場合は分布を指定してやる。
				[
					dist=std::uniform_real_distribution<T>()
					, engine=std::conditional_t<(sizeof(T) > 4), std::mt19937_64, std::mt19937>(std::random_device()())
				]() mutable {
					return	dist(engine);
				}
			)
		{}
		//! Use given (pseudo-)random number generator.
		template <typename U>
		Random(U &&rng)
		:m_rng(std::forward<U>(rng))
		{}
		~Random() = default;
		ns_optional::optional<T>	operator()(void) {
			return	ns_optional::make_optional<T>(m_rng());
		}
}; // class Random

template <typename Container,
		typename T=std::decay_t<decltype(*cbegin(std::declval<Container>()))>,
		typename It=std::decay_t<decltype(cbegin(std::declval<Container>()))>,
		typename=std::enable_if_t<std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<It>::iterator_category>{}>
	>
class	IterableSource {
	public:
		using	iterator_type = It;
		using	value_type = T;
	protected:
		iterator_type	m_it;
		iterator_type	m_ite;
	public:
		IterableSource()
		:m_it()
		,m_ite()
		{}
		IterableSource(const Container &obj)
		:m_it(cbegin(obj))
		,m_ite(cend(obj))
		{}
		~IterableSource() = default;
		IterableSource(const IterableSource &) = default;
		IterableSource(IterableSource &&) = default;
		ns_optional::optional<value_type>	operator()(void) {
			// 返す値はコピーで作る。
			// make_optional()はforwardしようとするため、*m_it++がconst参照を返すときエラーになる。
			return	(m_it != m_ite) ? ns_optional::optional<value_type>(*m_it++) : PROTEST_NULLOPT;
		}
}; // class IterableSource

template <typename Container, typename T=std::decay_t<decltype(*std::cbegin(std::declval<Container>()))>>
class	Pool : public IterableSource<Container> {
	public:
		using	value_type = T;
	private:
		Container		m_container;
	public:
		Pool(std::initializer_list<T> l)
		:IterableSource<Container>()
		,m_container(l)
		{
			this->m_it = cbegin(m_container);
			this->m_ite = cend(m_container);
		}
		template <typename... Args>
		Pool(Args&&... args)
		:IterableSource<Container>()
		,m_container(std::forward<Args>(args)...)
		{
			this->m_it = cbegin(m_container);
			this->m_ite = cend(m_container);
		}
		~Pool() = default;
		Pool(const Pool &) = default;
		Pool(Pool &&) = default;
}; // class Pool

// you can call this version in this way: makePool<Container>(foo, bar, baz);
template <typename T, typename... Args>
auto			makePool(Args&&... args)
{
	return	Pool<std::vector<T>>(std::forward<Args>(args)...);
}

// you can call this version in this way: makePool<Container>({foo, bar, baz});
template <typename T>
auto			makePool(std::initializer_list<T> l)
{
	return	Pool<std::vector<T>>{l};
}

template <typename T, typename=void>
class	Edge;

// Edge cases for signed integer types.
template <typename T>
class	Edge<T, std::enable_if_t<(std::is_integral<T>{} && std::is_signed<T>{})>> : public Pool<std::vector<T>> {
	public:
		using	value_type = T;
	private:
		using	limits = std::numeric_limits<T>;
	public:
		Edge()
		:Pool<std::vector<T>>{
			0, 1, -1, 2, -2,
			limits::min(),
			limits::min()+1,
			limits::max(),
			limits::max()-1
		}
		{}
		~Edge() = default;
		Edge(const Edge &) = default;
		Edge(Edge &&) = default;
}; // class Edge

// Edge cases for unsigned integer types.
template <typename T>
class	Edge<T, std::enable_if_t<(std::is_integral<T>{} && std::is_unsigned<T>{})>> : public Pool<std::vector<T>> {
	public:
		using	value_type = T;
	private:
		using	limits = std::numeric_limits<T>;
		using	signed_limits = std::numeric_limits<std::make_signed_t<T>>;
	public:
		Edge()
		:Pool<std::vector<T>>{
			0, 1, 2,
			limits::max(),
			limits::max()-1,
			static_cast<T>(signed_limits::max()),
			static_cast<T>(signed_limits::max()-1),
			static_cast<T>(signed_limits::max()+1)
		}
		{}
		~Edge() = default;
		Edge(const Edge &) = default;
		Edge(Edge &&) = default;
}; // class Edge

// Edge cases for floating point number types.
template <typename T>
class	Edge<T, std::enable_if_t<(std::is_floating_point<T>{})>> : public Pool<std::vector<T>> {
	public:
		using	value_type = T;
	private:
		using	limits = std::numeric_limits<T>;
	public:
		Edge()
		:Pool<std::vector<T>>{
				// zeros
				T(), -T(),
				// 最小の正の正規化数
				static_cast<T>(std::numeric_limits<float>		::min()),
				static_cast<T>(std::numeric_limits<double>		::min()),
				static_cast<T>(std::numeric_limits<long double>	::min()),
				-static_cast<T>(std::numeric_limits<float>			::min()),
				-static_cast<T>(std::numeric_limits<double>			::min()),
				-static_cast<T>(std::numeric_limits<long double>	::min()),
				// 最大値(無限大やNaNではない)
				static_cast<T>(std::numeric_limits<float>		::max()),
				static_cast<T>(std::numeric_limits<double>		::max()),
				static_cast<T>(std::numeric_limits<long double>	::max()),
				-static_cast<T>(std::numeric_limits<float>			::max()),
				-static_cast<T>(std::numeric_limits<double>			::max()),
				-static_cast<T>(std::numeric_limits<long double>	::max()),
				// 機械イプシロン
				static_cast<T>(std::numeric_limits<float>		::epsilon()),
				static_cast<T>(std::numeric_limits<double>		::epsilon()),
				static_cast<T>(std::numeric_limits<long double>	::epsilon()),
				// 正の無限表現
				static_cast<T>(std::numeric_limits<float>		::infinity()),
				static_cast<T>(std::numeric_limits<double>		::infinity()),
				static_cast<T>(std::numeric_limits<long double>	::infinity()),
				-static_cast<T>(std::numeric_limits<float>			::infinity()),
				-static_cast<T>(std::numeric_limits<double>			::infinity()),
				-static_cast<T>(std::numeric_limits<long double>	::infinity()),
				// quiet NaN
				limits::quiet_NaN(),
				// 最小の非正規化数
				static_cast<T>(std::numeric_limits<float>		::denorm_min()),
				static_cast<T>(std::numeric_limits<double>		::denorm_min()),
				static_cast<T>(std::numeric_limits<long double>	::denorm_min()),
				-static_cast<T>(std::numeric_limits<float>			::denorm_min()),
				-static_cast<T>(std::numeric_limits<double>			::denorm_min()),
				-static_cast<T>(std::numeric_limits<long double>	::denorm_min()),
		}
		{}
		~Edge() = default;
		Edge(const Edge &) = default;
		Edge(Edge &&) = default;
}; // class Edge

} // namespace case_gen

namespace	generic {

template <typename T>
using	Random = case_gen::Random<T>;

template <typename T>
using	Edge = case_gen::Edge<T>;

} // namespace generic

} // namespace protest
} // namespace nu11p0
#endif	// ifndef INCLUDED__NU11P0__PROTEST__CASE_GEN_HPP_

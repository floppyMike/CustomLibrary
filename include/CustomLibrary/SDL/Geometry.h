#if not defined _CTL_SDL2_GEO_
#define _CTL_SDL2_GEO_

#include "../BasicTypes.h"
#include "TypeTraits.h"
#include "Renderer.h"

#include <vector>
#include <span>

namespace ctl::sdl
{
	/**
	 * @brief Frame class encompassing all the shapes
	 * @tparam Shape Shape type to use
	 */
	template<typename Shape>
	class Frame
	{
	public:
		constexpr Frame() = default;

		constexpr Frame(const Frame &)	   = default;
		constexpr Frame(Frame &&) noexcept = default;

		constexpr auto operator=(const Frame &) -> Frame & = default;
		constexpr auto operator=(Frame &&) noexcept -> Frame & = default;

		/**
		 * @brief Constructs Frame using the shape s
		 * @param s Shape to use
		 */
		constexpr explicit Frame(const Shape &s)
			: m_shape(s)
		{
		}

		/**
		 * @brief Gives the constant shape
		 * @return Shape
		 */
		constexpr auto shape() const noexcept -> const auto & { return m_shape; }
		/**
		 * @brief Give the shape
		 * @return Shape
		 */
		constexpr auto shape() noexcept -> auto & { return m_shape; }
		/**
		 * @brief Sets the shape
		 * @param s Shape to use
		 */
		constexpr auto shape(const Shape &s) noexcept -> void { m_shape = s; }

	private:
		Shape m_shape;
	};

	/**
	 * @brief Area in shape of a rectangle
	 */
	using RectFrame = Frame<mth::Rect<int, int>>;
	/**
	 * @brief Area in shape of a circle
	 */
	using CircleFrame = Frame<mth::Circle<int, int>>;
	/**
	 * @brief Area in shape of a line
	 */
	using LineFrame = Frame<mth::Line<int>>;
	/**
	 * @brief Area in shape of a point
	 */
	using PointFrame = Frame<mth::Point<int>>;

	/**
	 * @brief CRTP like extensions pattern
	 * @tparam Ex Extension types
	 */
	template<template<typename> class... Ex>
	using ERectFrame = typename MixBuilder<RectFrame, Ex...>::type;
	/**
	 * @brief CRTP like extensions pattern
	 * @tparam Ex Extension types
	 */
	template<template<typename> class... Ex>
	using ECircleFrame = typename MixBuilder<CircleFrame, Ex...>::type;
	/**
	 * @brief CRTP like extensions pattern
	 * @tparam Ex Extension types
	 */
	template<template<typename> class... Ex>
	using ELineFrame = typename MixBuilder<LineFrame, Ex...>::type;
	/**
	 * @brief CRTP like extensions pattern
	 * @tparam Ex Extension types
	 */
	template<template<typename> class... Ex>
	using EPointFrame = typename MixBuilder<PointFrame, Ex...>::type;

	// -----------------------------------------------------------------------------
	// Geometric Helpers
	// -----------------------------------------------------------------------------

	class GenCircle
	{
	public:
		GenCircle(mth::Circle<int, int> c)
		{
			const auto d = c.r * 2;

			mth::Point<int> p(c.r - 1, 0);
			mth::Point<int> tp(1, 1);

			int err = tp.x - d;

			while (p.x >= p.y)
			{
				m_cache.insert(m_cache.end(),
							   { mth::Point{ c.x + p.x, c.y + p.y },
								 { c.x - p.x, c.y + p.y },
								 { c.x + p.x, c.y - p.y },
								 { c.x - p.x, c.y - p.y },
								 { c.x + p.y, c.y + p.x },
								 { c.x - p.y, c.y + p.x },
								 { c.x + p.y, c.y - p.x },
								 { c.x - p.y, c.y - p.x } });

				if (err <= 0)
				{
					++p.y;
					err += tp.y;
					tp.y += 2;
				}
				if (err > 0)
				{
					--p.x;
					tp.x += 2;
					err += tp.x - d;
				}
			}
		}

		/**
		 * @brief Moves the cached circle
		 *
		 * @param dx Delta x coord
		 * @param dy Delta y coord
		 */
		constexpr void mov(int dx, int dy) noexcept
		{
			for (auto &p : m_cache) p.x += dx, p.y += dy;
		}

		/**
		 * @brief Span reference to cache
		 * @return reference
		 */
		constexpr auto cache() const noexcept -> std::span<const mth::Point<int>> { return std::span(m_cache); }

	private:
		std::vector<mth::Point<int>> m_cache;
	};

	// -----------------------------------------------------------------------------
	// Procedures
	// -----------------------------------------------------------------------------

	/**
	 * @brief Gives reinterpreted ptr to SDL shape
	 *
	 * @param o Shape
	 * @return Ptr to shape
	 */
	auto frame_shape_diff(const mth::Rect<int, int> *o) noexcept -> const SDL_Rect *
	{
		return reinterpret_cast<const SDL_Rect *>(o);
	}
	/**
	 * @brief Gives reinterpreted ptr to SDL shape
	 *
	 * @param o Shape
	 * @return Ptr to shape
	 */
	auto frame_shape_diff(const mth::Rect<float, float> *o) noexcept -> const SDL_FRect *
	{
		return reinterpret_cast<const SDL_FRect *>(o);
	}
	/**
	 * @brief Gives reinterpreted ptr to SDL shape
	 *
	 * @param o Shape
	 * @return Ptr to shape
	 */
	auto frame_shape_diff(const mth::Point<int> *o) noexcept -> const SDL_Point *
	{
		return reinterpret_cast<const SDL_Point *>(o);
	}
	/**
	 * @brief Gives reinterpreted ptr to SDL shape
	 *
	 * @param o Shape
	 * @return Ptr to shape
	 */
	auto frame_shape_diff(const mth::Point<float> *o) noexcept -> const SDL_FPoint *
	{
		return reinterpret_cast<const SDL_FPoint *>(o);
	}

	/**
	 * @brief Gives ptr to shape from frame
	 *
	 * @param o Frame
	 * @return Ptr to shape
	 */
	template<typename O>
	auto frame_shape_diff(const Frame<O> *o) noexcept -> const auto *
	{
		return frame_shape_diff(&o->shape());
	}

	void draw(sdl::Renderer &r, mth::Rect<int, int> rect)
	{
		const auto x = SDL_RenderDrawRect(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}

	void draw_filled(sdl::Renderer &r, mth::Rect<int, int> rect)
	{
		const auto x = SDL_RenderFillRect(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}

	/**
	 * @brief Draws the generated circle
	 * @param c the circle generated
	 */
	auto draw(sdl::Renderer &r, const GenCircle &c) -> void
	{
		const auto s = c.cache();
		const auto x = SDL_RenderDrawPoints(r.get(), (const SDL_Point *)s.data(), s.size());
		ASSERT(x == 0, SDL_GetError());
	}
	// /**
	//  * @brief Draws the generated circle filled
	//  */
	// auto draw_filled() -> void
	// {
	// 	const auto r = SDL_RenderDrawLines(this->renderer()->get(), m_cache.data(), m_cache.size());
	// 	ASSERT(r == 0, SDL_GetError());
	// }

} // namespace ctl::sdl

#endif
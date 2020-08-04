#pragma once

#include <SDL2/SDL.h>
#include "../BasicTypes.h"
#include "../utility.h"
#include "TypeTraits.h"

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

} // namespace ctl::sdl

#pragma once

#include <SDL2/SDL.h>

#include "../utility.h"
#include "../BasicTypes.h"

#include "CustomLibrary/SDL/TypeTraits.h"
#include "Renderer.h"
#include "Drawable.h"

#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>
#include <type_traits>
#include <algorithm>

namespace ctl::sdl
{
	template<typename Shape>
	class Frame
	{
	public:
		using tag_t	 = typename Shape::tag;
		using base_t = Frame;

		Frame() = default;

		Frame(const Frame &)	 = default;
		Frame(Frame &&) noexcept = default;

		auto operator=(const Frame &) -> Frame & = default;
		auto operator=(Frame &&) noexcept -> Frame & = default;

		explicit Frame(const Shape &s)
			: m_shape(s)
		{
		}

		constexpr auto shape() const noexcept -> const auto & { return m_shape; }
		constexpr auto shape() noexcept -> auto & { return m_shape; }
		constexpr auto shape(const Shape &s) noexcept -> auto &
		{
			m_shape = s;
			return *this;
		}

		// auto draw(Renderer *r) const noexcept { return Draw<const Frame>(this, r); }

	private:
		Shape m_shape;
	};

	using RectFrame	  = Frame<mth::Rect<int, int>>;
	using CircleFrame = Frame<mth::Circle<int, unsigned int>>;
	using LineFrame	  = Frame<mth::Line<int>>;
	using PointFrame  = Frame<mth::Point<int>>;

	template<template<typename> class... Ex>
	using ERectFrame = typename MixBuilder<RectFrame, Ex...>::type;
	template<template<typename> class... Ex>
	using ECircleFrame = typename MixBuilder<CircleFrame, Ex...>::type;
	template<template<typename> class... Ex>
	using ELineFrame = typename MixBuilder<LineFrame, Ex...>::type;
	template<template<typename> class... Ex>
	using EPointFrame = typename MixBuilder<PointFrame, Ex...>::type;

	template<typename... Shapes>
	class MultiShape
	{
		static_assert(tag::contains_tag_v<Shapes...>, "Shapes must have a tag.");

	public:
		using base_t = MultiShape;
		using tag_t	 = tag::isMultiShape;

		MultiShape() = default;

		template<typename T>
		auto push(const T &arg) -> auto &
		{
			static_assert(std::disjunction_v<std::is_same<Shapes, T>...>, "Type must be of pack.");
			std::get<std::vector<T>>(m_packs).push_back(arg);

			return *this;
		}

		template<typename T, typename... Args>
		auto emplace(Args &&... arg) -> auto &
		{
			static_assert(std::disjunction_v<std::is_same<Shapes, T>...>, "Type must be of pack.");
			std::get<std::vector<T>>(m_packs).push_back(std::forward<Args>(arg)...);

			return *this;
		}

		constexpr auto tuple_data() const noexcept -> const auto & { return m_packs; }

		auto draw(Renderer *r) { return Draw<std::decay_t<decltype(*this)>>(this, r); }

		auto begin() const noexcept { return m_packs.begin(); }
		auto end() const noexcept { return m_packs.end(); }

	private:
		std::tuple<std::vector<Shapes>...> m_packs;
	};

} // namespace ctl::sdl

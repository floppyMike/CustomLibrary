#pragma once

#include <SDL.h>

#include "../utility.h"
#include "../BasicTypes.h"
#include "Renderer.h"
#include "Drawable.h"

#include <vector>
#include <type_traits>

namespace ctl::sdl
{
	template<typename Shape>
	class Frame
	{
	public:
		using tag_t = typename Shape::tag;
		using base_t = Frame;

		Frame() = default;

		Frame(const Frame&) = default;
		Frame(Frame&&) = default;

		Frame& operator=(const Frame&) = default;
		Frame& operator=(Frame&&) = default;

		Frame(const Shape& s)
			: m_shape(s)
		{
		}
		
		constexpr const auto& shape() const noexcept
		{
			return m_shape;
		}

		constexpr auto& shape() noexcept
		{
			return m_shape;
		}

		constexpr auto& shape(const Shape& s) noexcept
		{
			m_shape = s;
			return *this;
		}

		auto draw(Renderer* r)
		{
			return Draw<std::decay_t<decltype(*this)>>(this, r);
		}

	private:
		Shape m_shape;
	};

	using RectFrame = Frame<mth::Rect<int, int>>;
	using CircleFrame = Frame<mth::Circle<int, unsigned int>>;
	using LineFrame = Frame<mth::Line<int>>;
	using PointFrame = Frame<mth::Point<int>>;


	template<typename... Shapes>
	class MultiShape
	{
		static_assert(tag::contains_tag_v<Shapes...>, "Shapes must have a tag.");

	public:
		using base_t = MultiShape;
		using tag_t = tag::isMultiShape;

		MultiShape() = default;

		template<typename T>
		auto& push(const T& arg)
		{
			static_assert(std::disjunction_v<std::is_same<Shapes, T>...>, "Type must be of pack.");
			std::get<std::vector<T>>(m_packs).emplace_back(arg);
			
			return *this;
		}

		constexpr const auto& data() const noexcept { return m_packs; }

		auto draw(Renderer* r)
		{
			return Draw<std::decay_t<decltype(*this)>>(this, r);
		}

	private:
		std::tuple<std::vector<Shapes>...> m_packs;
	};

}
#pragma once

#include <SDL.h>

#include "../utility.h"
#include "../BasicTypes.h"
#include "Renderer.h"

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

		template<typename T>
		void draw_handler(T& arg) const
		{
			using tag_t = typename T::value_type::tag;

			if constexpr (std::is_same_v<ctl::tag::isRect, tag_t>)
				SDL_RenderDrawRects(m_rend->get(), arg.front().rect_ptr(), arg.size());

			else if constexpr (std::is_same_v<ctl::tag::isLine, tag_t>)
				SDL_RenderDrawLines(m_rend->get(), arg.front().point_ptr(), arg.size() << 1);

			else if constexpr (std::is_same_v<ctl::tag::isPoint, tag_t>)
				SDL_RenderDrawLines(m_rend->get(), arg.front().point_ptr(), arg.size());

			else
				assert(false && "Type is not supported for mass drawing.");
				//static_assert(false, "Type is not supported for mass drawing.");
		}

	public:
		MultiShape(sdl::Renderer* r) : m_rend(r) {}

		template<typename T>
		auto& push(const T& arg)
		{
			static_assert(std::disjunction_v<std::is_same<Shapes, T>...>, "Type must be of pack.");
			std::get<std::vector<T>>(m_packs).emplace_back(arg);

			return *this;
		}

		void draw() const
		{
			std::apply([this](auto&&... arg)
				{
					(this->draw_handler(arg), ...);
				}, m_packs);
		}

		constexpr auto& renderer(sdl::Renderer* const r) noexcept
		{
			m_rend = r;
			return *this;
		}

		constexpr auto* renderer() const noexcept
		{
			assert(m_rend != nullptr && "Renderer isn't assigned.");
			return m_rend;
		}

	private:
		Renderer* m_rend = nullptr;
		std::tuple<std::vector<Shapes>...> m_packs;
	};

}
#pragma once

#include <SDL.h>

#include "../utility.h"
#include "../BasicTypes.h"
#include "Renderer.h"

#include <vector>
#include <type_traits>

namespace ctl::sdl
{
	template<typename Shape, template<typename, typename...> class... Ex>
	class Frame : public Ex<Frame<Shape, Ex...>, typename Shape::tag>...
	{
		using tag_t = typename Shape::tag;

	public:
		Frame() = default;

		Frame(const Frame&) = default;
		Frame(Frame&&) = default;

		Frame& operator=(const Frame&) = default;
		Frame& operator=(Frame&&) = default;

		template<template<typename, typename...> class... T>
		Frame(T<Frame, tag_t>&&... arg)
			: T<Frame, tag_t>(std::move(arg))...
		{
		}

		template<template<typename, typename...> class... T>
		Frame(const Frame<Shape, T...>& cast)
			: T<Frame, tag_t>(static_cast<T<Frame, tag_t>>(*reinterpret_cast<T<Frame, tag_t>*>(static_cast<T<Frame<Shape, T...>, tag_t>*>(&cast))))...
			, m_rend(cast.m_rend)
			, m_shape(cast.m_shape)
		{
		}

		template<template<typename, typename...> class... T>
		Frame(Frame<Shape, T...>&& cast)
			: T<Frame, tag_t>(static_cast<T<Frame, tag_t>&&>(*reinterpret_cast<T<Frame, tag_t>*>(static_cast<T<Frame<Shape, T...>, tag_t>*>(&cast))))...
			, m_rend(cast.m_rend)
			, m_shape(cast.m_shape)
		{
		}

		Frame(DRenderer* rend)
			: m_rend(rend)
		{
		}

		Frame(DRenderer* rend, const Shape& s)
			: m_rend(rend)
			, m_shape(s)
		{
		}
		
		constexpr const auto& shape() const noexcept
		{
			return m_shape;
		}
		constexpr auto& shape(const Shape& s) noexcept
		{
			m_shape = s;
			return *this;
		}

		constexpr auto& renderer(sdl::DRenderer* const r) noexcept
		{
			m_rend = r;
			return *this;
		}

		constexpr auto* renderer() const noexcept
		{
			assert(m_rend != nullptr && "Renderer isn't assigned.");
			return m_rend;
		}

		FORWARDING_MEMBER_FUNCTIONS(Shape, m_shape, pos)
		FORWARDING_MEMBER_FUNCTIONS(Shape, m_shape, translate)

	protected:
		DRenderer* m_rend = nullptr;
		Shape m_shape;
	};

	template<template<typename, typename...> class... Ex>
	using RectFrame = Frame<mth::Rect<int, int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using CircleFrame = Frame<mth::Circle<int, unsigned int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using LineFrame = Frame<mth::Line<int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using PointFrame = Frame<mth::Point<int>, Ex...>;


	template<typename... Shapes>
	class MultiShape
	{
		static_assert(tag::contains_tag_v<Shapes...>, "Shapes must have the tag.");

		template<typename T>
		void draw_handler(T& arg) const
		{
			using tag_t = typename T::value_type::tag;

			if constexpr (std::is_same_v<tag::isRect, tag_t>)
				SDL_RenderDrawRects(m_rend->get(), arg.front().rect_ptr(), arg.size());

			else if constexpr (std::is_same_v<tag::isLine, tag_t>)
				SDL_RenderDrawLines(m_rend->get(), arg.front().point_ptr(), arg.size() << 1);

			else if constexpr (std::is_same_v<tag::isPoint, tag_t>)
				SDL_RenderDrawLines(m_rend->get(), arg.front().point_ptr(), arg.size());

			else
				assert(false && "Type is not supported for mass drawing.");
				//static_assert(false, "Type is not supported for mass drawing.");
		}

	public:
		MultiShape(sdl::DRenderer* r) : m_rend(r) {}

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

		constexpr auto& renderer(sdl::DRenderer* const r) noexcept
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
		DRenderer* m_rend = nullptr;
		std::tuple<std::vector<Shapes>...> m_packs;
	};

}
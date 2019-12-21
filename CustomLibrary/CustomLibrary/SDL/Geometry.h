#pragma once

#include <SDL.h>

#include <CustomLibrary/utility.h>
#include "BasicTypes.h"
#include "Renderer.h"

#include <vector>
#include <type_traits>

namespace ctl::sdl
{
	template<typename Shape, template<typename> class... Ex>
	class Frame : public Ex<Frame<Shape, Ex...>>...
	{
	public:
		using shape_t = Shape;

		Frame() = default;

		Frame(const Frame&) = default;
		Frame(Frame&&) = default;

		Frame& operator=(const Frame&) = default;
		Frame& operator=(Frame&&) = default;

		template<template<typename> class... T>
		Frame(T<Frame>&&... arg)
			: T<Frame>(std::move(arg))...
		{
		}

		template<template<typename> class... T>
		Frame(const Frame<Shape, T...>& cast)
			: T<Frame>(static_cast<T<Frame>>(*reinterpret_cast<T<Frame>*>(static_cast<T<Frame<Shape, T...>>*>(&cast))))...
			, m_rend(cast.m_rend)
			, m_shape(cast.m_shape)
		{
		}

		template<template<typename> class... T>
		Frame(Frame<Shape, T...>&& cast)
			: T<Frame>(static_cast<T<Frame>&&>(*reinterpret_cast<T<Frame>*>(static_cast<T<Frame<Shape, T...>>*>(&cast))))...
			, m_rend(cast.m_rend)
			, m_shape(cast.m_shape)
		{
		}

		Frame(Renderer* rend)
			: m_rend(rend)
		{
		}

		Frame(Renderer* rend, const Shape& s)
			: m_rend(rend)
			, m_shape(s)
		{
		}
		
		constexpr const auto& shape() const noexcept
		{
			return m_shape;
		}
		constexpr auto& shape(const shape_t& s) noexcept
		{
			m_shape = s;
			return *this;
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

		FORWARDING_MEMBER_FUNCTIONS(shape_t, m_shape, pos)
		FORWARDING_MEMBER_FUNCTIONS(shape_t, m_shape, translate)

	protected:
		Renderer* m_rend = nullptr;
		shape_t m_shape;
	};


	//----------------------------------------------
	//Single
	//----------------------------------------------

	template<template<typename> class... Ex>
	using RectFrame = Frame<Rect<int, int>, Ex...>;

	template<template<typename> class... Ex>
	using CircleFrame = Frame<Circle<int, unsigned int>, Ex...>;

	template<template<typename> class... Ex>
	using LineFrame = Frame<Line<int>, Ex...>;

	template<template<typename> class... Ex>
	using PointFrame = Frame<Point<int>, Ex...>;


	namespace detail
	{
		template<typename, typename>
		class _Drawable_ {};

		template<typename Impl>
		class _Drawable_<Impl, Tags::isRect>
		{
			const Impl* const pthis = static_cast<const Impl*>(this);

		public:
			void draw() const
			{
				if (SDL_RenderDrawRect(pthis->renderer()->get(), pthis->shape().rect_ptr()) != 0)
					throw err::Log(SDL_GetError());
			}

			void draw_filled() const
			{
				if (SDL_RenderFillRect(pthis->renderer()->get(), pthis->shape().rect_ptr()) != 0)
					throw err::Log(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, Tags::isCircle>
		{
			const Impl* const pthis = static_cast<const Impl*>(this);

		public:
			void draw() const
			{
				_draw_(SDL_RenderDrawPoints);
			}

			void drawFilled() const
			{
				_draw_(SDL_RenderDrawLines);
			}

			void drawP(int pres) const
			{
				std::vector<SDL_Point> ps;
				ps.reserve(pres + 1);

				for (size_t i = 0; i < pres; ++i)
				{
					const auto x = to_radians(360.f / pres * (i + 1.f));
					ps[i] = { static_cast<int>(pthis->shape().r * std::cos(x) + pthis->shape().x),
						static_cast<int>(pthis->shape().r * std::sin(x) + pthis->shape().y) };
				}
				ps.back() = ps.front();

				if (SDL_RenderDrawLines(pthis->renderer()->get(), ps.data(), ps.size()) != 0)
					throw err::Log(SDL_GetError());
			}

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int)) const
			{
				const auto d = pthis->shape().r * 2;

				Point<int> p(pthis->shape().r - 1, 0);
				Point<int> tp(1, 1);

				int err = tp.x - d;

				while (p.x >= p.y)
				{
					const std::array<SDL_Point, 8> ps =
					{ SDL_Point
						{ pthis->shape().x + p.x, pthis->shape().y + p.y },
						{ pthis->shape().x - p.x, pthis->shape().y + p.y },
						{ pthis->shape().x + p.x, pthis->shape().y - p.y },
						{ pthis->shape().x - p.x, pthis->shape().y - p.y },
						{ pthis->shape().x + p.y, pthis->shape().y + p.x },
						{ pthis->shape().x - p.y, pthis->shape().y + p.x },
						{ pthis->shape().x + p.y, pthis->shape().y - p.x },
						{ pthis->shape().x - p.y, pthis->shape().y - p.x }
					};

					if (func(pthis->renderer()->get(), ps.data(), ps.size()) != 0)
						throw err::Log(SDL_GetError());

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
		};

		template<typename Impl>
		class _Drawable_<Impl, Tags::isLine>
		{
			const Impl* const pthis = static_cast<const Impl*>(this);

		public:
			void draw() const
			{
				if (SDL_RenderDrawLine(pthis->renderer()->get(), pthis->shape().x1, pthis->shape().y1, pthis->shape().x2, pthis->shape().y2) != 0)
					throw err::Log(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, Tags::isPoint>
		{
			const Impl* const pthis = static_cast<const Impl*>(this);

		public:
			void draw() const
			{
				if (SDL_RenderDrawPoint(pthis->renderer()->get(), pthis->shape().x, pthis->shape().y) != 0)
					throw err::Log(SDL_GetError());
			}
		};

		template<template<typename, template<typename> class...> class T, typename Shape, template<typename> class... Arg>
		constexpr auto _extracted_tag_(T<Shape, Arg...>&&) -> typename Shape::tag;
	}


	template<typename Impl>
	using EDrawable = detail::_Drawable_<Impl, decltype(detail::_extracted_tag_(std::declval<Impl>()))>;


	//----------------------------------------------
	//Multi
	//----------------------------------------------

	template<typename... Shapes>
	class MultiShape
	{
		static_assert(std::conjunction_v<hasSDLTag<Shapes>...>, "Shapes must have the tag.");

		template<typename T>
		void draw_handler(T& arg) const
		{
			using tag_t = typename T::value_type::tag;

			if constexpr (std::is_same_v<Tags::isRect, tag_t>)
				SDL_RenderDrawRects(m_rend->get(), arg.front().rect_ptr(), arg.size());

			else if constexpr (std::is_same_v<Tags::isLine, tag_t>)
				SDL_RenderDrawLines(m_rend->get(), arg.front().point_ptr(), arg.size() << 1);

			else if constexpr (std::is_same_v<Tags::isPoint, tag_t>)
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
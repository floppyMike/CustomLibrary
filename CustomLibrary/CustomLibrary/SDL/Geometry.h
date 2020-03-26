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


	//----------------------------------------------
	//Single
	//----------------------------------------------

	template<template<typename, typename...> class... Ex>
	using RectFrame = Frame<mth::Rect<int, int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using CircleFrame = Frame<mth::Circle<int, unsigned int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using LineFrame = Frame<mth::Line<int>, Ex...>;

	template<template<typename, typename...> class... Ex>
	using PointFrame = Frame<mth::Point<int>, Ex...>;

	namespace
	{
		template<typename, typename>
		class _Drawable_ {};

		template<typename Impl>
		class _Drawable_<Impl, tag::isRect> : public crtp<Impl, _Drawable_, tag::isRect>
		{
		public:
			void draw_rect() const
			{
				const Impl* const cpthis = this->underlying();

				if (SDL_RenderDrawRect(cpthis->renderer()->get(), cpthis->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());
			}

			void draw_filled_rect() const
			{
				const Impl* const cpthis = this->underlying();

				if (SDL_RenderFillRect(cpthis->renderer()->get(), cpthis->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, tag::isCircle> : public crtp<Impl, _Drawable_, tag::isCircle>
		{
		public:
			void draw_circle() const
			{
				_draw_(SDL_RenderDrawPoints);
			}

			void draw_filled_circle() const
			{
				_draw_(SDL_RenderDrawLines);
			}

			void draw_p(int pres) const
			{
				const Impl* const pthis = this->underlying();

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
					throw std::runtime_error(SDL_GetError());
			}

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int)) const
			{
				const Impl* const pthis = this->underlying();

				const auto d = pthis->shape().r * 2;

				mth::Point<int> p(pthis->shape().r - 1, 0);
				mth::Point<int> tp(1, 1);

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
						throw std::runtime_error(SDL_GetError());

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
		class _Drawable_<Impl, tag::isLine> : public crtp<Impl, _Drawable_, tag::isLine>
		{
		public:
			void draw_line() const
			{
				const Impl* const pthis = this->underlying();

				if (SDL_RenderDrawLine(pthis->renderer()->get(), pthis->shape().x1, pthis->shape().y1, pthis->shape().x2, pthis->shape().y2) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, tag::isPoint> : public crtp<Impl, _Drawable_, tag::isPoint>
		{
		public:
			void draw_point() const
			{
				const Impl* const pthis = this->underlying();

				if (SDL_RenderDrawPoint(pthis->renderer()->get(), pthis->shape().x, pthis->shape().y) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, tag::isTexture> : public crtp<Impl, _Drawable_, tag::isTexture>
		{
		public:
			void draw_texture(const SDL_Rect* blit = nullptr) const
			{
				const Impl* const cpthis = this->underlying();

				if (SDL_RenderCopy(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr()) < 0)
					throw std::runtime_error(SDL_GetError());
			}

			void draw_texture(double angle, const mth::Point<int>& center, SDL_RendererFlip flip, const SDL_Rect* blit = nullptr) const
			{
				const Impl* const cpthis = this->underlying();

				if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
					throw std::runtime_error(SDL_GetError());
			}

			auto& color_mod(Uint8 r, Uint8 g, Uint8 b)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureColorMod(pthis->texture(), r, g, b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
			auto color_mod() const
			{
				const Impl* const cpthis = this->underlying();

				std::tuple<Uint8, Uint8, Uint8> c;

				if (SDL_GetTextureColorMod(cpthis->texture(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
					throw std::runtime_error(SDL_GetError());

				return c;
			}

			auto& blend_mode(const SDL_BlendMode& b)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureBlendMode(pthis->texture(), b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
			SDL_BlendMode blend_mode() const
			{
				const Impl* const cpthis = this->underlying();

				SDL_BlendMode b;

				if (SDL_GetTextureBlendMode(cpthis->texture(), &b) != 0)
					throw std::runtime_error(SDL_GetError());

				return b;
			}

			auto& alpha_mod(const Uint8& a)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureAlphaMod(pthis->texture(), a) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
			Uint8 alpha_mod() const
			{
				const Impl* const cpthis = this->underlying();

				Uint8 a;

				if (SDL_GetTextureAlphaMod(cpthis->texture(), &a) == -1)
					throw std::runtime_error(SDL_GetError());

				return a;
			}
		};
	}


	//template<typename Impl, typename... Tag>
	//using EDrawable = _Drawable_<Impl, 
	//	std::conditional_t<std::disjunction_v<std::is_same<tag::isRect, Tag>...>, tag::isRect,
	//	std::conditional_t<std::disjunction_v<std::is_same<tag::isPoint, Tag>...>, tag::isPoint,
	//	std::conditional_t<std::disjunction_v<std::is_same<tag::isLine, Tag>...>, tag::isLine,
	//	std::conditional_t<std::disjunction_v<std::is_same<tag::isCircle, Tag>...>, tag::isCircle,
	//	void>>>>>;

	template<typename Impl, typename... Tag>
	struct EDrawable : _Drawable_<Impl, Tag>...
	{
		using _Drawable_<Impl, Tag>::_Drawable_...;
	};


	//----------------------------------------------
	//Multi
	//----------------------------------------------

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
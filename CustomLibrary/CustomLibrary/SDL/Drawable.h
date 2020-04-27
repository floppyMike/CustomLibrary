#pragma once

#include "../Tags.h"
#include "../Traits.h"
#include "../BasicTypes.h"

#include "TypeTraits.h"

#include <SDL.h>

#include <tuple>
#include <stdexcept>
#include <vector>
#include <cmath>

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _Drawable_
		{
		};

#ifdef _CTL_SDL2_TEXTURE_
		template<typename Impl>
		class _Drawable_<Impl, tag::isTexture>
			: public crtp<Impl, _Drawable_, tag::isTexture>
		{
		public:
			auto& draw_texture(const SDL_Rect* blit = nullptr)
			{
				auto* cpthis = this->underlying()->pthis();

				if (SDL_RenderCopy(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr()) < 0)
					throw std::runtime_error(SDL_GetError());

				return *this->underlying();
			}

			auto& draw_texture(double angle, const mth::Point<int>& center, SDL_RendererFlip flip, const SDL_Rect* blit = nullptr)
			{
				Impl* const cpthis = this->underlying()->pthis();

				if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
					throw std::runtime_error(SDL_GetError());

				return *cpthis;
			}

			auto& color_mod(Uint8 r, Uint8 g, Uint8 b)
			{
				Impl* const pthis = this->underlying()->pthis();

				if (SDL_SetTextureColorMod(pthis->texture(), r, g, b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			auto color_mod()
			{
				Impl* const cpthis = this->underlying()->pthis();

				std::tuple<Uint8, Uint8, Uint8> c;

				if (SDL_GetTextureColorMod(cpthis->texture(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
					throw std::runtime_error(SDL_GetError());

				return c;
			}

			auto& blend_mode(const SDL_BlendMode& b)
			{
				Impl* const pthis = this->underlying()->pthis();

				if (SDL_SetTextureBlendMode(pthis->texture(), b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			SDL_BlendMode blend_mode()
			{
				Impl* const cpthis = this->underlying()->pthis();

				SDL_BlendMode b;

				if (SDL_GetTextureBlendMode(cpthis->texture(), &b) != 0)
					throw std::runtime_error(SDL_GetError());

				return b;
			}

			auto& alpha_mod(const Uint8& a)
			{
				Impl* const pthis = this->underlying()->pthis();

				if (SDL_SetTextureAlphaMod(pthis->texture(), a) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			Uint8 alpha_mod()
			{
				Impl* const cpthis = this->underlying()->pthis();

				Uint8 a;

				if (SDL_GetTextureAlphaMod(cpthis->texture(), &a) == -1)
					throw std::runtime_error(SDL_GetError());

				return a;
			}
		};
#endif // _CTL_SDL2_TEXTURE_


		template<typename Impl>
		class _Drawable_<Impl, ctl::tag::isRect>
			: public crtp<Impl, _Drawable_, ctl::tag::isRect>
		{
		public:
			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->underlying()->pthis()->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			auto& draw_rect()
			{
				if (SDL_RenderDrawRect(this->underlying()->pthis()->renderer()->get(), this->underlying()->pthis()->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}

			auto& draw_filled_rect()
			{
				if (SDL_RenderFillRect(this->underlying()->pthis()->renderer()->get(), this->underlying()->pthis()->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, ctl::tag::isCircle>
			: public crtp<Impl, _Drawable_, ctl::tag::isCircle>
		{
		public:
			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->underlying()->pthis()->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			auto& draw_circle()
			{
				_draw_(SDL_RenderDrawPoints);
				return *this;
			}

			auto& draw_filled_circle()
			{
				_draw_(SDL_RenderDrawLines);
				return *this;
			}

			auto& draw_p(int pres)
			{
				auto* pthis = this->underlying()->pthis();

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

				return *this;
			}

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int))
			{
				auto* pthis = this->underlying()->pthis();

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
		class _Drawable_<Impl, ctl::tag::isLine>
			: public crtp<Impl, _Drawable_, ctl::tag::isLine>
		{
		public:
			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->underlying()->pthis()->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			void draw_line()
			{
				if (SDL_RenderDrawLine(this->underlying()->pthis()->renderer()->get(), this->underlying()->pthis()->shape().x1, this->underlying()->pthis()->shape().y1, this->underlying()->pthis()->shape().x2, this->underlying()->pthis()->shape().y2) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, ctl::tag::isPoint>
			: public crtp<Impl, _Drawable_, ctl::tag::isPoint>
		{
		public:
			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->underlying()->pthis()->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			void draw_point()
			{
				if (SDL_RenderDrawPoint(this->underlying()->pthis()->renderer()->get(), this->underlying()->pthis()->shape().x, this->underlying()->pthis()->shape().y) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};
	}


	template<typename T>
	class Draw
		: public detail::_UnPeeler_<detail::_Drawable_, Draw<T>, T>
	{
	public:
		Draw() = default;
		Draw(T* o)
			: m_o(o)
		{
		}

		auto* pthis() noexcept { return m_o; }
		auto& pthis(T* o) noexcept { m_o = o; return *this; }

	private:
		T* m_o;
	};
}
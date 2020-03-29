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
		struct _EDrawable_
		{
		};


#ifdef _CTL_SDL2_TEXTURE_
		template<typename Impl>
		class _EDrawable_<Impl, tag::isTexture>
		{
			const Impl* const underlying() const noexcept { return static_cast<const Impl*>(this); }
			Impl* const underlying() noexcept { return static_cast<Impl*>(this); }

		public:
			_EDrawable_() = default;

			auto& draw_texture(const SDL_Rect* blit = nullptr)
			{
				auto& cpthis = this->underlying()->ref();

				if (SDL_RenderCopy(cpthis.renderer()->get(), cpthis.texture(), blit, cpthis.shape().rect_ptr()) < 0)
					throw std::runtime_error(SDL_GetError());

				return *this->underlying();
			}

			auto& draw_texture(double angle, const mth::Point<int>& center, SDL_RendererFlip flip, const SDL_Rect* blit = nullptr)
			{
				Impl* const cpthis = this->underlying()->ref();

				if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->texture(), blit, cpthis->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
					throw std::runtime_error(SDL_GetError());

				return *cpthis;
			}

			auto& color_mod(Uint8 r, Uint8 g, Uint8 b)
			{
				Impl* const pthis = this->underlying()->ref();

				if (SDL_SetTextureColorMod(pthis->texture(), r, g, b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			auto color_mod()
			{
				Impl* const cpthis = this->underlying()->ref();

				std::tuple<Uint8, Uint8, Uint8> c;

				if (SDL_GetTextureColorMod(cpthis->texture(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
					throw std::runtime_error(SDL_GetError());

				return c;
			}

			auto& blend_mode(const SDL_BlendMode& b)
			{
				Impl* const pthis = this->underlying()->ref();

				if (SDL_SetTextureBlendMode(pthis->texture(), b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			SDL_BlendMode blend_mode()
			{
				Impl* const cpthis = this->underlying()->ref();

				SDL_BlendMode b;

				if (SDL_GetTextureBlendMode(cpthis->texture(), &b) != 0)
					throw std::runtime_error(SDL_GetError());

				return b;
			}

			auto& alpha_mod(const Uint8& a)
			{
				Impl* const pthis = this->underlying()->ref();

				if (SDL_SetTextureAlphaMod(pthis->texture(), a) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			Uint8 alpha_mod()
			{
				Impl* const cpthis = this->underlying()->ref();

				Uint8 a;

				if (SDL_GetTextureAlphaMod(cpthis->texture(), &a) == -1)
					throw std::runtime_error(SDL_GetError());

				return a;
			}
		};
#endif // _CTL_SDL2_TEXTURE_



		template<typename Real, typename Tag>
		class _Drawable_
			: public _Func_<Real, _Drawable_<Real, Tag>>
			, public _UnPeeler_<_EDrawable_, _Drawable_<Real, Tag>, Real>
		{
		public:
			using _Func_<Real, _Drawable_>::_Func_;
		};

		template<typename Real>
		class _Drawable_<Real, ctl::tag::isRect> 
			: public _Func_<Real, _Drawable_<Real, ctl::tag::isRect>>
			, public _UnPeeler_<_EDrawable_, _Drawable_<Real, ctl::tag::isRect>, Real>
		{
		public:
			using _Func_<Real, _Drawable_>::_Func_;

			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->m_o->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			auto& draw_rect()
			{
				if (SDL_RenderDrawRect(this->m_o->renderer()->get(), this->m_o->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}

			auto& draw_filled_rect()
			{
				if (SDL_RenderFillRect(this->m_o->renderer()->get(), this->m_o->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
		};

		template<typename Real>
		class _Drawable_<Real, ctl::tag::isCircle> 
			: public _Func_<Real, _Drawable_<Real, ctl::tag::isCircle>>
			, public _UnPeeler_<_EDrawable_, _Drawable_<Real, ctl::tag::isCircle>, Real>
		{
		public:
			using _Func_<Real, _Drawable_>::_Func_;

			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->m_o->renderer()->get(), col.r, col.g, col.b, col.a);
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
				std::vector<SDL_Point> ps;
				ps.reserve(pres + 1);

				for (size_t i = 0; i < pres; ++i)
				{
					const auto x = to_radians(360.f / pres * (i + 1.f));
					ps[i] = { static_cast<int>(this->m_o->shape().r * std::cos(x) + this->m_o->shape().x),
						static_cast<int>(this->m_o->shape().r * std::sin(x) + this->m_o->shape().y) };
				}
				ps.back() = ps.front();

				if (SDL_RenderDrawLines(this->m_o->renderer()->get(), ps.data(), ps.size()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int))
			{
				const auto d = this->m_o->shape().r * 2;

				mth::Point<int> p(this->m_o->shape().r - 1, 0);
				mth::Point<int> tp(1, 1);

				int err = tp.x - d;

				while (p.x >= p.y)
				{
					const std::array<SDL_Point, 8> ps =
					{ SDL_Point
						{ this->m_o->shape().x + p.x, this->m_o->shape().y + p.y },
						{ this->m_o->shape().x - p.x, this->m_o->shape().y + p.y },
						{ this->m_o->shape().x + p.x, this->m_o->shape().y - p.y },
						{ this->m_o->shape().x - p.x, this->m_o->shape().y - p.y },
						{ this->m_o->shape().x + p.y, this->m_o->shape().y + p.x },
						{ this->m_o->shape().x - p.y, this->m_o->shape().y + p.x },
						{ this->m_o->shape().x + p.y, this->m_o->shape().y - p.x },
						{ this->m_o->shape().x - p.y, this->m_o->shape().y - p.x }
					};

					if (func(this->m_o->renderer()->get(), ps.data(), ps.size()) != 0)
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

		template<typename Real>
		class _Drawable_<Real, ctl::tag::isLine> 
			: public _Func_<Real, _Drawable_<Real, ctl::tag::isLine>>
			, public _UnPeeler_<_EDrawable_, _Drawable_<Real, ctl::tag::isLine>, Real>
		{
		public:
			using _Func_<Real, _Drawable_>::_Func_;

			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->m_o->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			void draw_line()
			{
				if (SDL_RenderDrawLine(this->m_o->renderer()->get(), this->m_o->shape().x1, this->m_o->shape().y1, this->m_o->shape().x2, this->m_o->shape().y2) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Real>
		class _Drawable_<Real, ctl::tag::isPoint> 
			: public _Func_<Real, _Drawable_<Real, ctl::tag::isPoint>>
			, public _UnPeeler_<_EDrawable_, _Drawable_<Real, ctl::tag::isPoint>, Real>
		{
		public:
			using _Func_<Real, _Drawable_>::_Func_;

			auto& color(const SDL_Color& col)
			{
				SDL_SetRenderDrawColor(this->m_o->renderer()->get(), col.r, col.g, col.b, col.a);
				return *this;
			}

			void draw_point()
			{
				if (SDL_RenderDrawPoint(this->m_o->renderer()->get(), this->m_o->shape().x, this->m_o->shape().y) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};
	}


	template<typename T>
	using Draw = detail::_Drawable_<T, typename T::base_t::tag_t>;
}
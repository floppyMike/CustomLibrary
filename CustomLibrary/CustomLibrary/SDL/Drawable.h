#pragma once

#include "../Tags.h"
#include "../Traits.h"
#include "../BasicTypes.h"

#include <SDL.h>

#include <tuple>
#include <stdexcept>

namespace ctl::sdl
{
	namespace detail
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
	struct EDrawable : detail::_Drawable_<Impl, Tag>...
	{
		using detail::_Drawable_<Impl, Tag>::_Drawable_...;
	};
}
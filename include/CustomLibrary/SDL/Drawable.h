#pragma once

#include <SDL.h>
#include "../Tags.h"
#include "../Traits.h"
#include "../BasicTypes.h"
#include "../utility.h"

#include "TypeTraits.h"
#include "Renderer.h"


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
			auto& texture(const SDL_Rect* const blit = nullptr)
			{
				auto* cpthis = this->underlying();

				if (SDL_RenderCopy(cpthis->renderer()->get(), cpthis->obj()->texture(), blit, cpthis->obj()->shape().rect_ptr()) < 0)
					throw std::runtime_error(SDL_GetError());

				return *this->underlying();
			}

			auto& texture(double angle, const mth::Point<int>& center, SDL_RendererFlip flip, const SDL_Rect* const blit = nullptr)
			{
				Impl* const cpthis = this->underlying();

				if (SDL_RenderCopyEx(cpthis->renderer()->get(), cpthis->obj()->texture(), blit, cpthis->obj()->shape().rect_ptr(), angle, center.point_ptr(), flip) < 0)
					throw std::runtime_error(SDL_GetError());

				return *cpthis;
			}

			auto& color_mod(Uint8 r, Uint8 g, Uint8 b)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureColorMod(pthis->obj()->texture(), r, g, b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			auto color_mod()
			{
				std::tuple<Uint8, Uint8, Uint8> c;

				if (SDL_GetTextureColorMod(this->underlying()->obj()->texture(), &std::get<0>(c), &std::get<1>(c), &std::get<2>(c)) != 0)
					throw std::runtime_error(SDL_GetError());

				return c;
			}

			auto& blend_mode(const SDL_BlendMode& b)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureBlendMode(pthis->obj()->texture(), b) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			SDL_BlendMode blend_mode()
			{
				SDL_BlendMode b;

				if (SDL_GetTextureBlendMode(this->underlying()->obj()->texture(), &b) != 0)
					throw std::runtime_error(SDL_GetError());

				return b;
			}

			auto& alpha_mod(const Uint8& a)
			{
				Impl* const pthis = this->underlying();

				if (SDL_SetTextureAlphaMod(pthis->obj()->texture(), a) != 0)
					throw std::runtime_error(SDL_GetError());

				return *pthis;
			}
			Uint8 alpha_mod()
			{
				Uint8 a;

				if (SDL_GetTextureAlphaMod(this->underlying()->obj()->texture(), &a) == -1)
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
			auto& rect()
			{
				if (SDL_RenderDrawRect(this->underlying()->renderer()->get(), this->underlying()->obj()->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}

			auto& filled_rect()
			{
				if (SDL_RenderFillRect(this->underlying()->renderer()->get(), this->underlying()->obj()->shape().rect_ptr()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, ctl::tag::isCircle>
			: public crtp<Impl, _Drawable_, ctl::tag::isCircle>
		{
		public:
			auto& circle()
			{
				_draw_(SDL_RenderDrawPoints);
				return *this;
			}

			auto& filled_circle()
			{
				_draw_(SDL_RenderDrawLines);
				return *this;
			}

			auto& draw_p(int pres)
			{
				auto* pthis = this->underlying();

				std::vector<SDL_Point> ps;
				ps.reserve(pres + 1);

				for (size_t i = 0; i < pres; ++i)
				{
					const auto x = to_radians(360.f / pres * (i + 1.f));
					ps[i] = { static_cast<int>(pthis->obj()->shape().r * std::cos(x) + pthis->obj()->shape().x),
						static_cast<int>(pthis->obj()->shape().r * std::sin(x) + pthis->obj()->shape().y) };
				}
				ps.back() = ps.front();

				if (SDL_RenderDrawLines(pthis->renderer()->get(), ps.data(), ps.size()) != 0)
					throw std::runtime_error(SDL_GetError());

				return *this;
			}

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int))
			{
				auto* pthis = this->underlying();

				const auto d = pthis->obj()->shape().r * 2;

				mth::Point<int> p(pthis->obj()->shape().r - 1, 0);
				mth::Point<int> tp(1, 1);

				int err = tp.x - d;

				while (p.x >= p.y)
				{
					const std::array<SDL_Point, 8> ps =
					{ SDL_Point
						{ pthis->obj()->shape().x + p.x, pthis->obj()->shape().y + p.y },
						{ pthis->obj()->shape().x - p.x, pthis->obj()->shape().y + p.y },
						{ pthis->obj()->shape().x + p.x, pthis->obj()->shape().y - p.y },
						{ pthis->obj()->shape().x - p.x, pthis->obj()->shape().y - p.y },
						{ pthis->obj()->shape().x + p.y, pthis->obj()->shape().y + p.x },
						{ pthis->obj()->shape().x - p.y, pthis->obj()->shape().y + p.x },
						{ pthis->obj()->shape().x + p.y, pthis->obj()->shape().y - p.x },
						{ pthis->obj()->shape().x - p.y, pthis->obj()->shape().y - p.x }
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
			void line()
			{
				auto* pthis = this->underlying();

				if (SDL_RenderDrawLine(pthis->renderer()->get(), pthis->obj()->shape().x1, pthis->obj()->shape().y1, pthis->obj()->shape().x2, pthis->obj()->shape().y2) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, ctl::tag::isPoint>
			: public crtp<Impl, _Drawable_, ctl::tag::isPoint>
		{
		public:
			void point()
			{
				auto* pthis = this->underlying();

				if (SDL_RenderDrawPoint(pthis->renderer()->get(), pthis->obj()->shape().x, pthis->obj()->shape().y) != 0)
					throw std::runtime_error(SDL_GetError());
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, tag::isMultiShape>
			: public crtp<Impl, _Drawable_, tag::isMultiShape>
		{
		public:
			auto& all() const
			{
				auto* pthis = this->underlying();

				std::apply([this](auto&&... arg)
					{
						(this->_draw_handler_(arg), ...);
					}, pthis->obj()->tuple_data());

				return *pthis;
			}

		private:
			template<typename T>
			void _draw_handler_(T& arg) const
			{
				using tag_t = typename T::value_type::tag;
				auto* pthis = this->underlying();

				if constexpr (std::is_same_v<ctl::tag::isRect, tag_t>)
					SDL_RenderDrawRects(pthis->renderer()->get(), arg.front().rect_ptr(), arg.size());

				else if constexpr (std::is_same_v<ctl::tag::isLine, tag_t>)
					SDL_RenderDrawLines(pthis->renderer()->get(), arg.front().point_ptr(), arg.size() << 1);

				else if constexpr (std::is_same_v<ctl::tag::isPoint, tag_t>)
					SDL_RenderDrawLines(pthis->renderer()->get(), arg.front().point_ptr(), arg.size());

				else
					assert(false && "Type is not supported for mass drawing.");
				//static_assert(false, "Type is not supported for mass drawing.");
			}
		};

		template<typename Impl>
		class _Drawable_<Impl, tag::isAnimation>
			: public crtp<Impl, _Drawable_, tag::isAnimation>
		{
		public:
			void animated()
			{
				Impl* const cpthis = this->underlying();
				cpthis->texture(cpthis->obj()->blit_ani().rect_ptr());
			}

			void animated(double angle, const mth::Point<int>& center, SDL_RendererFlip flip)
			{
				const Impl* const cpthis = this->underlying();
				cpthis->texture(angle, center, flip, &cpthis->obj()->blit_ani());
			}
		};

	}

	template<typename T>
	using Draw = FunctionalR<T, detail::_Drawable_>;
}

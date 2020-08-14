#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../BasicTypes.h"
#include "../Error.h"
#include "../Traits.h"
#include "../utility.h"

#include "Renderer.h"
#include "TypeTraits.h"

#include <span>

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Predefinitions
	// -----------------------------------------------------------------------------

	class Texture;
	class Animation;

	template<typename T>
	class Frame;

	// -----------------------------------------------------------------------------
	// Drawing implementations
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename, typename, typename>
		class _Drawable_;

		/**
		 * @brief Handles drawing of textures
		 */
		template<typename Full, uses_sdl_renderer Impl>
		class _Drawable_<Texture, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Simple draw operation
			 * Draws the texture onto the render buffer
			 * @param blit Specifies a part of the texture to draw. Default is whole texture.
			 */
			auto texture(const mth::Rect<int, int> *const blit = nullptr) const -> void
			{
				const auto r = SDL_RenderCopy(this->renderer()->get(), this->obj()->texture(),
											  reinterpret_cast<const SDL_Rect *>(blit),
											  reinterpret_cast<const SDL_Rect *>(&this->obj()->shape()));
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Advanced draw operation
			 * Draws the texture onto the render buffer
			 * @param angle Rotation angle
			 * @param center Rotation center
			 * @param flip Flip texture https://wiki.libsdl.org/SDL_RendererFlip#Values
			 * @param blit Specifies a part of the texture to draw. Default is whole texture.
			 */
			auto texture(double angle, const mth::Point<int> &center, SDL_RendererFlip flip,
						 const mth::Rect<int, int> *const blit = nullptr) const -> void
			{
				const auto r = SDL_RenderCopyEx(this->renderer()->get(), this->obj()->texture(),
												reinterpret_cast<const SDL_Rect *>(blit),
												reinterpret_cast<const SDL_Rect *>(this->obj()->shape()), angle,
												reinterpret_cast<const SDL_Point *>(&center), flip);
				ASSERT(r == 0, SDL_GetError());
			}
		};

		/**
		 * @brief Handles drawing of Rect Objects
		 */
		template<matches<SDL_Rect, mth::Rect<int, int>, Frame<mth::Rect<int, int>>> Type, typename Full,
				 uses_sdl_renderer													Impl>
		class _Drawable_<Type, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws rect onto render buffer
			 */
			auto rect() const -> void
			{
				const auto r = SDL_RenderDrawRect(this->renderer()->get(), _ptr_(&this->obj()));
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Draws filled rect onto render buffer
			 */
			auto filled_rect() const -> void
			{
				const auto r = SDL_RenderFillRect(this->renderer()->get(), _ptr_(&this->obj()));
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_()
			{
				using T = strip_t<decltype(this->obj())>;

				if constexpr (std::is_same_v<T, SDL_Rect> || std::is_same_v<T, mth::Rect<int, int>>)
					return this->obj();
				else
					return &this->obj()->shape();
			}
		};

		/**
		 * @brief Handles drawing of Circle Objects
		 */
		template<matches<mth::Circle<int, int>, Frame<mth::Circle<int, int>>> Type, typename Full,
				 uses_sdl_renderer											  Impl>
		class _Drawable_<Type, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws the generated circle
			 */
			auto circle() const -> void
			{
				const auto r = SDL_RenderDrawPoints(this->renderer()->get(), m_cache.data(), m_cache.size());
				ASSERT(r == 0, SDL_GetError());
			}
			/**
			 * @brief Draws the generated circle filled
			 */
			auto filled_circle() const -> void
			{
				const auto r = SDL_RenderDrawLines(this->renderer()->get(), m_cache.data(), m_cache.size());
				ASSERT(r == 0, SDL_GetError());
			}
			/**
			 * @brief Draws the generated presicion circle
			 */
			auto circle_p() const -> void { filled_circle(); }

			/**
			 * @brief Generate circle and store it into cache
			 * @return this
			 */
			auto generate() -> auto &
			{
				const auto d = _ptr_()->r * 2;

				mth::Point<int> p(_ptr_()->r - 1, 0);
				mth::Point<int> tp(1, 1);

				int err = tp.x - d;

				while (p.x >= p.y)
				{
					m_cache.insert(m_cache.end(),
								   { SDL_Point{ _ptr_()->x + p.x, _ptr_()->y + p.y },
									 { _ptr_()->x - p.x, _ptr_()->y + p.y },
									 { _ptr_()->x + p.x, _ptr_()->y - p.y },
									 { _ptr_()->x - p.x, _ptr_()->y - p.y },
									 { _ptr_()->x + p.y, _ptr_()->y + p.x },
									 { _ptr_()->x - p.y, _ptr_()->y + p.x },
									 { _ptr_()->x + p.y, _ptr_()->y - p.x },
									 { _ptr_()->x - p.y, _ptr_()->y - p.x } });

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

				return *this;
			}

			/**
			 * @brief Generate circle using a certain precision
			 *
			 * @param pres The number of edges the circle will have
			 * @return this
			 */
			auto generate_p(int pres) -> auto &
			{
				m_cache.reserve(pres + 1);

				for (size_t i = 0; i < pres; ++i)
				{
					const auto x = deg_to_rad(360.f / pres * (i + 1.F));
					m_cache[i]	 = { static_cast<int>(_ptr_()->r * std::cos(x) + _ptr_()->x),
									 static_cast<int>(_ptr_()->r * std::sin(x) + _ptr_()->y) };
				}
				m_cache.back() = m_cache.front();

				return *this;
			}

		private:
			std::vector<SDL_Point> m_cache;

			auto _ptr_() const noexcept
			{
				using T = strip_t<decltype(this->obj())>;

				if constexpr (std::is_same_v<T, mth::Circle<int, int>>)
					return this->obj();
				else
					return &this->obj()->shape();
			}
		};

		/**
		 * @brief Handles drawing of Line Objects
		 */
		template<matches<mth::Line<int>, Frame<mth::Line<int>>> Type, typename Full, uses_sdl_renderer Impl>
		class _Drawable_<Type, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws the line
			 */
			void line() const
			{
				const auto r =
					SDL_RenderDrawLine(this->renderer()->get(), _ptr_()->x1, _ptr_()->y1, _ptr_()->x2, _ptr_()->y2);
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_() const noexcept
			{
				using T = strip_t<decltype(this->obj())>;

				if constexpr (std::is_same_v<T, mth::Line<int>>)
					return this->obj();
				else
					return &this->obj()->shape();
			}
		};

		/**
		 * @brief Handles drawing of Point Objects
		 */
		template<matches<SDL_Point, mth::Point<int>, Frame<mth::Point<int>>> Type, typename Full,
				 uses_sdl_renderer											 Impl>
		class _Drawable_<Type, Full, Impl> : public Impl
		{
		public:
			/**
			 * @brief Draws the point
			 */
			void point() const
			{
				const auto r = SDL_RenderDrawPoint(this->renderer()->get(), _ptr_()->x, _ptr_()->y);
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_() const noexcept
			{
				using T = strip_t<decltype(this->obj())>;

				if constexpr (std::is_same_v<T, SDL_Point> || std::is_same_v<T, mth::Point<int>>)
					return this->obj();
				else
					return &this->obj()->shape();
			}
		};

		/**
		 * @brief Handles mass drawing of Rectangles
		 * This method of drawing is fast then drawing singular Rects. Array is a std::span of SDL_Rect or Rect<int,
		 * int>.
		 */
		template<matches<SDL_Rect, mth::Rect<int, int>> Type, size_t Size, typename Full, uses_sdl_renderer Impl>
		class _Drawable_<std::span<Type, Size>, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws rects
			 */
			auto rects() const -> void
			{
				const auto r =
					SDL_RenderDrawRects(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Draws filled rects
			 */
			auto filled_rects() const -> void
			{
				const auto r =
					SDL_RenderFillRects(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_(const SDL_Rect *ptr) const { return ptr; }
			auto _ptr_(const mth::Rect<int, int> *ptr) const { return reinterpret_cast<const SDL_Rect *>(ptr); }
		};

		/**
		 * @brief Handles mass drawing of Rectangles of type float
		 * This method of drawing is fast then drawing singular Rects. Array is a std::span of SDL_FRect or Rect<float,
		 * float>.
		 */
		template<matches<SDL_FRect, mth::Rect<float, float>> Type, size_t Size, typename Full, uses_sdl_renderer Impl>
		class _Drawable_<std::span<Type, Size>, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws rects
			 */
			auto rects() const -> void
			{
				const auto r =
					SDL_RenderDrawRectsF(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Draws filled rects
			 */
			auto filled_rects() const -> void
			{
				const auto r =
					SDL_RenderFillRectsF(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_(const SDL_FRect *ptr) const { return ptr; }
			auto _ptr_(const mth::Rect<float, float> *ptr) const { return reinterpret_cast<const SDL_FRect *>(ptr); }
		};

		/**
		 * @brief Handles mass drawing of Points
		 * This method of drawing is faster then drawing singular Points. Array is a std::span of SDL_Point or
		 * Point<int>.
		 */
		template<matches<SDL_Point, mth::Point<int>> Type, size_t Size, typename Full, uses_sdl_renderer Impl>
		class _Drawable_<std::span<Type, Size>, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws Points
			 */
			auto points() const -> void
			{
				const auto r =
					SDL_RenderDrawPoints(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Draws connected Lines
			 */
			auto lines() const -> void
			{
				const auto r =
					SDL_RenderDrawLines(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_(const SDL_Point *ptr) const { return ptr; }
			auto _ptr_(const mth::Point<int> *ptr) const { return reinterpret_cast<const SDL_Point *>(ptr); }
		};

		/**
		 * @brief Handles mass drawing of Points for floats
		 * This method of drawing is faster then drawing singular Points. Array is a std::span of SDL_Point or
		 * Point<float>.
		 */
		template<matches<SDL_FPoint, mth::Point<float>> Type, size_t Size, typename Full, uses_sdl_renderer Impl>
		class _Drawable_<std::span<Type, Size>, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws Points
			 */
			auto points() const -> void
			{
				const auto r =
					SDL_RenderDrawPointsF(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

			/**
			 * @brief Draws connected Lines
			 */
			auto lines() const -> void
			{
				const auto r =
					SDL_RenderDrawLinesF(this->renderer()->get(), _ptr_(this->obj()->data()), this->obj()->size());
				ASSERT(r == 0, SDL_GetError());
			}

		private:
			auto _ptr_(const SDL_FPoint *ptr) const { return ptr; }
			auto _ptr_(const mth::Point<float> *ptr) const { return reinterpret_cast<const SDL_FPoint *>(ptr); }
		};

		/**
		 * @brief Handles drawing of animations
		 */
		template<typename Full, uses_sdl_renderer Impl>
		class _Drawable_<Animation, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Draws current animation frame
			 */
			void animated() const { this->texture(&this->obj()->blit_ani()); }

			/**
			 * @brief Draws current animation frame advanced
			 *
			 * @param angle Rotation angle
			 * @param center Center of rotation
			 * @param flip Texture flip https://wiki.libsdl.org/SDL_RendererFlip#Values
			 */
			void animated(double angle, const mth::Point<int> &center, SDL_RendererFlip flip) const
			{
				this->texture(angle, center, flip, &this->obj()->blit_ani());
			}
		};

	}; // namespace detail

	// -----------------------------------------------------------------------------
	// Drawing Extension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Type for drawing type construction
	 * @tparam T Object to draw for type
	 */
	template<typename T, typename Renderer>
	using Draw = typename Filter<detail::_Drawable_, FunctorR<T, Renderer>, T>::type;

	/**
	 * @brief Shows drawing options for object
	 *
	 * @param ptr ptr to object
	 * @param r ptr to renderer
	 * @return Draw type for further options
	 */
	template<typename _T, typename _Renderer>
	auto draw(_T *const ptr, _Renderer *const r)
	{
		return Draw<_T, _Renderer>(ptr, r);
	}

} // namespace ctl::sdl

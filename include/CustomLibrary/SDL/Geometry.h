#if not defined _CTL_SDL2_GEO_
#define _CTL_SDL2_GEO_

#include "../BasicTypes.h"
#include "../Error.h"
#include "TypeTraits.h"
#include "Renderer.h"

#include <span>

namespace ctl::sdl
{
	/**
	 * @brief Frame class encompassing all the shapes
	 * @tparam Shape Shape type to use
	 */
	template<typename Shape>
	class Frame
	{
	public:
		constexpr Frame() = default;

		constexpr Frame(const Frame &)	   = default;
		constexpr Frame(Frame &&) noexcept = default;

		constexpr auto operator=(const Frame &) -> Frame & = default;
		constexpr auto operator=(Frame &&) noexcept -> Frame & = default;

		/**
		 * @brief Constructs Frame using the shape s
		 * @param s Shape to use
		 */
		constexpr explicit Frame(const Shape &s)
			: m_shape(s)
		{
		}

		/**
		 * @brief Gives the constant shape
		 * @return Shape
		 */
		constexpr auto shape() const noexcept -> const auto & { return m_shape; }
		/**
		 * @brief Give the shape
		 * @return Shape
		 */
		constexpr auto shape() noexcept -> auto & { return m_shape; }
		/**
		 * @brief Sets the shape
		 * @param s Shape to use
		 */
		constexpr auto shape(const Shape &s) noexcept -> void { m_shape = s; }

	private:
		Shape m_shape;
	};

	/**
	 * @brief Area in shape of a rectangle
	 */
	using RectFrame = Frame<mth::Rect<int, int>>;
	/**
	 * @brief Area in shape of a circle
	 */
	using CircleFrame = Frame<mth::Circle<int, int>>;
	/**
	 * @brief Area in shape of a line
	 */
	using LineFrame = Frame<mth::Line<int>>;
	/**
	 * @brief Area in shape of a point
	 */
	using PointFrame = Frame<mth::Point<int>>;

	/**
	 * @brief Area in shape of a rectangle
	 */
	using RectFrameF = Frame<mth::Rect<float, float>>;
	/**
	 * @brief Area in shape of a circle
	 */
	using CircleFrameF = Frame<mth::Circle<float, float>>;
	/**
	 * @brief Area in shape of a line
	 */
	using LineFrameF = Frame<mth::Line<float>>;
	/**
	 * @brief Area in shape of a point
	 */
	using PointFrameF = Frame<mth::Point<float>>;

	// -----------------------------------------------------------------------------
	// Concepts
	// -----------------------------------------------------------------------------

	template<typename T>
	concept is_rect_shape = matches<T, SDL_Rect, mth::Rect<int, int>, RectFrame>;

	template<typename T>
	concept is_circle_shape = matches<T, mth::Circle<int, int>, CircleFrame>;

	template<typename T>
	concept is_point_shape = matches<T, SDL_Point, mth::Point<int>, PointFrame>;

	template<typename T>
	concept is_line_shape = matches<T, mth::Line<int>, LineFrame>;

	template<typename T>
	concept is_rectf_shape = matches<T, SDL_FRect, mth::Rect<float, float>, RectFrameF>;

	template<typename T>
	concept is_circlef_shape = matches<T, mth::Circle<float, float>, CircleFrameF>;

	template<typename T>
	concept is_pointf_shape = matches<T, SDL_Point, mth::Point<float>, PointFrameF>;

	template<typename T>
	concept is_linef_shape = matches<T, mth::Line<float>, LineFrameF>;

	// -----------------------------------------------------------------------------
	// Procedures
	// -----------------------------------------------------------------------------

	// clang-format off
	auto frame_shape_diff(const mth::Rect<int, int> *o) noexcept -> const SDL_Rect *
	{
		return reinterpret_cast<const SDL_Rect *>(o);
	}
	auto frame_shape_diff(const mth::Rect<float, float> *o) noexcept -> const SDL_FRect *
	{
		return reinterpret_cast<const SDL_FRect *>(o);
	}
	auto frame_shape_diff(const mth::Point<int> *o) noexcept -> const SDL_Point *
	{
		return reinterpret_cast<const SDL_Point *>(o);
	}
	auto frame_shape_diff(const mth::Point<float> *o) noexcept -> const SDL_FPoint *
	{
		return reinterpret_cast<const SDL_FPoint *>(o);
	}
	auto frame_shape_diff(const mth::Circle<int, int> *o) noexcept -> const mth::Circle<int, int> * { return o; }
	auto frame_shape_diff(const mth::Circle<float, float> *o) noexcept -> const mth::Circle<float, float> * { return o; }
	auto frame_shape_diff(const mth::Line<int> *o) noexcept -> const mth::Line<int> * { return o; }
	auto frame_shape_diff(const mth::Line<float> *o) noexcept -> const mth::Line<float> * { return o; }
	// clang-format on

	template<typename O>
	auto frame_shape_diff(const Frame<O> *o) noexcept -> const auto *
	{
		return frame_shape_diff(&o->shape());
	}

	// --------------------------------- Actual Draws -----------------------------------------

	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rect_shape T>
	void draw(sdl::Renderer &r, const T &rect)
	{
		const auto x = SDL_RenderDrawRect(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rectf_shape T>
	void draw(sdl::Renderer &r, const T &rect)
	{
		const auto x = SDL_RenderDrawRectF(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_rect_shape T>
	void draw(sdl::Renderer &r, std::span<T> rect)
	{
		const auto x = SDL_RenderDrawRects(r.get(), frame_shape_diff(rect.data()), rect.size());
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_rectf_shape T>
	void draw(sdl::Renderer &r, std::span<T> rect)
	{
		const auto x = SDL_RenderDrawRectsF(r.get(), frame_shape_diff(rect.data()), rect.size());
		ASSERT(x == 0, SDL_GetError());
	}

	/**
	 * @brief Draws filled shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rect_shape T>
	void draw_filled(sdl::Renderer &r, const T &rect)
	{
		const auto x = SDL_RenderFillRect(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws filled shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rectf_shape T>
	void draw_filled(sdl::Renderer &r, const T &rect)
	{
		const auto x = SDL_RenderFillRectF(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws filled shapes onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rect_shape T>
	void draw_filled(sdl::Renderer &r, std::span<T> rect)
	{
		const auto x = SDL_RenderFillRects(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws filled shapes onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_rectf_shape T>
	void draw_filled(sdl::Renderer &r, std::span<T> rect)
	{
		const auto x = SDL_RenderFillRectsF(r.get(), frame_shape_diff(&rect));
		ASSERT(x == 0, SDL_GetError());
	}

	// ------------------------------------------------



	// ------------------------------------------------

	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_point_shape T>
	void draw(sdl::Renderer &r, const T &p)
	{
		const auto *o = frame_shape_diff(&p);
		const auto	x = SDL_RenderDrawPoint(r.get(), o->x, o->y);
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_pointf_shape T>
	void draw(sdl::Renderer &r, const T &p)
	{
		const auto *o = frame_shape_diff(&p);
		const auto	x = SDL_RenderDrawPointF(r.get(), o->x, o->y);
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_point_shape T>
	void draw(sdl::Renderer &r, std::span<T> p)
	{
		const auto x = SDL_RenderDrawPoints(r.get(), frame_shape_diff(p.data()), p.size());
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_pointf_shape T>
	void draw(sdl::Renderer &r, std::span<T> p)
	{
		const auto x = SDL_RenderDrawPointsF(r.get(), frame_shape_diff(p.data()), p.size());
		ASSERT(x == 0, SDL_GetError());
	}

	// ------------------------------------------------

	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_line_shape T>
	void draw(sdl::Renderer &r, const T &l)
	{
		const auto *o = frame_shape_diff(&l);
		const auto	x = SDL_RenderDrawLine(r.get(), o->x1, o->y1, o->x2, o->y2);
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shape onto render buffer
	 * @param r renderer
	 * @param rect shape
	 */
	template<is_linef_shape T>
	void draw(sdl::Renderer &r, const T &l)
	{
		const auto *o = frame_shape_diff(&l);
		const auto	x = SDL_RenderDrawLineF(r.get(), l->x1, l->y1, l->x2, l->y2);
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_point_shape T>
	void draw_connected(sdl::Renderer &r, std::span<T> l)
	{
		const auto x = SDL_RenderDrawLines(r.get(), frame_shape_diff(l.data()), l.size());
		ASSERT(x == 0, SDL_GetError());
	}
	/**
	 * @brief Draws shapes onto render buffer
	 * @param r renderer
	 * @param rect shapes
	 */
	template<is_pointf_shape T>
	void draw_connected(sdl::Renderer &r, std::span<T> p)
	{
		const auto x = SDL_RenderDrawLinesF(r.get(), frame_shape_diff(p.data()), p.size());
		ASSERT(x == 0, SDL_GetError());
	}

} // namespace ctl::sdl

#endif
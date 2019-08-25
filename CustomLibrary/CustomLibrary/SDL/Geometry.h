#pragma once

#include <vector>
#include <type_traits>

#include <SDL.h>

#include <CustomLibrary/utility.h>
#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/SDLTraits.h>

namespace ctl
{
	class Rectangle
	{
	public:
		using Graphical::Graphical;

		void draw()
		{

		}
	};

	void draw(const SDL_Color& rgba, SDLWindow* win, const SDLRect<int, int>& ass)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);


	}

	enum class Fig { LINE, SQUARE, SQUARE_FILLED, POINT, CIRCLE, CIRCLE_FILLED, P_CIRCLE };

	template<Fig shape, typename Type>
	void draw(const SDL_Color&, SDLWindow*, const Type&)
	{
		static_assert(true, "draw: Wrong parameters.");
	}

	template<>
	void draw<Fig::SQUARE_FILLED>(const SDL_Color& rgba, SDLWindow* win, const SDLRect &xywh)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		if (SDL_RenderFillRect(win->renderer(), &unmove(SDLRect(win->cam.worldToScreen({ xywh.x, xywh.y }), { xywh.w, xywh.h }))) != 0)
			throw Log(SDL_GetError(), Log::Sev::ERR0R);
	}
	template<>
	void draw<Fig::SQUARE>(const SDL_Color& rgba, SDLWindow* win, const SDLRect& xywh)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		if (SDL_RenderDrawRect(win->renderer(), &unmove(SDLRect(win->cam.worldToScreen({ xywh.x, xywh.y }), { xywh.w, xywh.h }))) != 0)
			throw Log(SDL_GetError(), Log::Sev::ERR0R);
	}
	template<>
	void draw<Fig::POINT>(const SDL_Color& rgba, SDLWindow* win, const SDLPoint& xy)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		if (SDL_RenderDrawPoint(win->renderer(), xy.x - win->cam.loc().x, xy.y - win->cam.loc().y) != 0)
			throw Log(SDL_GetError(), Log::Sev::ERR0R);
	}
	template<>
	void draw<Fig::CIRCLE>(const SDL_Color& rgba, SDLWindow* win, const SDLCircle& xyr)
	{
		SDL_assert(SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a) == 0);

		SDLPoint p(0, xyr.r);
		int d = 3 - 2 * xyr.r;

		auto drawCircle = [&xyr, &p, &d, &win]
		{
			const SDLPoint ps[] =
			{
				win->cam.worldToScreen({ xyr.x + p.x, xyr.y + p.y }),
				win->cam.worldToScreen({ xyr.x - p.x, xyr.y + p.y }),
				win->cam.worldToScreen({ xyr.x + p.x, xyr.y - p.y }),
				win->cam.worldToScreen({ xyr.x - p.x, xyr.y - p.y }),
				win->cam.worldToScreen({ xyr.x + p.y, xyr.y + p.x }),
				win->cam.worldToScreen({ xyr.x - p.y, xyr.y + p.x }),
				win->cam.worldToScreen({ xyr.x + p.y, xyr.y - p.x }),
				win->cam.worldToScreen({ xyr.x - p.y, xyr.y - p.x }),
			};

			SDL_assert(SDL_RenderDrawPoints(win->renderer(), ps, 8) == 0);
		};

		drawCircle();

		while (p.y >= p.x++)
		{
			if (d > 0)
				d = d + 4 * (p.x - --p.y) + 10;
			else
				d = d + 4 * p.x + 6;

			drawCircle();
		}
	}
	template<>
	void draw<Fig::CIRCLE_FILLED>(const SDL_Color& rgba, SDLWindow* win, const SDLCircle& xyr)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		SDLPoint p(0, xyr.r);
		int d = 3 - 2 * xyr.r;

		auto drawCircle = [&xyr, &p, &d, &win]
		{
			const SDLPoint ps[] =
			{
				win->cam.worldToScreen({ xyr.x + p.x, xyr.y + p.y }),
				win->cam.worldToScreen({ xyr.x - p.x, xyr.y + p.y }),
				win->cam.worldToScreen({ xyr.x + p.x, xyr.y - p.y }),
				win->cam.worldToScreen({ xyr.x - p.x, xyr.y - p.y }),
				win->cam.worldToScreen({ xyr.x + p.y, xyr.y + p.x }),
				win->cam.worldToScreen({ xyr.x - p.y, xyr.y + p.x }),
				win->cam.worldToScreen({ xyr.x + p.y, xyr.y - p.x }),
				win->cam.worldToScreen({ xyr.x - p.y, xyr.y - p.x }),
			};

			SDL_RenderDrawLines(win->renderer(), ps, 8);
		};

		drawCircle();

		while (p.y >= p.x++)
		{
			if (d > 0)
				d = d + 4 * (p.x - --p.y) + 10;
			else
				d = d + 4 * p.x + 6;

			drawCircle();
		}
	}

	template<Fig shape, typename Type1, typename Type2>
	void draw(const SDL_Color&, SDLWindow*, const Type1&, const Type2&)
	{
		static_assert(false, "draw: Wrong parameters.");
	}

	template<>
	void draw<Fig::P_CIRCLE>(const SDL_Color& rgba, SDLWindow* win, const SDLCircle& xyr, const size_t& pres)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		const auto point = win->cam.worldToScreen({ xyr.x, xyr.y });

		std::vector<SDLPoint> points;
		points.reserve(pres + 1);

		for (size_t i = 0; i < pres; ++i)
		{
			const auto& x = to_radians(360. / pres * (i + 1.));
			points.push_back({ static_cast<int>(xyr.r * std::cos(x) + point.x), static_cast<int>(xyr.r * std::sin(x) + point.y) });
		}
		points.push_back(points.front());

		SDL_RenderDrawLines(win->renderer(), points.data(), pres + 1);
	}
	template<>
	void draw<Fig::LINE>(const SDL_Color& rgba, SDLWindow* win, const SDLPoint& p1, const SDLPoint& p2)
	{
		SDL_SetRenderDrawColor(win->renderer(), rgba.r, rgba.g, rgba.b, rgba.a);

		const auto point1 = win->cam.worldToScreen(p1);
		const auto point2 = win->cam.worldToScreen(p2);

		if (SDL_RenderDrawLine(win->renderer(), point1.x, point1.y, point2.x, point2.y) != 0)
			throw Log(SDL_GetError(), Log::Sev::ERR0R);
	}

	//---------------------------------------------------------------------
	//------------------------------Multi----------------------------------
	//---------------------------------------------------------------------

	struct Line {};
	struct Square {};
	struct SquareFilled {};
	struct Point {};

	template<typename Type>
	class FastGeo
	{
		using Evaluated =
			std::conditional_t<std::is_same_v<Line, Type>, SDLPoint,
			std::conditional_t<std::is_same_v<Square, Type>, SDLRect,
			std::conditional_t<std::is_same_v<SquareFilled, Type>, SDLRect,
			std::conditional_t<std::is_same_v<Point, Type>, SDLPoint, void>>>>;

	public:
		FastGeo(ctl::SDLWindow* win)
			: m_win(win)
		{
		}

		template<typename ...Arg>
		auto& emplace_back(Arg&& ... argv) 
		{ 
			m_pool.emplace_back(std::move(argv)...); 
			return *this; 
		}

		auto& push_back(const Evaluated& val) 
		{ 
			m_pool.push_back(val); 
			return *this; 
		}
		auto& push_back(Evaluated&& val) 
		{ 
			m_pool.push_back(std::move(val));
			return *this; 
		}

		auto& clear() noexcept 
		{ 
			m_pool.clear(); 
			return *this; 
		}

		auto& reserve(const size_t& newSize) 
		{ 
			m_pool.reserve(newSize); 
			return *this; 
		}

		template<typename Iter, typename = typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iter>::value_type, Evaluated>>>
		auto& insert_back(const Iter& begin, const Iter& end) 
		{ 
			m_pool.insert(m_pool.end(), begin, end);
			return *this; 
		}

		void draw(const SDL_Color& col)
		{
			std::vector<Evaluated> temp(m_pool.begin(), m_pool.end());
			for (auto& i : temp)
			{
				i.x -= m_win->cam.loc().x;
				i.y -= m_win->cam.loc().y;
			}

			SDL_SetRenderDrawColor(m_win->renderer(), col.r, col.g, col.b, col.a);

			if constexpr (std::is_same_v<Type, Line>)
				SDL_RenderDrawLines(m_win->renderer(), temp.data(), temp.size());

			else if constexpr (std::is_same_v<Type, Square>)
				SDL_RenderDrawRects(m_win->renderer(), temp.data(), temp.size());

			else if constexpr (std::is_same_v<Type, SquareFilled>)
				SDL_RenderFillRects(m_win->renderer(), temp.data(), temp.size());

			else if constexpr (std::is_same_v<Type, Point>)
				SDL_RenderDrawPoints(m_win->renderer(), temp.data(), temp.size());
		}

		constexpr const auto& pool() const noexcept { return m_pool; }

	private:
		SDLWindow* m_win;
		std::vector<Evaluated> m_pool;
	};
}
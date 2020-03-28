#pragma once

#include "../Error.h"
#include "../Traits.h"
#include "../Dim.h"
#include <SDL.h>

#include "Engine.h"

#include <cassert>

namespace ctl::sdl
{
	class Renderer
	{
		struct Unique_Des { void operator()(SDL_Renderer* r) { SDL_DestroyRenderer(r); } };

	public:
		template<typename ImplWin>
		Renderer(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			create(win, rendererFlags);
		}

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&& r) noexcept = default;

		template<typename ImplWin>
		void create(ImplWin* win, Uint32 rendererFlags = SDL_RENDERER_ACCELERATED)
		{
			if (auto* r = SDL_CreateRenderer(win->get(), -1, rendererFlags); r)
				m_renderer.reset(r);
			else
				throw std::runtime_error(SDL_GetError());
		}

		void destroy()
		{
			m_renderer.reset();
		}

		constexpr auto* get() const noexcept
		{
			assert(m_renderer && "Renderer isn't loaded.");
			return m_renderer.get(); 
		}

		void render()
		{
			SDL_RenderPresent(m_renderer.get());
		}

	private:
		std::unique_ptr<SDL_Renderer, Unique_Des> m_renderer;
	};

	template<typename T>
	class LDelayedRender : public T
	{
	public:
		using T::T;

		void do_render(bool r)
		{
			m_do_render = r;
		}

		bool will_render() const noexcept
		{
			return m_do_render;
		}

		void render()
		{
			T::render();
			m_do_render = false;
		}

	private:
		bool m_do_render = true;
	};

	template<typename T>
	class LColorer : public T
	{
	public:
		using T::T;

		void color(const SDL_Color& col)
		{
			SDL_SetRenderDrawColor(this->get(), col.r, col.g, col.b, col.a);
		}

		void fill()
		{
			color(m_fill_col);
			SDL_RenderClear(this->get());
		}

		void fill_color(const SDL_Color& col) noexcept { m_fill_col = col; }
		const auto& fill_color() const noexcept { return m_fill_col; }

	private:
		SDL_Color m_fill_col = sdl::WHITE;
	};


	//template<typename Impl, typename... tags_t>
	//class ERendererSettings : public crtp<Impl, ERendererSettings, tags_t...>
	//{
	//	static_assert(tag::has_tag_v<tag::isRenderer, tags_t...>, "Parent must be a renderer.");

	//public:
	//	void logical_size(const mth::Dim<int>& dim)
	//	{
	//		Impl* const pthis = this->underlying();

	//		assert(pthis->get() != nullptr && "Renderer isn't loaded.");
	//		SDL_RenderSetLogicalSize(pthis->get(), dim.w, dim.h);
	//	}

	//	void color(const SDL_Color& col)
	//	{
	//		Impl* const pthis = this->underlying();

	//		assert(pthis->get() != nullptr && "Renderer isn't loaded.");
	//		SDL_SetRenderDrawColor(pthis->get(), col.r, col.g, col.b, col.a);
	//	}

	//	void fill()
	//	{
	//		Impl* const pthis = this->underlying();

	//		assert(pthis->get() != nullptr && "Renderer isn't loaded.");
	//		color(m_fill_col);
	//		SDL_RenderClear(pthis->get());
	//	}

	//	void fill_color(const SDL_Color& col) noexcept { m_fill_col = col; }
	//	const auto& fill_color() const noexcept { return m_fill_col; }

	//private:
	//	SDL_Color m_fill_col = sdl::WHITE;
	//};

	//template<typename Impl, typename... tags_t>
	//class ERendererRender : public crtp<Impl, ERendererRender, tags_t...>
	//{
	//	static_assert(tag::has_tag_v<tag::isRenderer, tags_t...>, "Parent must be a renderer.");

	//public:
	//	void do_render(bool r)
	//	{
	//		m_do_render = r;
	//	}

	//	bool will_render() const noexcept
	//	{
	//		return m_do_render;
	//	}

	//	void render()
	//	{
	//		Impl* const pthis = this->underlying();

	//		assert(pthis->get() != nullptr && "Renderer isn't loaded.");
	//		SDL_RenderPresent(pthis->get());
	//		m_do_render = false;
	//	}

	//private:
	//	bool m_do_render = true;
	//};

}
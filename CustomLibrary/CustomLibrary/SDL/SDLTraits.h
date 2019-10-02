#pragma once

#include "Renderer.h"

namespace ctl::sdl
{
	template<typename Derived>
	class Drawable
	{
	protected:
		constexpr Drawable() noexcept = default;

		constexpr Drawable(const Drawable&) noexcept = default;
		constexpr Drawable& operator=(const Drawable&) noexcept = default;

		/**
		* @summary constructs from window or position or both
		* @param "win" target SDLWindow
		* @param "pos" starting position
		*/
		Drawable(sdl::Renderer* const r) noexcept
			: m_rend(r)
		{
		}

		/**
		* @summary sets or returns window ptr
		* @param "win" new window to set
		* @remarks after setting a new window you must recreate the resource
		* @returns window ptr
		*/
		constexpr auto& renderer(sdl::Renderer* const r) noexcept
		{
			assert(r != nullptr && "Renderer passed is a nullptr.");
			m_rend = r;
			return static_cast<Derived&>(*this);
		}

		constexpr auto& renderer() const noexcept
		{
			assert(m_rend != nullptr && "Renderer isn't assigned.");
			return m_rend;
		}

		sdl::Renderer* m_rend = nullptr;
	};


	template<typename Shape, typename Derived>
	class Shapeable
	{
	protected:
		constexpr const auto& shape() const noexcept
		{
			return m_shape;
		}

		constexpr auto& shape(const Shape& s) noexcept
		{
			m_shape = s;
			return *static_cast<Derived*>(this);
		}

		Shape m_shape;
	};

}
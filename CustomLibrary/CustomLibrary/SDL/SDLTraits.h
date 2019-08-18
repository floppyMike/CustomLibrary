#pragma once

#include "BasicTypes.h"
#include "SDLWindow.h"

namespace ctl
{
	template<typename Derived>
	class Graphical
	{
	public:
		constexpr Graphical() noexcept = default;

		constexpr Graphical(const Graphical&) noexcept = default;
		constexpr Graphical& operator=(const Graphical&) noexcept = default;

		/**
		* @summary constructs from window or position or both
		* @param "win" target SDLWindow
		* @param "pos" starting position
		*/
		constexpr Graphical(SDLWindow* const win) noexcept
			: m_win(win)
		{
		}
		constexpr Graphical(const SDLRect<float, Uint32>& pos) noexcept
			: m_pos(pos)
		{
		}
		constexpr Graphical(SDLWindow* const win, const SDLRect<float, Uint32>& pos) noexcept
			: Graphical(win)
			, Graphical(pos)
		{
		}

		/**
		* @summary sets or returns window ptr
		* @param "win" new window to set
		* @remarks after setting a new window you must recreate the resource
		* @returns window ptr
		*/
		constexpr auto* win() { return m_win; }
		constexpr Derived& win(SDLWindow* win) 
		{
			m_win = win;
			return static_cast<Derived&>(*this);
		}

		/**
		* @summary sets and converts world position to screen position or accesses position
		* @param "p" position
		* @returns position
		*/
		constexpr const auto& pos() const { return m_pos.point(); }
		constexpr Derived& pos(const SDLPoint<short>& p)
		{
			m_pos.point() = m_win->cam.worldToScreen(p);
			return static_cast<Derived&>(*this);
		}

		constexpr const auto& dim() const { return m_pos.dim(); }

	protected:
		SDLWindow* m_win = nullptr;
		SDLRect<short, int> m_pos;
	};
}
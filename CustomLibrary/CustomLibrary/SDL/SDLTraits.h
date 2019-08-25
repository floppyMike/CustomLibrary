#pragma once

#include "BasicTypes.h"
#include "SDLWindow.h"

namespace ctl
{
	class Renderable
	{
	public:
		constexpr Renderable() noexcept = default;

		constexpr Renderable(const Renderable&) noexcept = default;
		constexpr Renderable& operator=(const Renderable&) noexcept = default;

		/**
		* @summary constructs from window or position or both
		* @param "win" target SDLWindow
		* @param "pos" starting position
		*/
		Renderable(SDLWindow* const win) noexcept
			: m_win(win)
		{
		}

		/**
		* @summary sets or returns window ptr
		* @param "win" new window to set
		* @remarks after setting a new window you must recreate the resource
		* @returns window ptr
		*/
		constexpr auto* win() { return m_win; }
		constexpr Renderable& win(SDLWindow* win)
		{
			m_win = win;
			return *this;
		}

	protected:
		SDLWindow* m_win = nullptr;
	};

	template<typename Shape>
	class Object : public Renderable
	{
	public:
		using Renderable::Renderable;

		constexpr Object() noexcept = default;

		constexpr Object(const Object&) noexcept = default;
		constexpr Object& operator=(const Object&) noexcept = default;

		/**
		* @summary sets and converts world position to screen position or accesses position
		* @param "p" position
		* @returns position
		*/
		constexpr const auto& renderShape() const { return m_shape; }
		constexpr auto& renderShape() { return m_shape; }

	protected:
		Shape m_shape;
	};

}
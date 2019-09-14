#pragma once

#include "BasicTypes.h"

namespace ctl
{
	namespace sdl
	{
		class Camera2D
		{
		public:
			/**
			* @summary constructs collider from camera location and screen size ref
			* @param "screen" ref to screen Dim
			*/
			constexpr Camera2D() noexcept = default;

			/**
			* @summary translates screen coord to world coord
			* @param "screen" ref to screen Dim
			* @returns Point of world coord
			*/
			template<typename T>
			constexpr Point<T> screenToWorld(const Point<T>& loc) const noexcept;

			/**
			* @summary translates world coord to screen coord
			* @param "screen" ref to world Dim
			* @returns Point of screen coord
			*/
			template<typename T>
			constexpr Point<T> worldToScreen(Point<T> loc) const noexcept;

			/**
			* @summary moves Point
			* @param "deltaX" x distance
			* @param "deltaY" y distance
			*/
			constexpr auto& mov(int deltaX, int deltaY) noexcept;

			/**
			* @summary location accessors
			*/
			constexpr const auto& loc() const noexcept;

			constexpr auto& loc(const Point<int>& loc) noexcept;

		private:
			Point<int> m_camLoc = { 0, 0 };
		};


		//----------------------------------------------
		//Implementation
		//----------------------------------------------

		template<typename T>
		constexpr Point<T> Camera2D::screenToWorld(const Point<T>& loc) const noexcept
		{
			return loc += m_camLoc;
		}

		template<typename T>
		inline constexpr Point<T> Camera2D::worldToScreen(Point<T> loc) const noexcept
		{
			return loc -= m_camLoc;
		}

		inline constexpr auto& ctl::sdl::Camera2D::mov(int deltaX, int deltaY) noexcept
		{
			m_camLoc.x += deltaX;
			m_camLoc.y += deltaY;

			return *this;
		}

		inline constexpr const auto& Camera2D::loc() const noexcept
		{
			return m_camLoc;
		}

		inline constexpr auto& Camera2D::loc(const Point<int>& loc) noexcept
		{
			m_camLoc = loc;
			return *this;
		}
	}
}

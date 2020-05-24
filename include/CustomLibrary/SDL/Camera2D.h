#pragma once

#include "../BasicTypes.h"

namespace ctl::sdl
{
	class Camera2D
	{
	public:
		/**
		* @summary constructs collider from camera location and screen size ref
		* @param "screen" ref to screen mth::Dim
		*/
		constexpr Camera2D() = default;

		/**
		* @summary translates screen coord to world coord
		* @param "screen" ref to screen mth::Dim
		* @returns mth::Point of world coord
		*/
		template<typename T>
		constexpr mth::Point<T> screen_world(const mth::Point<T>& loc) const noexcept;

		/**
		* @summary translates world coord to screen coord
		* @param "screen" ref to world mth::Dim
		* @returns mth::Point of screen coord
		*/
		template<typename T>
		constexpr mth::Point<T> world_screen(mth::Point<T> loc) const noexcept;

		/**
		* @summary moves mth::Point
		* @param "deltaX" x distance
		* @param "deltaY" y distance
		*/
		constexpr auto& mov(int deltaX, int deltaY) noexcept;

		/**
		* @summary location accessors
		*/
		constexpr const auto& loc() const noexcept;

		constexpr auto& loc(const mth::Point<int>& loc) noexcept;

	private:
		mth::Point<int> m_camLoc = { 0, 0 };
	};


	//----------------------------------------------
	//Implementation
	//----------------------------------------------

	template<typename T>
	constexpr mth::Point<T> Camera2D::screen_world(const mth::Point<T>& loc) const noexcept
	{
		return loc += m_camLoc;
	}

	template<typename T>
	inline constexpr mth::Point<T> Camera2D::world_screen(mth::Point<T> loc) const noexcept
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

	inline constexpr auto& Camera2D::loc(const mth::Point<int>& loc) noexcept
	{
		m_camLoc = loc;
		return *this;
	}

}

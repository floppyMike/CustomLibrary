#pragma once

#include "SDLRenderer.h"

namespace ctl
{
	namespace sdl
	{
		template<typename ImplRend, typename Shape>
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
			Renderable(ImplRend* const r) noexcept
				: m_rend(r)
			{
			}

			/**
			* @summary sets or returns window ptr
			* @param "win" new window to set
			* @remarks after setting a new window you must recreate the resource
			* @returns window ptr
			*/
			constexpr Renderable& renderer(ImplRend* const r)
			{
				m_rend = r;
				return *this;
			}

			constexpr const auto& shape() const noexcept
			{
				return m_shape;
			}

			constexpr auto& shape(const Shape& s) const noexcept
			{
				m_shape = s;
				return *this;
			}

		protected:
			ImplRend* m_rend = nullptr;
			Shape m_shape;
		};

	}
}
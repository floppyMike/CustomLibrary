#pragma once

#include "SDLRenderer.h"

namespace ctl
{
	namespace sdl
	{
		template<typename ImplRend>
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

		protected:
			ImplRend* m_rend = nullptr;
		};


		template<typename ImplRend, typename Shape>
		class Object : public Renderable<ImplRend>
		{
		public:
			using Renderable<ImplRend>::Renderable;

			constexpr const auto& shape() const noexcept
			{
				return m_shape;
			}

			constexpr auto& shape(const Shape& s) noexcept
			{
				m_shape = s;
				return *this;
			}

		protected:
			Shape m_shape;
		};


		template<typename Type>
		class ReliesOn
		{
		public:
			constexpr ReliesOn() noexcept = default;
			constexpr ReliesOn(const ReliesOn& v) noexcept = default;
			constexpr ReliesOn& operator=(const ReliesOn& v) noexcept = default;

			constexpr Type& set(Type* v) noexcept
			{
				return *(m_var = v);
			}

			template<typename T, typename = typename std::enable_if_t<std::is_same_v<Type, T>>>
			constexpr Type* get() const noexcept
			{
				return m_var;
			}

		private:
			Type* m_var = nullptr;
		};

	}
}
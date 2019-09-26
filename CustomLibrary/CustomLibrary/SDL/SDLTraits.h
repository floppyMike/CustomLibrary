#pragma once

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


		template<typename Shape, typename Derived>
		class Shapeable
		{
		public:
			constexpr const auto& shape() const noexcept
			{
				return m_shape;
			}

			constexpr Derived& shape(const Shape& s) noexcept
			{
				m_shape = s;
				return *static_cast<Derived*>(this);
			}

		protected:
			Shape m_shape;
		};


		template<typename Type, typename Derived>
		class ReliesOn
		{
		protected:
			constexpr ReliesOn() noexcept = default;
			constexpr ReliesOn(const ReliesOn& v) noexcept = default;
			constexpr ReliesOn& operator=(const ReliesOn& v) noexcept = default;

			constexpr Derived& set(Type* v) noexcept
			{
				m_var = v;
				return static_cast<Derived&>(*this);
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
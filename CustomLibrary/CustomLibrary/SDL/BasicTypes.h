#pragma once

#include <type_traits>

namespace ctl
{
	template<typename Check>
	using _IsArithmitic_ = std::enable_if_t<std::is_arithmetic_v<Check>>;

	template<typename T>
	class SDLPoint
	{
	public:
		using value_type = T;

		/**
		* @summary all constructors and assignments are default
		*/
		constexpr SDLPoint() = default;
		constexpr SDLPoint(const SDLPoint&) = default;
		constexpr SDLPoint(SDLPoint&&) = default;

		constexpr SDLPoint& operator=(const SDLPoint&) = default;
		constexpr SDLPoint& operator=(SDLPoint&&) = default;

		/**
		* @summary construct from x and y coords
		*/
		constexpr SDLPoint(const T& x, const T& y)
			: x(x), y(y)
		{
		}

		/**
		* @summary handles type overload to SDL_Point
		* @remarks must be of type int
		*/
		operator SDL_Point() const noexcept
		{
			static_assert(true, "Conversion must be of type int");
		}

		/**
		* @summary assignment operators
		*/
		constexpr auto& operator+=(const SDLPoint& p) noexcept;
		constexpr auto& operator-=(const SDLPoint& p) noexcept;

		/**
		* @summary arithmitic operators
		*/
		constexpr auto operator+(const SDLPoint& p) const noexcept;
		constexpr auto operator-(const SDLPoint& p) const noexcept;

		/**
		* @summary relational operators
		*/
		constexpr auto operator==(const SDLPoint& p) const noexcept;

		T x = static_cast<T>(0), y = static_cast<T>(0);
	};

	template<typename T>
	class SDLDim
	{
	public:
		using value_type = T;

		/**
		* @summary all constructors and assignments are default
		*/
		constexpr SDLDim() = default;
		constexpr SDLDim(const SDLDim&) = default;
		constexpr SDLDim(SDLDim&&) = default;

		constexpr SDLDim& operator=(const SDLDim&) = default;
		constexpr SDLDim& operator=(SDLDim&&) = default;

		/**
		* @summary construct from width and height
		*/
		constexpr SDLDim(const T& pw, const T& ph)
			: w(pw), h(ph)
		{
		}

		T w = static_cast<T>(0), h = static_cast<T>(0);
	};

	template<typename Point_T, typename Dim_T>
	class SDLRect
	{
	public:
		using value_type_point = Point_T;
		using value_type_dim = Dim_T;

		/**
		* @summary all constructors and assignments are default
		*/
		constexpr SDLRect() = default;
		constexpr SDLRect(const SDLRect&) = default;
		constexpr SDLRect(SDLRect&&) = default;

		constexpr SDLRect& operator=(const SDLRect&) = default;
		constexpr SDLRect& operator=(SDLRect&&) = default;

		/**
		* @summary handles type overload to SDL_Point
		* @remarks must be of type int
		*/
		operator SDL_Rect() const noexcept
		{
			static_assert(true, "Conversion must be of type int");
		}

		/**
		* @summary construct from x, y, width and height
		*/
		constexpr SDLRect(const Point_T& x, const Point_T& y, const Dim_T& w, const Dim_T& h) noexcept
			: m_point(x, y), m_dim(w, h)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLRect(const SDLPoint<Point_T>& p, const SDLDim<Dim_T>& d) noexcept
			: m_point(p), m_dim(d)
		{
		}

	private: //These need to be in front because of quick SDL_Rect conversion
		SDLPoint<Point_T> m_point;
		SDLDim<Dim_T> m_dim;

	public:
		Point_T& x = m_point.x, y = m_point.y;
		Dim_T& w = m_dim.w, h = m_dim.h;
	};

	template <typename Point_T, typename Rad_T>
	class SDLCircle
	{
	public:
		using value_type_point = Point_T;
		using value_type_radius = Rad_T;

		/**
		* @summary all constructors and assignments are default
		*/
		constexpr SDLCircle() = default;
		constexpr SDLCircle(const SDLCircle&) = default;
		constexpr SDLCircle(SDLCircle&&) = default;

		constexpr SDLCircle& operator=(const SDLCircle&) = default;
		constexpr SDLCircle& operator=(SDLCircle&&) = default;

		/**
		* @summary construct from x, y and radius
		*/
		constexpr SDLCircle(const T& px, const T& py, const T& pr)
			: x(px), y(py), r(pr)
		{
		}

		/**
		* @summary construct from point and radius
		*/
		constexpr SDLCircle(const SDLPoint<T>& p, const T& r)
			: SDLCircle(p.x, p.y, r)
		{
		}

		Point_T& x = m_point.x, y = m_point.y;
		Rad_T r = static_cast<Rad_T>(0);

	private:
		SDLPoint<Point_T> m_point;
	};

	template<>
	inline SDLPoint<int>::operator SDL_Point() const noexcept
	{
		return *reinterpret_cast<const SDL_Point*const>(this);
	}

	template<>
	inline SDLRect<int, int>::operator SDL_Rect() const noexcept
	{
		return *reinterpret_cast<const SDL_Rect* const>(this);
	}

	template<typename T>
	inline constexpr auto& SDLPoint<T>::operator+=(const SDLPoint& p) noexcept
	{
		x += p.x;
		y += p.y;

		return *this;
	}

	template<typename T>
	inline constexpr auto& SDLPoint<T>::operator-=(const SDLPoint& p) noexcept
	{
		x -= p.x;
		y -= p.y;

		return *this;
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator+(const SDLPoint& p) const noexcept
	{
		return SDLPoint(x + p.x, y + p.y);
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator-(const SDLPoint& p) const noexcept
	{
		return SDLPoint(x - p.x, y - p.y);
	}

	template<typename T>
	inline constexpr auto SDLPoint<T>::operator==(const SDLPoint& p) const noexcept
	{
		return x == p.x && y == p.y;
	}

}
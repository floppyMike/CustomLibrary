#pragma once

#include <type_traits>

namespace ctl
{
	template<typename T, 
		typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	class _SDLParam_
	{
	public:
		using value_type = T;

	protected:
		_SDLParam_() = default;
	};


	template<typename T>
	class SDLPoint : public _SDLParam_<T>
	{
	public:
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

		T x = 0, y = 0;
	};

	template<typename T>
	class SDLDim : public _SDLParam_<T>
	{
	public:
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

		T w = 0, h = 0;
	};

	template<typename T>
	class SDLRect : public _SDLParam_<T>
	{
	public:
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
		constexpr SDLRect(const T& x, const T& y, const T& w, const T& h) noexcept
			: x(x), y(y), w(w), h(h)
		{
		}

		/**
		* @summary construct from point and dimension
		*/
		constexpr SDLRect(const SDLPoint<T>& p, const SDLDim<T>& d) noexcept
			: SDLRect(p.x, p.y, d.w, d.h)
		{
		}

		T x = 0, y = 0, w = 0, h = 0;
	};

	template <typename T>
	class SDLCircle : public _SDLParam_<T>
	{
	public:
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

		T x = 0, y = 0, r = 0;
	};

	template<>
	inline SDLPoint<int>::operator SDL_Point() const noexcept
	{
		return *reinterpret_cast<const SDL_Point*const>(this);
	}

	template<>
	inline SDLRect<int>::operator SDL_Rect() const noexcept
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

}
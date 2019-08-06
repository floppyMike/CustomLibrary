#pragma once

#include <CustomLibrary/Timer.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/Sprite.h>

namespace ctl
{
	class AnimationLite
	{
		SDLRect _passFrame_();

	public:
		AnimationLite() = default;
		AnimationLite(const AnimationLite&) = default;
		AnimationLite(AnimationLite&&) = default;

		AnimationLite& operator=(const AnimationLite&) = default;
		AnimationLite& operator=(AnimationLite&&) = default;

		AnimationLite(BaseTexture*, const SDLDim&, const std::chrono::milliseconds&, const NumVec<size_t, 2> &);

		AnimationLite& unpause() noexcept;
		AnimationLite& pause() noexcept;
		constexpr const bool& isPaused() const noexcept;

		template<typename ...T>
		void draw(const T&... para) { m_texture->render(std::forward<const T&>(para)..., &unmove(_passFrame_())); }

		constexpr const auto& range() const { return m_range; }
		constexpr auto& range(const NumVec<size_t, 2> & range) { m_range = range; m_currFrame = range[0]; return *this; }

		constexpr const auto& dim() const noexcept { return m_dim; }

		constexpr auto& textureAndDim(BaseTexture* texture, const SDLDim& dim) noexcept { m_texture = texture; m_xMax = texture->dim().w / dim.w; return *this; }

		constexpr const auto& interval() const noexcept { return m_waitTime; }
		constexpr auto& interval(const std::chrono::milliseconds& i) noexcept { m_waitTime = i; return *this; }

	private:
		BaseTexture* m_texture;

		SDLDim m_dim;
		int m_xMax;

		Timer m_timer;
		std::chrono::milliseconds m_waitTime = std::chrono::milliseconds(0);
		std::chrono::milliseconds m_tillNext = std::chrono::milliseconds(0);

		int m_currFrame;

		NumVec<size_t, 2> m_range = { 0, 0 };
	};

	SDLRect AnimationLite::_passFrame_()
	{
		if (!m_timer.isPaused())
		{
			m_tillNext += m_timer.ticks();

			while (m_tillNext >= m_waitTime)
			{
				m_tillNext -= m_waitTime;
				++m_currFrame;

				if (m_currFrame == m_range[1])
					m_currFrame = m_range[0];
			}

			m_timer.start();
		}

		return SDLRect{ m_currFrame % m_xMax * m_dim.w, m_currFrame / m_xMax * m_dim.h, m_dim.w, m_dim.h };
	}

	AnimationLite::AnimationLite(BaseTexture* texture, const SDLDim& dim, const std::chrono::milliseconds& interval, const NumVec<size_t, 2> & range)
		: m_texture(texture)
		, m_dim(dim)
		, m_xMax(texture->dim().w / dim.w)
		, m_waitTime(interval)
		, m_currFrame(range[0])
		, m_range(range)
	{
	}

	AnimationLite& AnimationLite::unpause() noexcept
	{
		m_timer.unpause(); 
		return *this; 
	}

	AnimationLite& AnimationLite::pause() noexcept
	{
		if (!m_timer.isStarted())
			m_timer.start();

		m_timer.pause();
		return *this;
	}
	
	constexpr const bool& AnimationLite::isPaused() const noexcept
	{ 
		return m_timer.isPaused(); 
	}
}
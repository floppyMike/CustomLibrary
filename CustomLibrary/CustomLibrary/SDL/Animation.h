#pragma once

#include <CustomLibrary/Timer.h>
#include "Texture.h"
#include "BasicTypes.h"

#include <vector>

namespace ctl
{
	namespace sdl
	{
		template<typename ImplTex, typename ImplRend>
		class Animation
		{
		public:
			using Frame = std::pair<Rect<int, int>, std::chrono::milliseconds>;

			Animation()
			{
			}

		private:
			Timer m_time;
			std::chrono::milliseconds m_tillNext = std::chrono::milliseconds(0);

			std::vector<Frame> m_frames;
			std::vector<Frame>::iterator m_currFrame = m_frames.begin();
		};

	}
	class AnimationBase
	{
		virtual Rect* _passFrame_() = 0;

	public:
		AnimationBase() = default;
		AnimationBase(const AnimationBase&) = default;
		AnimationBase(AnimationBase&&) = default;

		AnimationBase& operator=(const AnimationBase&) = default;
		AnimationBase& operator=(AnimationBase&&) = default;

		AnimationBase(BaseTexture* t)
			: m_texture(t)
		{
		}

		template<typename ...T>
		void draw(const T& ... para) { m_texture->render(std::forward<const T&>(para)..., _passFrame_()); }

		void unpause() noexcept;
		void pause() noexcept;
		bool isPaused() noexcept;

	protected:
		BaseTexture* m_texture;

		Timer m_timer;
		std::chrono::milliseconds m_tillNext = std::chrono::milliseconds(0);
	};

	void AnimationBase::unpause() noexcept
	{
		m_timer.unpause();
	}

	void AnimationBase::pause() noexcept
	{
		m_timer.pause();
	}

	bool AnimationBase::isPaused() noexcept
	{
		return m_timer.isPaused();
	}





	class Animation : public AnimationBase
	{
		Rect* _passFrame_();

	public:
		using Frame = std::pair<Rect, std::chrono::milliseconds>;

		using AnimationBase::AnimationBase;

		Animation() = default;
		Animation(const Animation&) = default;
		Animation(Animation&&) = default;

		Animation& operator=(const Animation&) = default;
		Animation& operator=(Animation&&) = default;

		Animation(BaseTexture* texture, const std::initializer_list<Frame>& frames)
			: AnimationBase(texture)
			, m_frames(frames)
		{
		}

		Animation& emplace(const std::vector<Frame>::const_iterator&, const Rect&, const std::chrono::milliseconds&);
		Animation& remove(const std::vector<Frame>::const_iterator&);

		template<typename Iter>
		Animation& insert(std::vector<Frame>::const_iterator loc, const Iter& begin, const Iter& end)
		{
			m_frames.insert(loc, begin, end);
			m_currFrame = m_frames.begin();
			return *this;
		}

		constexpr const auto& frames() const noexcept { return m_frames; }

	private:
		std::vector<Frame> m_frames;
		std::vector<Frame>::iterator m_currFrame = m_frames.begin();
	};
	
	Rect* Animation::_passFrame_()
	{
		if (!m_timer.isPaused() && !m_frames.empty())
		{
			m_tillNext += m_timer.ticks();

			while (m_tillNext >= m_currFrame->second)
			{
				m_tillNext -= m_currFrame->second;
				++m_currFrame;

				if (m_currFrame == m_frames.end())
					m_currFrame = m_frames.begin();
			}

			m_timer.start();
		}

		return &m_currFrame->first;
	}

	Animation& Animation::emplace(const std::vector<Frame>::const_iterator& loc, const Rect& r, const std::chrono::milliseconds& mil)
	{
		m_frames.emplace(loc, r, mil);
		m_currFrame = m_frames.begin();
		return *this;
	}

	Animation& Animation::remove(const std::vector<Frame>::const_iterator& loc)
	{
		m_frames.erase(loc);
		m_currFrame = m_frames.begin();
		return *this;
	}
}
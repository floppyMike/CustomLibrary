#ifndef _CTL_SDL2_ANIMATION_
#define _CTL_SDL2_ANIMATION_

#include <SDL2/SDL.h>

#include "../BasicTypes.h"
#include "../Timer.h"
#include "Drawable.h"
#include "Texture.h"
#include "TypeTraits.h"
#include <vector>

// DEPRECATED

namespace ctl::sdl
{
	struct AniFrame
	{
		mth::Rect<int, int>		  shape;
		std::chrono::milliseconds step;
	};

	class Animation : public Texture
	{
	public:
		using base_t = Texture;
		using tag_t	 = tag::isAnimation;

		Animation() = default;

		void push_frame(AniFrame &&f)
		{
			m_frames.emplace_back(f);
			m_curr_frame = m_frames.begin();
		}
		template<typename Iter>
		void assign_frames(Iter begin_frame, Iter end_frame)
		{
			m_frames.assign(begin_frame, end_frame);
			m_curr_frame = m_frames.begin();
		}

		void start_ani() noexcept { m_time.start(); }
		void pause_ani() noexcept { m_time.pause(); }
		void unpause_ani() noexcept { m_time.unpause(); }
		bool is_ani_paused() noexcept { return m_time.isPaused(); }

		const auto &blit_ani() noexcept
		{
			if (!m_time.isPaused() && !m_frames.empty())
			{
				m_till_next += m_time.ticks();

				while (m_till_next >= m_curr_frame->step)
				{
					m_till_next -= m_curr_frame->step;
					++m_curr_frame;

					if (m_curr_frame == m_frames.end())
						m_curr_frame = m_frames.begin();
				}

				m_time.start();
			}

			return m_curr_frame->shape;
		}

		auto frames_size() const noexcept { return m_frames.size(); }

	private:
		Timer					  m_time;
		std::chrono::milliseconds m_till_next = std::chrono::milliseconds(0);

		std::vector<AniFrame>			m_frames;
		std::vector<AniFrame>::iterator m_curr_frame = m_frames.begin();
	};

	template<template<typename> class... Ex>
	using EAnimation = typename MixBuilder<Animation, Ex...>::type;

} // namespace ctl::sdl
#endif // !_CTL_SDL2_ANIMATION_

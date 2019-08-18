#include "Engine.h"

ctl::SDL::SDL(const size_t& fps, const Uint32& SDLFlags)
	: m_frameTime(1000 / fps)
{
	if (SDL_Init(SDLFlags) < 0)
		throw Log(SDL_GetError());
}

void ctl::SDL::setHint(const char* name, const char* value) noexcept
{
	if (!SDL_SetHint(name, value))
		Log::logWrite(std::string("SDL: setHint: ") + name + " failed with value " + value, Log::Sev::WARNING);
}

void ctl::SDL::run()
{
	Timer timer;
	timer.start();
	std::chrono::duration<double> lastTime(0);
	std::chrono::duration<double> lag(0);

	unsigned long long frames = 0;

	for (bool quit = false; !quit; ++frames)
	{
		const auto time = timer.ticks<std::chrono::duration<double>>();
		const auto elapsed = time - lastTime;
		lastTime = time;
		lag += elapsed;

		if (time >= std::chrono::seconds(1))
			m_fps = frames / time.count();

		const auto endTime = std::chrono::steady_clock::now() + m_frameTime;

		static SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			m_event.notify(e);

			if (e.type == SDL_QUIT)
				quit = true;
		}

		m_delta = elapsed.count();
		for (auto& i : m_windows)
			i->_invoke_(&StateBase::update);

		while (lag >= m_frameTime)
		{
			lag -= m_frameTime;
			for (auto& i : m_windows)
				i->_invoke_(&StateBase::fixedUpdate);
		}

		for (auto& i : m_windows)
			i->_render_();

		std::this_thread::sleep_until(endTime);
	}
}

ctl::SDL& ctl::SDL::detachWin(WindowBase* window)
{
	m_windows.erase(std::find_if(m_windows.begin(), m_windows.end(), 
		[&window](const std::unique_ptr<WindowBase>& w) { return w.get() == window; }));
	m_event.detach(window);

	return *this;
}

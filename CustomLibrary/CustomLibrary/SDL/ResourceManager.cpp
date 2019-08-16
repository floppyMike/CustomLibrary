#include "ResourceManager.h"

ctl::ResManager& ctl::ResManager::erase(const std::string_view& var)
{
	m_res.erase(var);
	return *this;
}

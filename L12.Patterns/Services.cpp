#include "Services.h"


using namespace Learnings;

void Services::Add(std::shared_ptr<Service> service)
{
	std::type_index idx = std::type_index(typeid(*service));
	m_Services[idx] = service;
}


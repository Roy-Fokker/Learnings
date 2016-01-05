#pragma once

#include <map>
#include <typeinfo>
#include <typeindex>
#include <memory>

namespace Learnings
{
	class Service
	{
	public:
		virtual ~Service() {};
	};

	class Services
	{
	public:
		void Add(std::shared_ptr<Service> service);
		
		template <class T>
		std::shared_ptr<T> Get() const
		{
			auto type = std::type_index(typeid(T));
			auto it = m_Services.find(type);

			if (it != m_Services.end())
				return std::static_pointer_cast<T>(it->second);

			return nullptr;
		}

	private:
		std::map<std::type_index, std::shared_ptr<Service>> m_Services;
	};

};
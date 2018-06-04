#include <proc/Proc.h>
#include <data/IDataProcAccess.h>

#include <iostream>

using namespace proc;

void Proc::add(data::IDataProcAccess &elem)
{
	// note: IDataProcAccess is always owned by a shared_ptr
	m_data.push_back(elem.ptr());
}

void Proc::run()
{
	for(const auto &e : m_data)
	{
		std::cout << "{ ";

		for(uint32_t i = 0; i < e->len(); i++)
		{
            std::cout << std::to_string(e->data()[i]) << " ";
		}

        std::cout << "} ";
	}
    std::cout << std::endl;
}

void Proc::clear()
{
    m_data.clear();
}

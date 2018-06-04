#include "Data.h"
#include "DataImpl.h"

using namespace data;

// shared_ptr factory
std::shared_ptr<IDataProcAccess> data::createData()
{

    return std::make_shared<Data>();
}

Data::Data()
	: m_impl(std::make_unique<internal::DataImpl>())
{

}

uint8_t* Data::data() const
{
	return m_impl->data();
}

uint32_t Data::len() const
{
	return m_impl->len();
}

std::shared_ptr<IDataProcAccess> Data::ptr()
{
    return shared_from_this();
}

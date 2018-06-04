#include "DataImpl.h"

using namespace data;
using namespace data::internal;

DataImpl::DataImpl()
{
	m_data[0] = 0x01;
	m_data[1] = 0x02;
	m_data[2] = 0x03;
    m_data[3] = 0x04;
}

uint8_t* DataImpl::data()
{
	return m_data;
}

uint32_t DataImpl::len() const
{
    return m_len;
}

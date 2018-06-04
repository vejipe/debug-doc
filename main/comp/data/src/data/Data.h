#include <data/IDataProcAccess.h>

#include <cstdint>
#include <memory>

namespace data
{

// forward declaration
namespace internal
{
class DataImpl;
}

class Data
	: public IDataProcAccess
{
public:
	Data();

// Implementation of IDataProcAccess
public:
	uint8_t* data() const override;
	uint32_t len() const override;
	std::shared_ptr<IDataProcAccess> ptr() override;

private:
	std::unique_ptr<internal::DataImpl> m_impl;
};

} // namespace data

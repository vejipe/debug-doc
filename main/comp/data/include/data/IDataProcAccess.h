#include <cstdint>
#include <memory>

namespace data
{

class IDataProcAccess
	: public std::enable_shared_from_this<IDataProcAccess>
{
public:
	virtual uint8_t* data() const = 0;
	virtual uint32_t len() const = 0;
	virtual std::shared_ptr<IDataProcAccess> ptr() = 0;

protected:
	// ctor is protected to prevent direct object construction
	// (i.e., without shared_ptr)
	// This restriction enables to use ptr() (i.e., shared_ptr_from_this)
	// on any instance of the class.
    IDataProcAccess() = default;
};

// the only way to create an instance
std::shared_ptr<IDataProcAccess> createData();

} // namespace data


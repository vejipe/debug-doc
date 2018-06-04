#include <cstdint>

namespace data
{
namespace internal
{

class DataImpl
{
public:
	DataImpl();
    uint8_t* data();
	uint32_t len() const;

private:
    static const uint32_t m_len = 4;
    uint8_t m_data[m_len];
};

} // namespace internal
} // namespace data

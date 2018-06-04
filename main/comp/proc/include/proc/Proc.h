#include <memory>
#include <vector>

// forward declare
namespace data
{
class IDataProcAccess;
}

namespace proc
{

class Proc
{
public:
        void add(data::IDataProcAccess &);
        void run();
        void clear();

private:
        std::vector<std::shared_ptr<data::IDataProcAccess>> m_data;
};

} // namespace proc

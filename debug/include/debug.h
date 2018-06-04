#include <data/IDataProcAccess.h>

namespace proc
{
class Proc;
}

namespace debug
{

class DataDebug
    : public data::IDataProcAccess
{
public:
    DataDebug(data::IDataProcAccess &);

// implement data::IDataProcAccess
public:
    uint8_t* data() const override;
    uint32_t len() const override;
    std::shared_ptr<data::IDataProcAccess> ptr() override;

private:
    std::shared_ptr<data::IDataProcAccess> m_originalData;
};

std::shared_ptr<data::IDataProcAccess> createDataDebug(data::IDataProcAccess &);

} // namespace debug

// helper
void addToProc(proc::Proc *, data::IDataProcAccess *);
// to prevent addToProc from being removed from the final lib.
void dummyDep() { addToProc(nullptr, nullptr); }

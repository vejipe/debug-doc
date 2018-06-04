#include <debug.h>

#include <proc/Proc.h>

using namespace debug;

std::shared_ptr<data::IDataProcAccess> debug::createDataDebug(data::IDataProcAccess &orig)
{
    return std::make_shared<DataDebug>(orig);
}

DataDebug::DataDebug(data::IDataProcAccess &orig)
    : m_originalData(orig.ptr())
{

}

uint8_t* DataDebug::data() const
{
    return m_originalData->data();
}

uint32_t DataDebug::len() const
{
    return m_originalData->len() - 1;
}

std::shared_ptr<data::IDataProcAccess> DataDebug::ptr()
{
    return shared_from_this();
}

void addToProc(proc::Proc *p, data::IDataProcAccess *d)
{
    static auto debug_d = createDataDebug(*d);
    p->add(*debug_d);
}

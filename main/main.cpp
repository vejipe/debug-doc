#include <data/IDataProcAccess.h>
#include <proc/Proc.h>

#include <chrono>
#include <thread>

#include <dlfcn.h>

int main(int argc, char** /*argv*/)
{
    auto d = data::createData();
    proc::Proc p;

    // dummy dependency on 'dlopen'
    if(argc == 2)
    {
        dlopen(nullptr, 1);
    }

    while(true)
    {
        p.add(*d);
        p.run();
        p.clear();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

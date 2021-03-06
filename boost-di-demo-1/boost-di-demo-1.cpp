// This is the C++ version of ninject-demo.

#include "stdafx.h"
#include "boost/di.hpp"
#include <string>
#include <iostream>
#include <memory>

namespace di = boost::di;
using namespace std;

struct IDoSomethingable
{
    virtual ~IDoSomethingable() = default;
    virtual void DoSomething() = 0;
};

struct IWhat
{
    virtual ~IWhat() = default;
    virtual string What() = 0;
};

struct ITimes
{
    virtual ~ITimes() = default;
    virtual int Times() = 0;
};

class DoSomethinger : public IDoSomethingable
{
    unique_ptr<IWhat> _a;
    unique_ptr<ITimes> _b;

    void DoSomething() override
    {
        cout << "Here we go again...\n";
        for (int i = 0; i < _b->Times(); ++i)
            cout << _a->What() << "\n";
    }

public:
    DoSomethinger(unique_ptr<IWhat> a, unique_ptr<ITimes> b) : _a{move(a)}, _b{move(b)}
    {
    }
};

class ProductionWhat : public IWhat
{
    string What() override
    {
        return "The Real Slim Shady";
    }
};

class TestWhat : public IWhat
{
    string What() override
    {
        return "Just a test";
    }
};

class ProductionTimes : public ITimes
{
    int _times;

public:
    int Times() override
    {
        return _times;
    }

    ProductionTimes(int times) : _times{times}
    {
    }
};

static void create_simple_objects()
{
    cout << "create_simple_objects()" << "\n";

    auto const injector = di::make_injector(
        di::bind<IWhat>().to<ProductionWhat>(),
        // Note that without binding an int, it will still work with 0.
        di::bind<int>().to(7),
        di::bind<ITimes>().to<ProductionTimes>(),
        // Note that it will use 42, even if 7 was specified before.
        di::bind<int>().to(42)[di::override]
    );

    auto a = injector.create<IWhat*>();
    cout << "a.What() == " << a->What() << "\n";

    auto b = injector.create<unique_ptr<ProductionTimes>>();
    cout << "b->Times() == " << b->Times() << "\n";

    auto ib = injector.create<unique_ptr<ITimes>>();
    cout << "ib->Times() == " << ib->Times() << "\n";

    auto const statelessInjector = di::make_injector(
        di::bind<ITimes>().to([](auto const& injector) -> unique_ptr<ITimes>
        {
            return make_unique<ProductionTimes>(3);
        })
    );

    cout << "sizeof(statelessInjector) == " << sizeof(statelessInjector) << "\n";

    auto const statefulInjector = di::make_injector(
        di::bind<ITimes>().to([times = 3](auto const& injector) -> unique_ptr<ITimes>
        {
            return make_unique<ProductionTimes>(times);
        })
    );

    cout << "sizeof(statefulInjector) == " << sizeof(statefulInjector) << "\n";

    // It displays the following:
    /*
    create_simple_objects()
    a.What() == The Real Slim Shady
    b->Times() == 42
    ib->Times() == 42
    sizeof(statelessInjector) == 4
    sizeof(statefulInjector) == 12
    */
}

static void create_compound_object()
{
    cout << "create_compound_object()" << "\n";

    // Note the type of the productionInjector:
    //di::core::injector<
    //    di::config, 
    //    di::core::pool_t<>, 
    //    di::core::dependency<di::scopes::deduce, IDoSomethingable, DoSomethinger>,
    //    di::core::dependency<di::scopes::deduce, IWhat, ProductionWhat>, 
    //    di::core::dependency<di::scopes::instance, ITimes, struct <...lambda...>>
    //>

    auto productionInjector = di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IWhat>().to<ProductionWhat>(),
        // Note that without the return type specifier, it returns a const unique_ptr...
        di::bind<ITimes>().to(
            [](auto const& injector) -> unique_ptr<ITimes> { return make_unique<ProductionTimes>(3); })
    );

    auto dosomethingable = productionInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();

    // Note that passing in an injector would not work, not copyable, only movable.
    //auto testInjector = di::make_injector(injector);
    //auto testInjector = di::make_injector(move(injector));
}

static void create_test_object()
{
    cout << "create_test_object()" << "\n";

    auto const makeProductionInjector = []
    {
        return di::make_injector(
            di::bind<IDoSomethingable>().to<DoSomethinger>(),
            di::bind<IWhat>().to<ProductionWhat>(),
            di::bind<ITimes>().to(
                [](auto const& injector) -> unique_ptr<ITimes> { return make_unique<ProductionTimes>(3); })
        );
    };

    // Note the type of the testInjector:
    //di::core::injector<
    //    di::config,
    //    di::core::pool_t<>,
    //    di::core::injector<
    //        di::config,
    //        di::core::pool_t<>,
    //        di::core::dependency<di::scopes::deduce, IDoSomethingable, DoSomethinger>,
    //        di::core::dependency<di::scopes::deduce, IWhat, ProductionWhat>,
    //        di::core::dependency<di::scopes::instance, ITimes, struct <...lambda...>>
    //    >,
    //    di::core::dependency<di::scopes::deduce, IWhat, TestA, di::no_name, di::core::override>,
    //    di::core::dependency<di::scopes::instance, ITimes, struct <...lambda...>, di::no_name, di::core::override>
    //>

    auto const testInjector = di::make_injector(
        makeProductionInjector(),
        di::bind<IWhat>().to<TestWhat>()[di::override],
        di::bind<ITimes>().to(
            [](auto const& injector) -> unique_ptr<ITimes> { return make_unique<ProductionTimes>(2); })[di::override]
    );

    auto dosomethingable = testInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();
}

static di::injector<unique_ptr<IDoSomethingable>> module1()
{
    return di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IWhat>().to<ProductionWhat>(),
        di::bind<ITimes>().to(
            [](auto const& injector) -> unique_ptr<ITimes> { return make_unique<ProductionTimes>(3); })
    );
}

static di::injector<unique_ptr<IDoSomethingable>> module2()
{
    return di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IWhat>().to<TestWhat>(),
        di::bind<ITimes>().to<ProductionTimes>(),
        di::bind<int>().to(2)
    );
}

static void DoSomething(di::injector<unique_ptr<IDoSomethingable>> const& typeErasedInjector)
{
    auto dosomethingable = typeErasedInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();
}

static void module_type_erasure()
{
    cout << "module_type_erasure()" << "\n";

    di::injector<unique_ptr<IDoSomethingable>> const m1 = module1();
    DoSomething(m1);
    di::injector<unique_ptr<IDoSomethingable>> const m2 = module2();
    DoSomething(m2);

    // Note that this will not work, type erased injector's bindings are fixed!!!
    auto const testInjector = di::make_injector(
        module1(),
        di::bind<IWhat>().to<TestWhat>()[di::override],
        di::bind<ITimes>().to(
            [](auto const& injector) -> unique_ptr<ITimes> { return make_unique<ProductionTimes>(2); })[di::override]
    );
    auto dosomethingable = testInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();

    cout << "However the bindings were overriden: " << testInjector.create<unique_ptr<IWhat>>()->What() << endl;
}

int main()
{
    create_simple_objects();
    //create_compound_object();
    //create_test_object();
    //module_type_erasure();

    return 0;
}

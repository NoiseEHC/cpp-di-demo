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

struct IA
{
    virtual ~IA() = default;
    virtual string What() = 0;
};

struct IB
{
    virtual ~IB() = default;
    virtual int Times() = 0;
};

class DoSomethinger : public IDoSomethingable
{
    unique_ptr<IA> _a;
    unique_ptr<IB> _b;

    void DoSomething() override
    {
        cout << "Here we go again...\n";
        for (int i = 0; i < _b->Times(); ++i)
            cout << _a->What() << "\n";
    }

public:
    DoSomethinger(unique_ptr<IA> a, unique_ptr<IB> b) : _a{move(a)}, _b{move(b)}
    {
    }
};

class A : public IA
{
public:
    string What() override
    {
        return "The Real Slim Shady";
    }
};

class TestA : public IA
{
    string What() override
    {
        return "Just a test";
    }
};

class B : public IB
{
    int _times;

public:
    int Times() override
    {
        return _times;
    }

    B(int times) : _times{times}
    {
    }
};

static void create_simple_objects()
{
    cout << "create_simple_objects()" << "\n";
    auto const injector = di::make_injector(
        di::bind<IA>().to<A>(),
        // Note that without binding an int, it will still work with 0.
        di::bind<int>().to(7),
        di::bind<IB>().to<B>(),
        // Note that it will use 42, even if 7 was specified before.
        di::bind<int>().to(42)[di::override]
    );

    auto a = injector.create<A>();
    cout << a.What() << "\n";

    auto b = injector.create<unique_ptr<B>>();
    cout << b->Times() << "\n";

    auto ib = injector.create<unique_ptr<IB>>();
    cout << ib->Times() << "\n";
}

static void create_compound_object()
{
    cout << "create_compound_object()" << "\n";

    // Note the type of the injector:
    //di::core::injector<
    //    di::config, 
    //    di::core::pool_t<>, 
    //    di::core::dependency<di::scopes::deduce, IDoSomethingable, DoSomethinger>,
    //    di::core::dependency<di::scopes::deduce, IA, A>, 
    //    di::core::dependency<di::scopes::instance, IB, struct <...lambda...>>
    //>

    auto injector = di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IA>().to<A>(),
        di::bind<IB>().to([](auto const& injector) -> unique_ptr<IB> { return make_unique<B>(3); })
    );

    auto dosomethingable = injector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();

    // Note that passing in an injector would not work, not copyable, only movable.
    //auto testInjector = di::make_injector(injector);
    //auto testInjector = di::make_injector(move(injector));
}

static void create_test_object()
{
    cout << "create_test_object()" << "\n";
    auto const realModule = []
    {
        return di::make_injector(
            di::bind<IDoSomethingable>().to<DoSomethinger>(),
            di::bind<IA>().to<A>(),
            di::bind<IB>().to([](auto const& injector) -> unique_ptr<IB> { return make_unique<B>(3); })
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
    //        di::core::dependency<di::scopes::deduce, IA, A>,
    //        di::core::dependency<di::scopes::instance, IB, struct <...lambda...>>
    //    >,
    //    di::core::dependency<di::scopes::deduce, IA, TestA, di::no_name, di::core::override>,
    //    di::core::dependency<di::scopes::instance, IB, struct <...lambda...>, di::no_name, di::core::override>
    //>

    auto const testInjector = di::make_injector(
        realModule(),
        di::bind<IA>().to<TestA>()[di::override],
        di::bind<IB>().to([](auto const& injector) -> unique_ptr<IB> { return make_unique<B>(2); })[di::override]
    );

    auto dosomethingable = testInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();
}

static di::injector<unique_ptr<IDoSomethingable>> module1()
{
    return di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IA>().to<A>(),
        di::bind<IB>().to([](auto const& injector) -> unique_ptr<IB> { return make_unique<B>(3); })
    );
}

static di::injector<unique_ptr<IDoSomethingable>> module2()
{
    return di::make_injector(
        di::bind<IDoSomethingable>().to<DoSomethinger>(),
        di::bind<IA>().to<TestA>(),
        di::bind<IB>().to<B>(),
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
        di::bind<IA>().to<TestA>()[di::override],
        di::bind<IB>().to([](auto const& injector) -> unique_ptr<IB> { return make_unique<B>(2); })[di::override]
    );
    auto dosomethingable = testInjector.create<unique_ptr<IDoSomethingable>>();
    dosomethingable->DoSomething();
}

int main()
{
    create_simple_objects();
    //create_compound_object();
    //create_test_object();
    //module_type_erasure();

    return 0;
}

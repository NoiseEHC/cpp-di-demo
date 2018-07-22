using System;
using Ninject;

interface IDoSomethingable
{
    void DoSomething();
}

interface IA
{
    string What();
}

interface IB
{
    int Times();
}

class DoSomethinger : IDoSomethingable
{
    private readonly IA _a;
    private readonly IB _b;

    // Note that the compiler thinks this is unused...
    public DoSomethinger(IA a, IB b)
    {
        _a = a;
        _b = b;
    }

    public void DoSomething()
    {
        Console.WriteLine("Here we go again...");
        for (int i = 0; i < _b.Times(); ++i)
            Console.WriteLine(_a.What());
    }
}

class A : IA
{
    public string What()
    {
        return "The Real Slim Shady";
    }
}

class TestA : IA
{
    public string What()
    {
        return "Just a test";
    }
}

class B : IB
{
    private readonly int _times;

    public B(int times)
    {
        _times = times;
    }

    public int Times()
    {
        return _times;
    }
}

class DemoModule : Ninject.Modules.NinjectModule
{
    public override void Load()
    {
        Bind<IDoSomethingable>().To<DoSomethinger>();

        Bind<IA>().To<A>();
        // Note that this does not compile:
        //Bind<IB>().To<A>();

        Bind<IB>().ToMethod(context => new B(3));
        // Note that this is a runtime error:
        //Bind<IB>().To<B>();
        // Unless you fix it by this:
        //Bind<int>().ToConstant(5);
    }
}

namespace ninject_demo
{
    class Program
    {
        static void Main()
        {
            Real();
            //Test();
        }

        static void Real()
        {
            IKernel kernel = new StandardKernel(new DemoModule());
            
            var dosomethingable = kernel.Get<IDoSomethingable>();
            dosomethingable.DoSomething();
        }

        static void Test()
        {
            IKernel kernel = new StandardKernel(new DemoModule());

            // Note that even if the object hierarchy changes, nothing changes in this test!!!
            kernel.Rebind<IA>().To<TestA>();
            kernel.Rebind<IB>().ToMethod(context => new B(2));
            
            var dosomethingable = kernel.Get<IDoSomethingable>();
            dosomethingable.DoSomething();
        }
    }
}
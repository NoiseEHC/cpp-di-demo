using System;
using Ninject;

interface IDoSomethingable
{
    void DoSomething();
}

interface IWhat
{
    string What();
}

interface ITimes
{
    int Times();
}

class DoSomethinger : IDoSomethingable
{
    private readonly IWhat _a;
    private readonly ITimes _b;

    // Note that the compiler thinks this is unused...
    public DoSomethinger(IWhat a, ITimes b)
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

class ProductionWhat : IWhat
{
    public string What()
    {
        return "The Real Slim Shady";
    }
}

class TestWhat : IWhat
{
    public string What()
    {
        return "Just a test";
    }
}

class ProductionTimes : ITimes
{
    private readonly int _times;

    public ProductionTimes(int times)
    {
        _times = times;
    }

    public int Times()
    {
        return _times;
    }
}

class ProductionModule : Ninject.Modules.NinjectModule
{
    public override void Load()
    {
        Bind<IDoSomethingable>().To<DoSomethinger>();

        Bind<IWhat>().To<ProductionWhat>();
        // Note that this does not compile:
        //Bind<ITimes>().To<ProductionWhat>();

        Bind<ITimes>().ToMethod(context => new ProductionTimes(3));
        // Note that this is a runtime error:
        //Bind<ITimes>().To<ProductionTimes>();
        // Unless you fix it by this:
        //Bind<int>().ToConstant(3);
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
            IKernel productionKernel = new StandardKernel(new ProductionModule());
            
            var dosomethingable = productionKernel.Get<IDoSomethingable>();
            dosomethingable.DoSomething();
        }

        static void Test()
        {
            IKernel testKernel = new StandardKernel(new ProductionModule());

            // Note that even if the object hierarchy changes, nothing changes in this test!!!
            testKernel.Rebind<IWhat>().To<TestWhat>();
            testKernel.Rebind<ITimes>().ToMethod(context => new ProductionTimes(2));
            
            var dosomethingable = testKernel.Get<IDoSomethingable>();
            dosomethingable.DoSomething();
        }
    }
}
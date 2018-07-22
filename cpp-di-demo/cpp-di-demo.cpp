#include "stdafx.h"
#include <memory>
#include "DoSomething.h"
#include "CallSomething.h"

#include "boost/di.hpp"
namespace di = boost::di;

int main()
{
    std::cout << "direct call" << std::endl;
    // Not that this is an unique_ptr cast!!!
    std::unique_ptr<IDoSomething const> doSomethinger
        {std::make_unique<DoSomethinger const>(1, "2")};
    doSomethinger->DoIt(3, "4");

    std::cout << "call through caller" << std::endl;
    CallSomethinger const callSomethinger(std::move(doSomethinger));
    callSomethinger.CallIt(3);

    std::cout << "call through injection created caller" << std::endl;
    auto injector = di::make_injector(
        // Note that the parameter is an unique_ptr!!!
        di::bind<IDoSomething>().to<DoSomethinger>(),
        // Note that these will be used in ALL constructors!!!
        di::bind<int>().to(42),
        di::bind<std::string>().to("str")
    );
    auto const caller = injector.create<CallSomethinger>();
    caller.CallIt(4);

    //int a = int{};
    //std::cout << a << std::endl;

    return 0;
}

//#include "stdafx.h"
//#include <cassert>
//#include <string>
//#include "boost/di.hpp"
//
//namespace di = boost::di;
//
////<-
//struct renderer {
//	int device;
//};
////->
//
//class iview {
//public:
//	virtual ~iview() noexcept = default;
//	virtual void update() = 0;
//};
//
//class gui_view : public iview {
//public:
//	gui_view(std::string /*title*/, const renderer& r) { assert(42 == r.device); }
//	void update() override {}
//};
//
//class text_view : public iview {
//public:
//	void update() override {}
//};
//
////<-
//class model {};
////->
//
//class controller {
//public:
//	controller(model&, iview&) {}
//};
//
////<-
//class user {};
//
//class app {
//public:
//	app(controller&, user&) {}
//};
////->
//
//int main() {
//	// clang-format off
//	auto injector = di::make_injector(
//		di::bind<iview>().to<gui_view>()
//		, di::bind<int>().to(42) // renderer device
//	);
//	// clang-format on
//
//	injector.create<app>();
//}

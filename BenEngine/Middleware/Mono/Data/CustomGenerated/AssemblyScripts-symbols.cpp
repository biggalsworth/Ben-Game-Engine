#include <D:\Github\GameEngine-12129991\BenEngine\src\Minimal.h>
#include <new>

int (TestClass::*_0)() = &TestClass::GetValue;
void (TestClass::*_1)(int) = &TestClass::SetValue;
class TestClass& (TestClass::*_2)(class TestClass&&) = &TestClass::operator=;
extern "C" __declspec(dllexport) void Data_Data(void* __instance) { ::new (__instance) Data(); }
class Data& (Data::*_3)(class Data&&) = &Data::operator=;

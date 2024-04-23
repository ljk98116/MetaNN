#include "CppUTest/TestHarness.h"
#include "CppUTest/CommandLineTestRunner.h"
#include <iostream>

class ClassName{};
TEST_GROUP(ClassName)
{
	ClassName* className;

	void setup()
	{
		className = new ClassName();
	}
	void teardown()
	{
		delete className;
	}
};

TEST(ClassName, Create)
{
	CHECK(0 != className);
	CHECK(true);
	CHECK_EQUAL(1,1);
	LONGS_EQUAL(1,1);
	DOUBLES_EQUAL(1.000, 1.001, .01);
	STRCMP_EQUAL("hello", "hello");
	FAIL("The prior tests pass, but this one doesn't");
}

int main(int argc, char *argv[])
{
    CommandLineTestRunner::RunAllTests(argc, argv);
    return 0;
}
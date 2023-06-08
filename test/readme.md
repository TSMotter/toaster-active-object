# Google test

## Basic concepts
- A *Test* contains assertions
- *Test suite* contains one or more *Tests*
    - *Test suite* should reflect the structure of the tested code
    - When multiple *Tests* in a *Test suite* need to share objects/subroutines, they can be grouped into a *Test fixture*
- A *Test program* can contain multiple *Test suites*

## Assertions
- `ASSERT_*` methods generate fatal failures when fail
- `EXPECT_*` methods generate nonfatal failures when fail
    - Generally preferred because allow for multiple failures to be reported in a single test run

## Simple tests structure
- General test structure:
```Cpp
TEST(TestSuiteName, IndividualTestName) {
  ... test body ...
}
```
#### Example 1 - Simplest of tests:
```Cpp
int Factorial(int n);  // Returns the factorial of n

// Tests factorial of 0.
TEST(FactorialTest, HandlesZeroInput) {
  EXPECT_EQ(Factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(FactorialTest, HandlesPositiveInput) {
  EXPECT_EQ(Factorial(1), 1);
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(3), 6);
  EXPECT_EQ(Factorial(8), 40320);
}
```

## Using test fixtures
- Derive a class from ::testing::Test . Start its body with `protected:`
- Inside the class, declare any objects you plan to use.
- If necessary, write a default constructor or SetUp() function 
- If necessary, write a destructor or TearDown() function
- If needed, define subroutines for tests to share
- Use `TEST_F` instead of `TEST` for the tests definitions
    - In which case, the first parameter becames the fixture name instead of the name of the test suite
- For each test defined with TEST_F(), GoogleTest will create a fresh test fixture at runtime, immediately initialize it via SetUp(), run the test, clean up by calling TearDown(), and then delete the test fixture
#### Example 2 - Tests with fixture
```C++
#include "this/package/foo.h"

#include "gtest/gtest.h"

namespace my
{
namespace project
{
namespace
{

// The fixture for testing class Foo.
class FooTest : public ::testing::Test
{
   protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    FooTest()
    {
        // You can do set-up work for each test here.
    }

    ~FooTest() override
    {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc)
{
    const std::string input_filepath  = "this/package/testdata/myinputfile.dat";
    const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
    Foo               f;
    EXPECT_EQ(f.Bar(input_filepath, output_filepath), 0);
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz)
{
    // Exercises the Xyz feature of Foo.
}

}  // namespace
}  // namespace project
}  // namespace my

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```



## Invoking the tests
- After defining your tests, you can run them with RUN_ALL_TESTS(), which returns 0 if all the tests are successful, or 1 otherwise
- Developer can write it's own `main()` function or link the tests with the provided `gtest_main` library and it "just works"

## Mocks
- A mock object implements the same interface as a real object (so it can be used as one), but lets you specify at run time how it will be used and what it should do (which methods will be called? in which order? how many times? with what arguments? what will they return? etc)
    - Allows you to check the interaction between itself and code that uses it
- The key to using a mock object successfully is to set the right expectations on it. If you set the expectations too strict, your test will fail as the result of unrelated changes. If you set them too loose, bugs can slip through. 
#### Example 3 - Basic mock:
```C++
class Turtle {
  ...
  virtual ~Turtle() {}
  virtual void PenUp() = 0;
  virtual void PenDown() = 0;
  virtual void Forward(int distance) = 0;
  virtual void Turn(int degrees) = 0;
  virtual void GoTo(int x, int y) = 0;
  virtual int GetX() const = 0;
  virtual int GetY() const = 0;
};
.
.
.
#include "gmock/gmock.h"  // Brings in gMock.

class MockTurtle : public Turtle {
 public:
  ...
  MOCK_METHOD(void, PenUp, (), (override));
  MOCK_METHOD(void, PenDown, (), (override));
  MOCK_METHOD(void, Forward, (int distance), (override));
  MOCK_METHOD(void, Turn, (int degrees), (override));
  MOCK_METHOD(void, GoTo, (int x, int y), (override));
  MOCK_METHOD(int, GetX, (), (const, override));
  MOCK_METHOD(int, GetY, (), (const, override));
};
.
.
.
#include "path/to/mock-turtle.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::AtLeast

TEST(PainterTest, CanDrawSomething) {
  MockTurtle turtle
  EXPECT_CALL(turtle, PenDown())
      .Times(AtLeast(1));

  Painter painter(&turtle);

  EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
}
```
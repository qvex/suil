//
// Created by dc on 7/14/16.
//

#ifndef SUIL_ROUTE_PARAMS_TESTS_H
#define SUIL_ROUTE_PARAMS_TESTS_H

#include "gtest/gtest.h"
#include "route_params.h"

using namespace gap;

TEST(ParamValuesTest, IntParamValueTest) {
    ParamValue::Type type  = ParamValue::Type::T_INT;
    ParamValue value(type, "0");
    ASSERT_EQ(value.i(), 0);
    ASSERT_EQ(value.s(), "0");

    // Setting to undefined number
    value = ParamValue(type, "undef");
    ASSERT_EQ(value.type(), ParamValue::Type::T_UNDEFINED);
    ASSERT_TRUE(value.isUndefined());
    ASSERT_EQ(value.i(), INT64_MIN);
    ASSERT_EQ(value.s(), ParamValue::undefined().s());

    value = ParamValue(type, "1989");
    ASSERT_EQ(value.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(value.i(), 1989);
    ASSERT_EQ(value.u(), 1989);
    ASSERT_EQ(value.s(), "1989");

    // negative numbers
    value = ParamValue(type, "-1989");
    ASSERT_EQ(value.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(value.i(), -1989);
    ASSERT_EQ(value.u(), -1989);
    ASSERT_EQ(value.s(), "-1989");
    // floats to integers (although gap won't let you)
    value = ParamValue(type, "-1.6");
    ASSERT_EQ(value.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(value.i(), -1);
    ASSERT_EQ(value.u(), -1);
    ASSERT_EQ(value.s(), "-1.6");
}


TEST(ParamValuesTest, FloatParamValueTest) {
    ParamValue::Type type  = ParamValue::Type::T_FLOAT;

    ParamValue value(type, "0.0");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.f(), 0.0);
    ASSERT_EQ(value.s(), "0.0");

    value = ParamValue(type, "12.5");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.f(), 12.5);
    ASSERT_EQ(value.s(), "12.5");

    // negative float numbers
    value = ParamValue(type, "-12.5");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.f(), -12.5);
    ASSERT_EQ(value.s(), "-12.5");

    // large numbers
    value = ParamValue(type, "999999999999999999.500000000000000000000000009");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.f(), 999999999999999999.500000000000000000000000009);
    ASSERT_EQ(value.s(), "999999999999999999.500000000000000000000000009");
}

TEST(ParamValuesTest, StringParamValueTest) {
    ParamValue::Type type  = ParamValue::Type::T_STRING;

    ParamValue value(type, "Hello World");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.s(), "Hello World");
    ASSERT_EQ(value.i(), INT64_MIN);

    // empty string
    value = ParamValue(type, "");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.s(), "");
    ASSERT_EQ(value.i(), INT64_MIN);
}

TEST(ParamValuesTest, UnsignedParamValueTest)
{
    ParamValue::Type type  = ParamValue::Type::T_UINT;

    ParamValue value(type, "125");
    ASSERT_EQ(value.type(), type);
    ASSERT_EQ(value.s(), "125");
    ASSERT_EQ(value.i(), 125);
    ASSERT_EQ(value.u(), 125);

    // empty string
    value = ParamValue(type, "undef");
    ASSERT_EQ(value.type(), ParamValue::Type::T_UNDEFINED);
    ASSERT_EQ(value.s(), "undefined");
    ASSERT_EQ(value.i(), INT64_MIN);

    // Assigning negative numbers to unsigned should fail
    value = ParamValue(type, "-125");
    ASSERT_EQ(value.type(), ParamValue::Type::T_UNDEFINED);
    ASSERT_EQ(value.s(), "undefined");
    ASSERT_EQ(value.i(), INT64_MIN);
}

class RouteParamsTest {
public:
    static RouteParams create() {
        return  RouteParams();
    }

    static void update(RouteParams& p, const std::string& name, const ParamValue::Type& type, const std::string& value) {
        p.update(name, type, value);
    }

    static bool isEmpty(RouteParams& p) {
        return p.values_.empty();
    }
};


TEST(RouteParamsTest, EmptyRouteParamsTest)
{
    RouteParams params = RouteParamsTest::create();
    ASSERT_TRUE(RouteParamsTest::isEmpty(params));
}

TEST(RouteParamsTest, AddParametersTest)
{
    RouteParams params = RouteParamsTest::create();
    ASSERT_TRUE(RouteParamsTest::isEmpty(params));
    RouteParamsTest::update(params, "one", ParamValue::Type::T_INT, "1");
    ASSERT_FALSE(RouteParamsTest::isEmpty(params));
    ParamValue p = params["one"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(p.i(), 1);

    RouteParamsTest::update(params, "name", ParamValue::Type::T_STRING, "carter");
    ASSERT_TRUE(RouteParamsTest::isEmpty(params));
    p = params["name"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(p.s(), "carter");

    RouteParamsTest::update(params, "float", ParamValue::Type::T_FLOAT, "45.89");
    ASSERT_TRUE(RouteParamsTest::isEmpty(params));
    p = params["float"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_FLOAT);
    ASSERT_EQ(p.f(), 45.89);
}

TEST(RouteParamsTest, UpdateParametersTests)
{
    RouteParams params = RouteParamsTest::create();
    ASSERT_TRUE(RouteParamsTest::isEmpty(params));
    RouteParamsTest::update(params, "one", ParamValue::Type::T_INT, "1");
    ASSERT_FALSE(RouteParamsTest::isEmpty(params));
    ParamValue p = params["one"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(p.i(), 1);

    RouteParamsTest::update(params, "one", ParamValue::Type::T_INT, "890");
    ASSERT_FALSE(RouteParamsTest::isEmpty(params));
    p = params["one"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_INT);
    ASSERT_EQ(p.i(), 890);

    RouteParamsTest::update(params, "one", ParamValue::Type::T_UINT, "1");
    ASSERT_FALSE(RouteParamsTest::isEmpty(params));
    p = params["one"];
    ASSERT_FALSE(p.isUndefined());
    ASSERT_EQ(p.type(), ParamValue::Type::T_UINT);
    ASSERT_EQ(p.i(), 1);
}

#endif //SUIL_ROUTE_PARAMS_TESTS_H







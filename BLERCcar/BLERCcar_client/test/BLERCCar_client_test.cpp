#include "gtest/gtest.h"
#include "BLERCCar_client.hpp"

TEST(TestBLERCcar, ConnectionTest)
{
    auto server{"78:E3:6D:65:45:22"};
    BLERCCar_client car_client{};
    EXPECT_TRUE(car_client.Connect(server));
}
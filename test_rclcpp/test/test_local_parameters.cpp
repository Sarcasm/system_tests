// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "rclcpp/rclcpp.hpp"

#include "parameter_fixtures.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

TEST(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), to_string) {
  rclcpp::parameter::ParameterVariant pv("foo", "bar");
  rclcpp::parameter::ParameterVariant pv2("foo2", "bar2");
  std::string json_dict = std::to_string(pv);
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"string\", \"value\": \"bar\"}",
    json_dict.c_str());
  json_dict = rclcpp::parameter::_to_json_dict_entry(pv);
  EXPECT_STREQ(
    "\"foo\": {\"type\": \"string\", \"value\": \"bar\"}",
    json_dict.c_str());
  std::vector<rclcpp::parameter::ParameterVariant> vpv;
  vpv.push_back(pv);
  vpv.push_back(pv2);
  json_dict = std::to_string(vpv);
  EXPECT_STREQ(
    "{\"foo\": {\"type\": \"string\", \"value\": \"bar\"}, "
    "\"foo2\": {\"type\": \"string\", \"value\": \"bar2\"}}",
    json_dict.c_str());

  pv = rclcpp::parameter::ParameterVariant("foo", 2.1);
  // TODO(tfoote) convert the value to a float and use epsilon test.
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"double\", \"value\": \"2.100000\"}",
    std::to_string(pv).c_str());
  pv = rclcpp::parameter::ParameterVariant("foo", 8);
  EXPECT_STREQ(
    "{\"name\": \"foo\", \"type\": \"integer\", \"value\": \"8\"}",
    std::to_string(pv).c_str());
}

TEST(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_synchronous) {
  auto node = rclcpp::Node::make_shared("test_parameters_local_synchronous");
  // TODO(esteve): Make the parameter service automatically start with the node.
  auto parameter_service = std::make_shared<rclcpp::parameter_service::ParameterService>(node);
  auto parameters_client = std::make_shared<rclcpp::parameter_client::SyncParametersClient>(node);
  set_test_parameters(parameters_client);
  verify_test_parameters(parameters_client);
}

TEST(CLASSNAME(test_local_parameters, RMW_IMPLEMENTATION), local_asynchronous) {
  auto node = rclcpp::Node::make_shared(std::string("test_parameters_local_asynchronous"));
  // TODO(esteve): Make the parameter service automatically start with the node.
  auto parameter_service = std::make_shared<rclcpp::parameter_service::ParameterService>(node);
  auto parameters_client = std::make_shared<rclcpp::parameter_client::AsyncParametersClient>(node);
  verify_set_parameters_async(node, parameters_client);
  verify_get_parameters_async(node, parameters_client);
}

int main(int argc, char ** argv)
{
  // NOTE: use custom main to ensure that rclcpp::init is called only once
  rclcpp::init(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return ret;
}

//
// Created by FullHat on 18/04/2024.
//

#ifndef ENGINE_TESTS_GRAPHIC_MODULE_H
#define ENGINE_TESTS_GRAPHIC_MODULE_H

#include <graphics/facades/facade.hpp>
#include <gtest/gtest.h>

using WindowType = window_api_impls::WindowApiFacadeGlfwImpl;

#define GraphicDependencies graphic_api_impls::VulkanDependencies<WindowType>

class VkTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    WindowApiFacade<> *WindowAdapterInstance = nullptr;
    ASSERT_NO_THROW(WindowAdapterInstance =
                        new WindowApiFacade<>(WindowFacadeStructs::WindowProps{
                            .Size = {800, 600},
                            .Title = "Test",
                            .Mode = WindowFacadeStructs::WindowProps::WINDOWED,
                            .IsResizable = false,
                        }));

    auto InstanceProps = GraphicFacadeStructs::InstanceProps{
        .AppName = "Test",
        .EngineName = "Test",
        .AppVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .EngineVersion = {.Major = 1, .Minor = 0, .Patch = 0},
        .RequestedWindowExtensions =
            // clang-format off
            WindowAdapterInstance->ImplInstance.getRequiredExtensions(),
            // clang-format on
    };

    auto FacadeProps =
        GraphicFacadeStructs::GraphicEngineProps<GraphicDependencies>{
            .Dependencies = {std::move(*WindowAdapterInstance)},
            .InstancePropsInstance = InstanceProps,
            .PhysicalDevicePropsInstance = {},
        };

    GraphicDependencies *Dependencies = nullptr;
    ASSERT_NO_THROW(Dependencies = new GraphicDependencies(
                        {.Window = std::move(*WindowAdapterInstance)}));

    GraphicAdapterInstance = new GraphicApiFacade<
        GraphicDependencies,
        graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependencies>>(
        std::move(FacadeProps));

    ASSERT_TRUE(GraphicAdapterInstance != nullptr);
  }

  static void TearDownTestSuite() {}

  static inline GraphicApiFacade<
      GraphicDependencies,
      graphic_api_impls::GraphicApiFacadeVulkanImpl<GraphicDependencies>>
      *GraphicAdapterInstance;
};

TEST_F(VkTest, GraphicGetters) {
  ASSERT_TRUE(
      GraphicAdapterInstance->ImplInstance.getLocalPhysicalDevices().size() >
      0);
}

TEST_F(VkTest, GraphicSetupEngine) {
  ASSERT_NO_THROW(GraphicAdapterInstance->ImplInstance.chooseGpu(
      GraphicFacadeStructs::DeviceChoosePolicy::BEST));

  ASSERT_NO_THROW(GraphicAdapterInstance->ImplInstance.chooseGpu({}));
}

#endif // ENGINE_TESTS_GRAPHIC_MODULE_H

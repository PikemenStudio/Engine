#include "../facades/facade.hpp"
#include "GraphicEngine.hpp"

using namespace graphic_api_impls;

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
using DataType = vk_core::GraphicEngine<WindowImpl, ShaderImpl, SceneImpl>;

template <
    WindowApiImpl WindowImpl, ShaderLoaderImpl ShaderImpl,
    SceneManagerImpl<scene_manager_facades::SceneManagerDependencies> SceneImpl>
using DataTypePtr = DataType<WindowImpl, ShaderImpl, SceneImpl> *;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convertors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <VulkanDependenciesConcept Dep>
vk_core::GraphicEngine<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                       typename Dep::SceneManagerType>::DeviceChoosePolicy
toModuleType(GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  switch (Policy) {
  case GraphicFacadeStructs::DeviceChoosePolicy::FIRST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType, typename Dep::ShaderLoaderType,
        typename Dep::SceneManagerType>::DeviceChoosePolicy::FIRST;
  case GraphicFacadeStructs::DeviceChoosePolicy::BEST:
    return vk_core::GraphicEngine<
        typename Dep::WindowType, typename Dep::ShaderLoaderType,
        typename Dep::SceneManagerType>::DeviceChoosePolicy::BEST;
  default:
    return vk_core::GraphicEngine<
        typename Dep::WindowType, typename Dep::ShaderLoaderType,
        typename Dep::SceneManagerType>::DeviceChoosePolicy::FIRST;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <VulkanDependenciesConcept Dep>
GraphicApiFacadeVulkanImpl<Dep>::GraphicApiFacadeVulkanImpl(
    GraphicFacadeStructs::GraphicEngineProps<Dep> &&Props) {

  // Convert
  // From GraphicFacadeStructs::GraphicEngineProps<Dep> &&Props
  // To   vk_core::GraphicEngine<Dep::WindowType>::GraphicEngineProps
  typename vk_core::GraphicEngine<
      typename Dep::WindowType, typename Dep::ShaderLoaderType,
      typename Dep::SceneManagerType>::GraphicEngineProps ModuleProps{
      .Window = Props.Dependencies.Window,
      .ShaderLoader = Props.Dependencies.ShaderLoader,
      .SceneManager = Props.Dependencies.SceneManager,
      .VkInstanceProps =
          {
              .AppName = std::move(Props.InstancePropsInstance.AppName),
              .EngineName = std::move(Props.InstancePropsInstance.EngineName),
              .AppVersion =
                  {
                      .Major = Props.InstancePropsInstance.AppVersion.Major,
                      .Minor = Props.InstancePropsInstance.AppVersion.Minor,
                      .Patch = Props.InstancePropsInstance.AppVersion.Patch,
                  },
              .EngineVersion =
                  {
                      .Major = Props.InstancePropsInstance.EngineVersion.Major,
                      .Minor = Props.InstancePropsInstance.EngineVersion.Minor,
                      .Patch = Props.InstancePropsInstance.EngineVersion.Patch,
                  },
              .RequestedWindowExtensions = std::move(
                  Props.InstancePropsInstance.RequestedWindowExtensions),
          },
      .VkPhysicalDeviceProps = {
          // Just empty :)
      }};

  // And pass to the module
  Data = new vk_core::GraphicEngine<typename Dep::WindowType,
                                    typename Dep::ShaderLoaderType,
                                    typename Dep::SceneManagerType>(
      std::move(ModuleProps));
}

template <VulkanDependenciesConcept Dep>
GraphicApiFacadeVulkanImpl<Dep>::~GraphicApiFacadeVulkanImpl() {
  delete static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                  typename Dep::SceneManagerType>>(Data);
}

template <VulkanDependenciesConcept Dep>
std::vector<GraphicFacadeStructs::PhysicalDeviceData>
GraphicApiFacadeVulkanImpl<Dep>::getLocalPhysicalDevices() const {
  std::vector<GraphicFacadeStructs::PhysicalDeviceData> Result;
  for (const auto &Device :
       static_cast<
           DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                       typename Dep::SceneManagerType>>(Data)
           ->getLocalPhysicalDevices()) {
    Result.push_back(GraphicFacadeStructs::PhysicalDeviceData{
        .Name = Device.Name,
        .ApiVersion = Device.ApiVersion,
        .DriverVersion = Device.DriverVersion,
        .VendorId = Device.VendorId,
        .DeviceId = Device.DeviceId,
        .Type = static_cast<GraphicFacadeStructs::PhysicalDeviceData::TypeEnum>(
            Device.Type),
    });
  }
  return Result;
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::PhysicalDeviceData &DeviceData) {
  typename vk_core::VkPhysicalDevice<
      typename Dep::WindowType, typename Dep::ShaderLoaderType,
      typename Dep::SceneManagerType>::PhysicalDeviceLocalProps Device{
      .Name = DeviceData.Name,
      .ApiVersion = DeviceData.ApiVersion,
      .DriverVersion = DeviceData.DriverVersion,
      .VendorId = DeviceData.VendorId,
      .DeviceId = DeviceData.DeviceId,
      .Type = static_cast<vk_core::VkPhysicalDevice<
          typename Dep::WindowType, typename Dep::ShaderLoaderType,
          typename Dep::SceneManagerType>::PhysicalDeviceLocalProps::TypeEnum>(
          DeviceData.Type),
  };
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                  typename Dep::SceneManagerType>>(Data)
      ->chooseLocalPhysicalDevice(Device);
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::chooseGpu(
    const GraphicFacadeStructs::DeviceChoosePolicy Policy) {
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                  typename Dep::SceneManagerType>>(Data)
      ->chooseLocalPhysicalDevice(toModuleType<Dep>(Policy));
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::render() {
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                  typename Dep::SceneManagerType>>(Data)
      ->render();
}

template <VulkanDependenciesConcept Dep>
void GraphicApiFacadeVulkanImpl<Dep>::addObjectData(
    const std::map<std::string, GraphicFacadeStructs::ObjectData> &ObjectData) {
  typename vk_core::VkPipeline<
      typename Dep::WindowType, typename Dep::ShaderLoaderType,
      typename Dep::SceneManagerType>::PublicObjectDataMap ModuleObjectData;

  for (const auto &[Name, ObjectData] : ObjectData) {
    ModuleObjectData[Name] = {
        .Vertices = ObjectData.Vertices,
        .Colors = ObjectData.Colors,
    };
  };
  static_cast<
      DataTypePtr<typename Dep::WindowType, typename Dep::ShaderLoaderType,
                  typename Dep::SceneManagerType>>(Data)
      ->addObject(ModuleObjectData);
}

// Explicitly instantiate class
template class graphic_api_impls::GraphicApiFacadeVulkanImpl<
    graphic_api_impls::VulkanDependencies<
        window_api_impls::WindowApiFacadeGlfwImpl,
        shader_loader_impls::ShaderLoaderSimpleImpl,
        scene_manager_facades::SceneManagerBaseImpl<
            scene_manager_facades::SceneManagerDependencies>>>;

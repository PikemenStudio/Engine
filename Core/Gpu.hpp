//
// Created by Full-Hat on 02/12/2023.
//

#pragma once

#include "vulkan/vulkan.hpp"

#include "Core/Instance.hpp"
#include "Core/Window/GlfwWindow.hpp"
#include "Shaders/Pipeline.hpp"

#include <optional>

class Gpu {
public:
    explicit Gpu(std::shared_ptr<peVk::Instance> _instance, GlfwWindow &_window);
    ~Gpu();

    std::vector<vk::PhysicalDevice> GetAllAvailable();
    void SetCurrent(vk::PhysicalDevice _device);

    explicit operator std::string() const;
    [[nodiscard]] std::string ToString() const { return (std::string)*this; }

    void CheckExtensions() const;
    static void CheckExtensions(const vk::PhysicalDevice &_device);
    static bool NoExceptCheckExtensions(const vk::PhysicalDevice &_device);

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    vk::Device GetLogicalDevice() const { return m_logical_device; }
    vk::Device& GetLogicalDevice() { return m_logical_device; }

    vk::Format GetSwapChainFormat() const { return m_swap_chain.m_format; }
    vk::Extent2D GetSwapChainExtent() const { return m_swap_chain.m_extent; }

    struct SwapChainFrame {
        vk::Image m_image { nullptr };
        vk::ImageView image_view { nullptr };
        vk::Framebuffer frame_buffer;
        vk::CommandBuffer command_buffer;
        // Syncs
        vk::Fence inFlightFence;
        vk::Semaphore imageAvailable, renderFinished;
    };

    std::vector<SwapChainFrame>& GetSwapChainFrames() { return m_swap_chain.m_frames; }

    // Command pool
    void make_command_pool();

    // Framebuffer
    struct framebufferInput {
        vk::Device device;
        vk::RenderPass renderpass;
        vk::Extent2D swapchainExtent;
    };

    void make_framebuffers(framebufferInput _input_chunk);

    void make_command_buffers();

    void make_syncs_objs();

    void RecordDrawCommand(vk::CommandBuffer command_buffer, uint32_t image_index, GraphicsPipeline &pipeline);
    void Render(GraphicsPipeline &pipeline);

    void DestroyCommandPool();

protected:
    void FindAndSetQueues();

    void QuerySwapChainSupport();

    vk::SurfaceFormatKHR ChooseSurfaceFormat();
    vk::PresentModeKHR ChoosePresentMode();
    vk::Extent2D ChooseExtent();

    void InitSwapchain();

    void InitSurface();

protected:
    vk::PhysicalDevice m_device { nullptr };
    vk::Device m_logical_device { nullptr };
    vk::Queue m_graphics_queue { nullptr };
    vk::Queue m_present_queue { nullptr };

    struct SwapChainBundle {
        vk::SwapchainKHR m_swap_chain{ nullptr };
        std::vector<SwapChainFrame> m_frames;
        vk::Format m_format;
        vk::Extent2D m_extent;
    } m_swap_chain;

    vk::SurfaceKHR m_surface;

    SwapChainSupportDetails m_swap_chain_support_details;

    struct QueueIndexes {
        std::optional<uint32_t> graphics_queue;
        std::optional<uint32_t> present_queue;

        bool FoundAll() const {
            return graphics_queue.has_value() && present_queue.has_value();
        }

        std::string GetNotSettedFamily() {
            std::stringstream str;
            if (!graphics_queue.has_value()) {
                str << "Graphics" << "\n";
            }
            if (!present_queue.has_value()) {
                str << "Present" << "\n";
            }
            return str.str();
        }
    } m_queue_indexes;

    std::shared_ptr<peVk::Instance> m_instance;

    GlfwWindow &m_window;

    vk::CommandPool m_command_pool;
    vk::CommandBuffer m_command_buffer;

    size_t MaxFramesInFlight;
    size_t FrameNumber;
};

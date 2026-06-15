//--------------------------------------------------------------------------------------
// PreOrderStub.cpp
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "PreOrderStub.h"

#include "ATGColors.h"
#include "FindMedia.h"

#include <map>

#include <XPackage.h>
#include <XStore.h>

#include <CommonStates.h>
#include <DirectXHelpers.h>
#include <WICTextureLoader.h>
#include <ResourceUploadBatch.h>

#include "FindMedia.h"

extern void ExitSample();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

namespace
{
    // Method to get localized messages if desired
    std::wstring GetLocalizedMessage()
    {
        #pragma message( __FILE__  ": Add localized message" )
        std::map<std::string, std::wstring> localizedMessages = {
            {"en-US", L"Click to launch Microsoft Store"}
        };

        char localeBuffer[256] = {};

        XPackageGetUserLocale(256, localeBuffer);

        auto messageValue = localizedMessages.find(localeBuffer);

        // If the specific locale didn't have a translation, return a default
        if (messageValue == localizedMessages.end())
        {
            return L"Click to launch Microsoft Store";
        }
        else
        {
            return messageValue->second;
        }
    }
}

Sample::Sample() noexcept(false)
{
    // Renders only 2D, so no need for a depth buffer.
    m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_UNKNOWN);
    m_deviceResources->RegisterDeviceNotify(this);
}

Sample::~Sample()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(HWND window, int width, int height)
{
	// NOTE: When running the app from the Start Menu (required for
	//	Store API's to work) the Current Working Directory will be
	//	returned as C:\Windows\system32 unless you overwrite it.
	//	The sample relies on the font and image files in the .exe's
	//	directory and so we do the following to set the working
	//	directory to what we want.
	char dir[1024];
	GetModuleFileNameA(NULL, dir, 1024);
	std::string exe = dir;
	exe = exe.substr(0, exe.find_last_of("\\"));
	SetCurrentDirectoryA(exe.c_str());

    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
}

#pragma region Frame Update
// Executes basic render loop.
void Sample::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Sample::Update(DX::StepTimer const&)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        if (pad.IsViewPressed())
        {
            ExitSample();
        }
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (kb.Escape)
    {
        ExitSample();
    }

    auto mouse = m_mouse->GetState();
    mouse;

    if (mouse.leftButton)
    {
		// This should only return the current game package
		XPackageEnumeratePackages(XPackageKind::Game, XPackageEnumerationScope::ThisOnly, nullptr, [](void*, const XPackageDetails* details)
			{
				std::string productId = details->storeId;
				std::string pdpURI = "ms-windows-store://pdp/?ProductId=" + productId;

				XLaunchUri(nullptr, pdpURI.c_str());

				return false;
			});
    }

    PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Sample::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    // Prepare the command list to render a new frame.
    m_deviceResources->Prepare();
    Clear();

    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

    m_spriteBatch->Begin(commandList);

	ID3D12DescriptorHeap* heap = m_resourceDescriptors->Heap();
	commandList->SetDescriptorHeaps(1, &heap);

    // Draw the logo
    m_spriteBatch->Draw(m_resourceDescriptors->GetGpuHandle(1), DirectX::XMUINT2{ 256, 256 }, RECT{ 192, 75, 448, 331 });

    //std::wstring message = GetLocalizedMessage();
    std::wstring message = L"Click to launch Microsoft Store";
    auto length = m_font->MeasureString(message.c_str());
    float width = XMVectorGetX(length);
    m_font->DrawString(m_spriteBatch.get(), message.c_str(), DirectX::XMFLOAT2{ 320.f - (width / 2.f), 360.f });

    m_spriteBatch->End();

    PIXEndEvent(commandList);

    // Show the new frame.
    PIXBeginEvent(m_deviceResources->GetCommandQueue(), PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());
    PIXEndEvent(m_deviceResources->GetCommandQueue());
}

// Helper method to clear the back buffers.
void Sample::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto rtvDescriptor = m_deviceResources->GetRenderTargetView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, nullptr);
    commandList->ClearRenderTargetView(rtvDescriptor, ATG::Colors::Background, 0, nullptr);

    // Set the viewport and scissor rect.
    auto viewport = m_deviceResources->GetScreenViewport();
    auto scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(commandList);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Sample::OnActivated()
{
}

void Sample::OnDeactivated()
{
}

void Sample::OnSuspending()
{
}

void Sample::OnResuming()
{
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
    m_keyboardButtons.Reset();
}

void Sample::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Sample::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

// Properties
void Sample::GetDefaultSize(int& width, int& height) const
{
    width = 640;
    height = 480;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());
    rtState;

    m_resourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(device,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        2);

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

	SpriteBatchPipelineStateDescription pd(rtState, &CommonStates::NonPremultiplied);
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(device, resourceUpload, pd);
    
    wchar_t buffer[MAX_PATH] = {};
    DX::FindMediaFile(buffer, MAX_PATH, L"SegoeUI_18.spritefont");

    m_font = std::make_unique<DirectX::SpriteFont>(device, resourceUpload, buffer, m_resourceDescriptors->GetCpuHandle(0), m_resourceDescriptors->GetGpuHandle(0));

    DX::FindMediaFile(buffer, MAX_PATH, L"Logo.png");
    DirectX::CreateWICTextureFromFile(device, resourceUpload, buffer, m_logo.ReleaseAndGetAddressOf());
    CreateShaderResourceView(device, m_logo.Get(), m_resourceDescriptors->GetCpuHandle(1));

    auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());
    uploadResourcesFinished.wait();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    m_spriteBatch->SetViewport(m_deviceResources->GetScreenViewport());
}

void Sample::OnDeviceLost()
{
    m_graphicsMemory.reset();
    m_spriteBatch.reset();
    m_font.reset();
    m_logo.Reset();
}

void Sample::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

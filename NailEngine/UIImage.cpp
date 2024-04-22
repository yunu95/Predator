#include "UIImage.h"
#include "RenderSystem.h"
#include "ResourceBuilder.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Device.h"
#include "Texture.h"
#include "IYunuGITexture.h"
#include "SwapChain.h"
#include "NailEngine.h"

void UIImage::SetTexture(yunuGI::ITexture* texture)
{
    this->texture = texture;
    if (width == -1 && height == -1)
    {
        SetWidth(texture->GetWidth());
        SetHeight(texture->GetHeight());
    }
    processedTexture.ReleaseAndGetAddressOf();
    processedTexture = nullptr;
}

yunuGI::ITexture* UIImage::GetTexture()
{
    return this->texture;
}
ID3D11Texture2D* UIImage::GetProcessedTexture()
{
    return processedTexture.Get();
}
ID3D11ShaderResourceView* UIImage::GetProcessedTextureSRV()
{
    return processedTextureSRV.Get();
}

float UIImage::GetWidth()
{
    return width;
};

float UIImage::GetHeight()
{
    return height;
};
float UIImage::GetXPivot()
{
    return xPivot;
};
float UIImage::GetYPivot()
{
    return yPivot;
};
void UIImage::SetXPivot(float xPivot)
{
    this->xPivot = xPivot;
}
void UIImage::SetYPivot(float yPivot)
{
    this->yPivot = yPivot;
}
void UIImage::SetWidth(float width)
{
    this->width = width;
}
void UIImage::SetHeight(float height)
{
    this->height = height;
};
bool UIImage::operator<(const UIImage& other) const
{
    return layer < other.layer;
}

DirectX::FXMVECTOR UIImage::GetColor()
{
    return color;
}
void UIImage::SetColor(const DirectX::FXMVECTOR& color)
{
    DirectX::XMStoreFloat4((DirectX::XMFLOAT4*)&this->color, color);
}

bool UIImage::IsRadialFillMode()
{
    return isRadialFillMode;
}
bool UIImage::IsUsingProcessedTexture()
{
    return isRadialFillMode;
}
int UIImage::GetLayer()
{
    return layer;
}
void UIImage::SetRadialFillMode(bool fill)
{
    isRadialFillMode = fill;
    if (isRadialFillMode == true)
    {
        RenderSystem::Instance.Get().PushPreProcessingUIObject(this);
    }
}
void UIImage::SetRadialFillDegree(float degree)
{
    if (radialFillDegree != degree)
    {
        RenderSystem::Instance.Get().PushPreProcessingUIObject(this);
    }
    radialFillDegree = degree;
}
void UIImage::SetRadialFillStartPoint(float x, float y)
{
    if (radialFillStartDirection.x != x || radialFillStartDirection.y != y)
    {
        RenderSystem::Instance.Get().PushPreProcessingUIObject(this);
    }
    radialFillStartDirection = { x,y };
}
// 시계방향으로 채우는가?
void UIImage::SetRadialFillDirection(bool isClockwise)
{
    if (radialFillIsClockwise != isClockwise)
    {
        RenderSystem::Instance.Get().PushPreProcessingUIObject(this);
    }
    radialFillIsClockwise = isClockwise;
}
void UIImage::PreProcessTexture()
{
    if (processedTexture.Get() == nullptr)
    {
        D3D11_TEXTURE2D_DESC desc
        {
             .Width = static_cast<unsigned int>(texture->GetWidth()),
             .Height = static_cast<unsigned int>(texture->GetHeight()),
             .MipLevels = 1,
             .ArraySize = 1,
             .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
             .SampleDesc = {1, 0},
             .Usage = D3D11_USAGE_DEFAULT,
             .BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
             .CPUAccessFlags = 0,
             .MiscFlags = 0
        };
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
        {
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D
            {
                .MostDetailedMip = 0,
                .MipLevels = 1,
            }
        };
        ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&desc, nullptr, processedTexture.GetAddressOf());
        ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(processedTexture.Get(), nullptr, processedTextureRTV.GetAddressOf());
        ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(processedTexture.Get(), &srvDesc, processedTextureSRV.GetAddressOf());
    }
    if (isRadialFillMode)
    {
        static Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        DirectX::SimpleMath::Vector2 clippingDirection{ 1,1 };
        float rotatingDirection = radialFillIsClockwise ? -1 : 1;
        DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationZ(rotatingDirection * 0.5 * radialFillDegree * DirectX::XM_PI / 180.0f);
        radialFillStartDirection.x = 0;
        radialFillStartDirection.y = 1;
        clippingDirection = DirectX::SimpleMath::Vector2::Transform(radialFillStartDirection, rotationMatrix);
        clippingDirection.Normalize();
        float clippingThreshold{ cosf(0.5 * radialFillDegree * DirectX::XM_PI / 180.0f) };
        UIRadialFillVertex vertices[6]
        {
            {
                .pos = DirectX::SimpleMath::Vector3{-1,-1,0},
                .uv = DirectX::SimpleMath::Vector2{0,1},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            },
            {
                .pos = DirectX::SimpleMath::Vector3{-1,1,0},
                .uv = DirectX::SimpleMath::Vector2{0,0},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            },
            {
                .pos = DirectX::SimpleMath::Vector3{1,1,0},
                .uv = DirectX::SimpleMath::Vector2{1,0},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            },
            {
                .pos = DirectX::SimpleMath::Vector3{-1,-1,0},
                .uv = DirectX::SimpleMath::Vector2{0,1},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            },
            {
                .pos = DirectX::SimpleMath::Vector3{1,1,0},
                .uv = DirectX::SimpleMath::Vector2{1,0},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            },
            {
                .pos = DirectX::SimpleMath::Vector3{1,-1,0},
                .uv = DirectX::SimpleMath::Vector2{1,1},
                .clippingDirection = clippingDirection,
                .clippingThreshold = clippingThreshold
            }
        };
        D3D11_BUFFER_DESC vBufferDesc =
        {
            .ByteWidth = sizeof(vertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0,
        };
        D3D11_SUBRESOURCE_DATA initData = { .pSysMem = vertices };
        auto hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateBuffer(&vBufferDesc, &initData, vertexBuffer.GetAddressOf());
        assert(SUCCEEDED(hr));

        auto vs = std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"UIRadialFillVS.cso"));
        auto ps = std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"UIRadialFillPS.cso"));
        ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, processedTextureRTV.GetAddressOf(), nullptr);
        ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearRenderTargetView(processedTextureRTV.Get(), DirectX::Colors::Transparent);
        static_cast<Texture*>(texture)->Bind(0);

        vs->Bind();
        ps->Bind();
        const unsigned int vertexStride = sizeof(UIRadialFillVertex);
        const unsigned int vertexOffset = 0;
        D3D11_VIEWPORT viewport
        {
             .TopLeftX = 0.0f,
             .TopLeftY = 0.0f,
             .Width = texture->GetWidth(),
             .Height = texture->GetHeight(),
             .MinDepth = 0.0f,
             .MaxDepth = 1.0f,
        };
        ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);
        ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset);
        ResourceBuilder::Instance.Get().device->GetDeviceContext()->Draw(6, 0);




    }
}

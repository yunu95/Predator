#pragma once

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>

class Texture;
class VertexShader;
class PixelShader;
class NailEngine;

class ShadowPass
{
public:
    static LazyObjects<ShadowPass> Instance;
    friend LazyObjects<ShadowPass>;

public:
    void Init(Texture* dsTexture, VertexShader* vs, PixelShader* ps, VertexShader* skinnedVS);
    void Bind();
    void SkinnedBind();

private:
    void SetTempRTV(Texture* texture);
    Texture* dsTexture;
    VertexShader* vs;
    VertexShader* skinnedVS;
    PixelShader* ps;
    Texture* tempRTV;

    friend NailEngine;
};


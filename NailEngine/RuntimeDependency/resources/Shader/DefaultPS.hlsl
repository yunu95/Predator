#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 main(PixelIn input) : SV_TARGET
{
    float4 ambient = lights[0].color.ambient;
    float4 diffuse = float4(0.f, 0.f, 0.f, 0.f);
    float4 spec = float4(0.f, 0.f, 0.f, 0.f);

    float3 lightVec = normalize(-lights[0].direction);

    float diffuseFactor = dot(lightVec, input.normal);

	
    if (diffuseFactor > 0.f)
    {
        //float3 reflection = normalize(2 * diffuseFactor * input.normal - lightVec);
        float3 reflection = normalize(reflect(-lightVec, input.normal));

        float specFactor = pow(max(dot(reflection, input.viewDirection), 0.f), 32.f);
        
        diffuse = diffuseFactor * lights[0].color.diffuse;
        spec = specFactor * lights[0].color.specular;
    }
    
    float4 retColor = materialColor * (ambient + diffuse) + spec;
    
    return retColor;
    
    //float4 albedo;
    //float4 lightColor;
    //float4 ambientColor;
    
    //float3 lightDir = -lights[0].direction;
    //float intensity = saturate(dot(input.normal, lightDir));
    
    //lightColor = saturate(lights[0].color.diffuse * intensity);
    //ambientColor = lights[0].color.ambient;
    
    //if (useAlbedo)
    //{
    //    albedo = AlbedoMap.Sample(sam, input.uv);
        
    //    if (intensity > 0.f)
    //    {
    //        ambientColor += (lightColor * intensity);
    //        ambientColor = saturate(ambientColor);
    //    }
        
    //    return albedo * ambientColor;
    //}
    //else
    //{
    //    return input.color * materialColor * lightColor;
    //}

    //LightColor totalColor = (LightColor) 0.f;

    ////for (int i = 0; i < lightCount; ++i)
    ////{
    ////    LightColor color = CalculateLight(i, input.normal, input.pos.xyz);
    ////    totalColor.diffuse += color.diffuse;
    ////    totalColor.ambient += color.ambient;
    ////    totalColor.specular += color.specular;
    ////}

    ////color1.xyz = (totalColor.diffuse.xyz * color1.xyz)
    ////    + totalColor.ambient.xyz * color1.xyz
    ////    + totalColor.specular.xyz;
}

// ShaderInfo
// ShaderType : Solid

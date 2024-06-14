#pragma once
#define SAFE_RELEASE(x) if(x) { (x)->Release(); x = nullptr; }
#include <string>
#include <cassert>

// ----------------------------------
// MF API usage copied from mediafoundationsamples\MFMP4ToYUVWithoutMFT
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>
#include <unordered_set>
#include "Functors.h"
#include "Texture.h"
#include "IVideo.h"

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

#define CHECK_HR(x, msg) hr = x; if( hr != S_OK ) { dbg(msg); return false; }

//#define SHOW_DEBUG        1
#if SHOW_DEBUG
static void dbg(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    char dest[1024 * 4];
    int n = _vsnprintf(dest, sizeof(dest), format, argptr);
    assert(n < sizeof(dest));
    dest[n] = 0x00;
    va_end(argptr);
    ::OutputDebugString(dest);
}
#else
#define dbg(...)
#endif


// ---------------------------------------------------------------------------
#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif

LPCSTR GetGUIDNameConst(const GUID& guid);

#define CHECKHR_GOTO(x,y) if(x != S_OK) goto y;

// Returns all attributes from the stream in string format
std::string GetMediaTypeDescription(IMFMediaType* pMediaType);

// -------------------------------------------------------------------
class Video :public yunuGI::IVideo, public Resource
{
public:
    static std::unordered_set<Video*> renderList;
    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* uSource = NULL;
    IMFMediaSource* mediaFileSource = NULL;
    IMFAttributes* pVideoReaderAttributes = NULL;
    IMFSourceReader* pSourceReader = NULL;
    IMFMediaType* pReaderOutputType = NULL, * pFirstOutputType = NULL;
    MF_OBJECT_TYPE     ObjectType = MF_OBJECT_INVALID;

    // Start processing frames.
    IMFSample* pVideoSample = NULL;
    DWORD      streamIndex = 0, flags = 0;
    LONGLONG   llVideoTimeStamp = 0, llSampleDuration = 0;
    int        sampleCount = 0;
    DWORD      sampleFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> videoIYUVTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> videoIYUVTextureSRV;
    std::shared_ptr<Texture> videoTexture;
    uint32_t  videoTextureXres{ 0 };
    uint32_t  videoTextureYres{ 0 };
    uint32_t  width = 0;
    uint32_t  height = 0;
    float     fps = 0.0f;

    float     clock_time = 0.0f;
    LONGLONG  video_time = 0;
    bool      finished = false;
    bool      paused = false;
    //bool      autoloop = true;

    bool readOutputMediaFormat();
    bool createVideoTexture(uint32_t new_xres, uint32_t new_yres, DXGI_FORMAT new_format, bool is_dynamic);
    ~Video();
    bool open(const wchar_t* filename);
    virtual float GetWidth() override;
    virtual float GetHeight() override;
    virtual bool IsDonePlaying();
    //virtual float GetDuration() override;
    virtual void update(float deltaTime, bool autoloop)override;
    virtual void ResetVideo() override;
    //virtual void SetRepeat(bool repeat) override;
    bool updateTextureWithIYUV(const uint8_t* data, size_t data_size);
    bool renderToLocalTexture();
};

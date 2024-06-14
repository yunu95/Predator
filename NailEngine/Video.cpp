#include "Video.h"
#include "ResourceBuilder.h"
#include "NailEngine.h"
#include "Device.h"
#include "Shader.h" 
#include "VertexShader.h" 
#include "PixelShader.h" 
#define _CRT_SECURE_NO_WARNINGS

LPCSTR GetGUIDNameConst(const GUID& guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

    return NULL;
}
// Returns all attributes from the stream in string format
std::string GetMediaTypeDescription(IMFMediaType* pMediaType)
{
    HRESULT hr = S_OK;
    GUID MajorType;
    UINT32 cAttrCount;
    LPCSTR pszGuidStr;
    std::string description;
    WCHAR TempBuf[200];

    if (pMediaType == NULL)
    {
        description = "<NULL>";
        goto done;
    }

    hr = pMediaType->GetMajorType(&MajorType);
    CHECKHR_GOTO(hr, done);

    //pszGuidStr = STRING_FROM_GUID(MajorType);
    pszGuidStr = GetGUIDNameConst(MajorType);
    if (pszGuidStr != NULL)
    {
        description += pszGuidStr;
        description += ": ";
    }
    else
    {
        description += "Other: ";
    }

    hr = pMediaType->GetCount(&cAttrCount);
    CHECKHR_GOTO(hr, done);

    for (UINT32 i = 0; i < cAttrCount; i++)
    {
        GUID guidId;
        MF_ATTRIBUTE_TYPE attrType;

        hr = pMediaType->GetItemByIndex(i, &guidId, NULL);
        CHECKHR_GOTO(hr, done);

        hr = pMediaType->GetItemType(guidId, &attrType);
        CHECKHR_GOTO(hr, done);

        pszGuidStr = GetGUIDNameConst(guidId);
        if (pszGuidStr != NULL)
        {
            description += pszGuidStr;
        }
        else
        {
            LPOLESTR guidStr = NULL;
            // GUID's won't have wide chars.
            CHECKHR_GOTO(StringFromCLSID(guidId, &guidStr), done);
            char str[128];
            //MultiByteToWideChar(CP_UTF8, 0, filePath, -1, &wFilePath[0], bufferSize);
            WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, str, sizeof(str), NULL, NULL);
            //wcstombs(str, guidStr, sizeof(str));
            description += std::string(str);

            CoTaskMemFree(guidStr);
        }

        description += "=";

        switch (attrType)
        {
        case MF_ATTRIBUTE_UINT32:
        {
            UINT32 Val;
            hr = pMediaType->GetUINT32(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            description += std::to_string(Val);
            break;
        }
        case MF_ATTRIBUTE_UINT64:
        {
            UINT64 Val;
            hr = pMediaType->GetUINT64(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            if (guidId == MF_MT_FRAME_SIZE)
            {
                description += "W:" + std::to_string(HI32(Val)) + " H: " + std::to_string(LO32(Val));
            }
            else if (guidId == MF_MT_FRAME_RATE)
            {
                // Frame rate is numerator/denominator.
                description += std::to_string(HI32(Val)) + "/" + std::to_string(LO32(Val));
            }
            else if (guidId == MF_MT_PIXEL_ASPECT_RATIO)
            {
                description += std::to_string(HI32(Val)) + ":" + std::to_string(LO32(Val));
            }
            else
            {
                //tempStr.Format("%ld", Val);
                description += std::to_string(Val);
            }

            //description += tempStr;

            break;
        }
        case MF_ATTRIBUTE_DOUBLE:
        {
            DOUBLE Val;
            hr = pMediaType->GetDouble(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            //tempStr.Format("%f", Val);
            description += std::to_string(Val);
            break;
        }
        case MF_ATTRIBUTE_GUID:
        {
            GUID Val;
            const char* pValStr;

            hr = pMediaType->GetGUID(guidId, &Val);
            CHECKHR_GOTO(hr, done);

            pValStr = GetGUIDNameConst(Val);
            if (pValStr != NULL)
            {
                description += pValStr;
            }
            else
            {
                LPOLESTR guidStr = NULL;
                CHECKHR_GOTO(StringFromCLSID(Val, &guidStr), done);
                char str[128];
                //wcstombs(str, guidStr, sizeof(str));
                WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, str, sizeof(str), NULL, NULL);
                description += std::string(str);

                CoTaskMemFree(guidStr);
            }

            break;
        }
        case MF_ATTRIBUTE_STRING:
        {
            hr = pMediaType->GetString(guidId, TempBuf, sizeof(TempBuf) / sizeof(TempBuf[0]), NULL);
            if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            {
                description += "<Too Long>";
                break;
            }
            CHECKHR_GOTO(hr, done);
            char str[200];
            WideCharToMultiByte(CP_UTF8, 0, TempBuf, -1, str, sizeof(str), NULL, NULL);
            description += std::string(str);

            break;
        }
        case MF_ATTRIBUTE_BLOB:
        {
            description += "<BLOB>";
            break;
        }
        case MF_ATTRIBUTE_IUNKNOWN:
        {
            description += "<UNK>";
            break;
        }
        }

        description += ", ";
    }

done:
    return description;
}

std::unordered_set<Video*> Video::renderList;
bool Video::readOutputMediaFormat() {
    assert(pSourceReader);
    HRESULT hr;

    IMFMediaType* pNativeType = nullptr;
    CHECK_HR(pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &pNativeType),
        "Error retrieving GetNativeMediaType.");

    SAFE_RELEASE(pFirstOutputType);
    CHECK_HR(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFirstOutputType), "Error reading GetCurrentMediaType");

    // The sample uses the pNativeType here, but we want the resolution required to 'render' the texture as if the height is not x16 the conversion
    // YUV to RGB has problems
    CHECK_HR(MFGetAttributeSize(pFirstOutputType, MF_MT_FRAME_SIZE, &width, &height), "Read input resolution");

    // Read fps
    uint64_t Val;
    hr = pNativeType->GetUINT64(MF_MT_FRAME_RATE, &Val);
    fps = (float)HI32(Val) / (float)LO32(Val);

    SAFE_RELEASE(pNativeType);

    dbg("Source reader output media type: %dx%d %s (@ %f fps)\n", width, height, GetMediaTypeDescription(pFirstOutputType).c_str(), fps);
    return true;
}
bool Video::createVideoTexture(uint32_t new_xres, uint32_t new_yres, DXGI_FORMAT new_format, bool is_dynamic) {
    videoTextureXres = new_xres;
    videoTextureYres = new_yres;
    //format = new_format;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = new_xres;
    desc.Height = new_yres;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = new_format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    if (is_dynamic) {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    ID3D11Texture2D* tex2d = nullptr;
    auto device = ResourceBuilder::Instance.Get().device->GetDevice();
    HRESULT hr = device->CreateTexture2D(&desc, nullptr, videoIYUVTexture.GetAddressOf());
    if (FAILED(hr))
        return false;

    // Create a resource view so we can use the data in a shader
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = new_format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = desc.MipLevels;
    hr = device->CreateShaderResourceView(videoIYUVTexture.Get(), &srv_desc, videoIYUVTextureSRV.GetAddressOf());
    if (FAILED(hr))
        return false;

    return true;
}


Video::~Video() {
    SAFE_RELEASE(pSourceResolver);
    SAFE_RELEASE(uSource);
    SAFE_RELEASE(mediaFileSource);
    SAFE_RELEASE(pVideoReaderAttributes);
    SAFE_RELEASE(pSourceReader);
    SAFE_RELEASE(pReaderOutputType);
    SAFE_RELEASE(pFirstOutputType);
}

bool Video::open(const wchar_t* filename) {
    finished = false;

    std::wstring wfilename{ filename };
    //mbstowcs(wfilename, filename, sizeof(wfilename) / sizeof(wchar_t));
    HRESULT hr = S_OK;

    // Need the color converter DSP for conversions between YUV, RGB etc.
    // Lots of examples use this explicit colour converter load. Seems
    // like most cases it gets loaded automatically.
   /* CHECK_HR(MFTRegisterLocalByCLSID(
      __uuidof(CColorConvertDMO),
      MFT_CATEGORY_VIDEO_PROCESSOR,
      L"",
      MFT_ENUM_FLAG_SYNCMFT,
      0,
      NULL,
      0,
      NULL),
      "Error registering colour converter DSP.");*/

      // Set up the reader for the file.
    CHECK_HR(MFCreateSourceResolver(&pSourceResolver),
        "MFCreateSourceResolver failed.");

    CHECK_HR(pSourceResolver->CreateObjectFromURL(
        wfilename.c_str(),		        // URL of the source.
        MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
        NULL,                       // Optional property store.
        &ObjectType,				        // Receives the created object type. 
        &uSource					          // Receives a pointer to the media source.
    ),
        "Failed to create media source resolver for file.");

    CHECK_HR(uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource)),
        "Failed to create media file source.");

    CHECK_HR(MFCreateAttributes(&pVideoReaderAttributes, 2),
        "Failed to create attributes object for video reader.");

    CHECK_HR(pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID),
        "Failed to set dev source attribute type for reader config.");

    CHECK_HR(pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
        "Failed to set enable video processing attribute type for reader config.");

    CHECK_HR(pVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),
        "Failed to set media sub type on source reader output media type.");

    CHECK_HR(MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader),
        "Error creating media source reader.");

    CHECK_HR(MFCreateMediaType(&pReaderOutputType), "Failed to create source reader output media type.");
    CHECK_HR(pReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), "Failed to set major type on source reader output media type.");
    CHECK_HR(pReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV), "Failed to set media sub type on source reader output media type.");

    CHECK_HR(pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pReaderOutputType),
        "Failed to set output media type on source reader.");

    if (!readOutputMediaFormat())
        return false;

    CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE), "Enable Video Stream");
    //CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, FALSE), "Disable Audio Stream");

    clock_time = 0.0f;

    // Just to ensure we have something as the first frame, and also ensure we have the real required size (height % 16 should be 0)
    //update(0.0f, false);

    return true;
}
float Video::GetWidth()
{
    return width;
}
float Video::GetHeight()
{
    return height;
}
bool Video::IsDonePlaying()
{
    return finished;
}
//float Video::GetDuration()
//{
//
//}
void Video::update(float deltaTime, bool autoloop)
{

    if (paused)
        return;

    if (finished)
        return;

    clock_time += deltaTime;

    LONGLONG uct = (LONGLONG)(clock_time * 10000000);
    if (uct < video_time)
        return;

    assert(pSourceReader);
    flags = 0;

    HRESULT hr;
    hr = pSourceReader->ReadSample(
        MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0,                              // Flags.
        &streamIndex,                   // Receives the actual stream index. 
        &flags,                         // Receives status flags.
        &llVideoTimeStamp,              // Receives the time stamp.
        &pVideoSample                   // Receives the sample or NULL.
    );

    if (!SUCCEEDED(hr)) {
        finished = true;
        return;
    }

    video_time = llVideoTimeStamp;

    if (flags & MF_SOURCE_READERF_STREAMTICK)
    {
        dbg("\tStream tick.\n");
    }
    if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        dbg("\tEnd of stream.\n");
        if (autoloop) {
            PROPVARIANT var = { 0 };
            var.vt = VT_I8;
            hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
            if (hr != S_OK) { dbg("Failed to set source reader position."); }
            clock_time = 0.0f;
        }
        else
            finished = true;
    }
    if (flags & MF_SOURCE_READERF_NEWSTREAM)
    {
        dbg("\tNew stream.\n");
        finished = true;
    }
    if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
    {
        dbg("\tNative type changed.\n");
        finished = true;
    }
    if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
    {
        dbg("\tCurrent type changed: from %dx%d %s (@ %f fps)\n", width, height, GetMediaTypeDescription(pFirstOutputType).c_str(), fps);
        if (!readOutputMediaFormat()) {
            finished = true;
            return;
        }
    }

    if (pVideoSample) {
        hr = pVideoSample->SetSampleTime(llVideoTimeStamp);
        assert(SUCCEEDED(hr));

        hr = pVideoSample->GetSampleDuration(&llSampleDuration);
        assert(SUCCEEDED(hr));

        hr = pVideoSample->GetSampleFlags(&sampleFlags);
        assert(SUCCEEDED(hr));

        // 
        IMFMediaBuffer* buf = NULL;
        DWORD bufLength;

        hr = pVideoSample->ConvertToContiguousBuffer(&buf);
        hr = buf->GetCurrentLength(&bufLength);

        byte* byteBuffer = NULL;
        DWORD buffMaxLen = 0, buffCurrLen = 0;
        hr = buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
        assert(SUCCEEDED(hr));

        // Some videos report one resolution and after the first frame change the height to the next multiple of 16 (using the event MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        if (!videoIYUVTexture) {
            int texture_height = height * 2;
            videoTexture = std::make_shared<Texture>();
            videoTexture->CreateTexture(L"", width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE));
            createVideoTexture(width, texture_height, DXGI_FORMAT_R8_UNORM, true);
        }

        //Clock clock;
        //static bool once = true;
        //if (once)
        //{
        //    target_texture->updateFromIYUV(byteBuffer, buffCurrLen);
        //    once = false;
        //}
        updateTextureWithIYUV(byteBuffer, buffCurrLen);
        //float deltaTime = clock.deltaTime();

        dbg("Sample count %d, Sample flags %d, sample duration %I64d, sample time %I64d. CT:%I64d. Buffer: %ld/%ld. Elapsed:%f\n", sampleCount, sampleFlags, llSampleDuration, llVideoTimeStamp, uct, buffCurrLen, buffMaxLen, deltaTime);

        hr = buf->Unlock();
        assert(SUCCEEDED(hr));

        SAFE_RELEASE(buf);
        sampleCount++;

        SAFE_RELEASE(pVideoSample);
    }
}
void Video::ResetVideo()
{
    dbg("\tEnd of stream.\n");
    PROPVARIANT var = { 0 };
    var.vt = VT_I8;
    auto hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
    if (hr != S_OK) { dbg("Failed to set source reader position."); }
    clock_time = 0.0f;
}
//void Video::SetRepeat(bool repeat)
//{
//    autoloop = repeat;
//}
bool Video::updateTextureWithIYUV(const uint8_t* data, size_t data_size) {
    assert(data);
    D3D11_MAPPED_SUBRESOURCE ms;
    auto ctx = ResourceBuilder::Instance.Get().device->GetDeviceContext();
    HRESULT hr = ctx->Map(videoIYUVTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    if (FAILED(hr))
        return false;

    uint32_t bytes_per_texel = 1;
    /*assert(format == DXGI_FORMAT_R8_UNORM);
    assert(data_size == xres * yres * 3 / 4);*/

    const uint8_t* src = data;
    uint8_t* dst = (uint8_t*)ms.pData;

    // Copy the Y lines
    uint32_t nlines = videoTextureYres / 2;
    uint32_t bytes_per_row = videoTextureXres * bytes_per_texel;
    for (uint32_t y = 0; y < nlines; ++y) {
        memcpy(dst, src, bytes_per_row);
        src += bytes_per_row;
        dst += ms.RowPitch;
    }

    // Now the U and V lines, need to add Width/2 pixels of padding between each line
    uint32_t uv_bytes_per_row = bytes_per_row / 2;
    for (uint32_t y = 0; y < nlines; ++y) {
        memcpy(dst, src, uv_bytes_per_row);
        src += uv_bytes_per_row;
        dst += ms.RowPitch;
    }

    ctx->Unmap(videoIYUVTexture.Get(), 0);
    renderList.insert(this);
    return true;
}
bool Video::renderToLocalTexture()
{
    if (videoTexture.get() == nullptr)
        return false;

    static Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

    // 버텍스 버퍼가 없으면 버텍스 버퍼부터 만든다.
    if (!vertexBuffer.Get())
    {
        VideoYUVtoRGBVertex vertices[6]
        {
            {
                .pos = DirectX::SimpleMath::Vector4{-1,-1,0,1},
                .uv = DirectX::SimpleMath::Vector2{0,1},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
            },
            {
                .pos = DirectX::SimpleMath::Vector4{-1,1,0,1},
                .uv = DirectX::SimpleMath::Vector2{0,0},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
            },
            {
                .pos = DirectX::SimpleMath::Vector4{1,1,0,1},
                .uv = DirectX::SimpleMath::Vector2{1,0},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
            },
            {
                .pos = DirectX::SimpleMath::Vector4{-1,-1,0,1},
                .uv = DirectX::SimpleMath::Vector2{0,1},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
            },
            {
                .pos = DirectX::SimpleMath::Vector4{1,1,0,1},
                .uv = DirectX::SimpleMath::Vector2{1,0},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
            },
            {
                .pos = DirectX::SimpleMath::Vector4{1,-1,0,1},
                .uv = DirectX::SimpleMath::Vector2{1,1},
                .color = DirectX::SimpleMath::Vector4{1,1,1,1}
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
    }
    auto vs = std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"VideoVS.cso"));
    auto ps = std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"VideoPS.cso"));
    vs->Bind();
    ps->Bind();
    const unsigned int vertexStride = sizeof(VideoYUVtoRGBVertex);
    const unsigned int vertexOffset = 0;
    D3D11_VIEWPORT viewport
    {
         .TopLeftX = 0.0f,
         .TopLeftY = 0.0f,
         .Width = videoTexture->GetWidth(),
         .Height = videoTexture->GetHeight(),
         .MinDepth = 0.0f,
         .MaxDepth = 1.0f,
    };
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, videoTexture->GetRTV().GetAddressOf(), nullptr);
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(0, 1, videoIYUVTextureSRV.GetAddressOf());
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &vertexStride, &vertexOffset);
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->Draw(6, 0);
    return true;
}

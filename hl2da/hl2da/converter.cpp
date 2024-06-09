
#include <mfapi.h>
#include "neon.h"
#include "log.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>

#include <windows.graphics.imaging.h>
#include <windows.graphics.imaging.interop.h>

using namespace winrt::Windows::Graphics::Imaging;

class IMF2DBuffer2Wrapper : public IMFMediaBuffer, public IMF2DBuffer2
{
private:
    ULONG m_nRefCount;

    uint8_t* data;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t size;

    IMF2DBuffer2Wrapper(uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch, uint32_t size);

public:
    static HRESULT Create(uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch, uint32_t size, IMF2DBuffer2Wrapper** pBuf);

    HRESULT QueryInterface(REFIID iid, void** ppv);
    ULONG AddRef();
    ULONG Release();

    HRESULT GetCurrentLength(DWORD* pcbCurrentLength);
    HRESULT GetMaxLength(DWORD* pcbMaxLength);
    HRESULT Lock(BYTE** ppbBuffer, DWORD* pcbMaxLength, DWORD* pcbCurrentLength);
    HRESULT SetCurrentLength(DWORD cbCurrentLength);
    HRESULT Unlock();

    HRESULT ContiguousCopyFrom(const BYTE* pbSrcBuffer, DWORD cbSrcBuffer);
    HRESULT ContiguousCopyTo(BYTE* pbDestBuffer, DWORD cbDestBuffer);
    HRESULT GetContiguousLength(DWORD* pcbLength);
    HRESULT GetScanline0AndPitch(BYTE** pbScanline0, LONG* plPitch);
    HRESULT IsContiguousFormat(BOOL* pfIsContiguous);
    HRESULT Lock2D(BYTE** ppbScanline0, LONG* plPitch);
    HRESULT Unlock2D();

    HRESULT Copy2DTo(IMF2DBuffer2* pDestBuffer);
    HRESULT Lock2DSize(MF2DBuffer_LockFlags lockFlags, BYTE** ppbScanline0, LONG* plPitch, BYTE** ppbBufferStart, DWORD* pcbBufferLength);
};

class bitmap_container
{
public:
    SoftwareBitmap bmp;
    BitmapBuffer buf = nullptr;
    winrt::Windows::Foundation::IMemoryBufferReference ref;

    bitmap_container(SoftwareBitmap const& bitmap);
    ~bitmap_container();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static ISoftwareBitmapNativeFactory* g_sbnf; // Release

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
IMF2DBuffer2Wrapper::IMF2DBuffer2Wrapper(uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch, uint32_t size) : m_nRefCount(1), data(data), width(width), height(height), pitch(pitch), size(size)
{

}

// OK
HRESULT IMF2DBuffer2Wrapper::Create(uint8_t* data, uint32_t width, uint32_t height, uint32_t pitch, uint32_t size, IMF2DBuffer2Wrapper** pBuf)
{
    if (!pBuf) { return E_POINTER; }
    IMF2DBuffer2Wrapper* b = new (std::nothrow) IMF2DBuffer2Wrapper(data, width, height, pitch, size);
    if (!b) { return E_OUTOFMEMORY; }
    *pBuf = b;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv) { return E_POINTER; }

    *ppv = NULL;

    if      (iid == IID_IUnknown)       { *ppv = this;                               } // Unused
    else if (iid == IID_IMFMediaBuffer) { *ppv = static_cast<IMFMediaBuffer*>(this); }
    else if (iid == IID_IMF2DBuffer)    { *ppv = static_cast<IMF2DBuffer*>(this);    } // Unused
    else if (iid == IID_IMF2DBuffer2)   { *ppv = static_cast<IMF2DBuffer2*>(this);   }
    else                                { return E_NOINTERFACE; }

    AddRef();
    return S_OK;
}

// OK
ULONG IMF2DBuffer2Wrapper::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

// OK
ULONG IMF2DBuffer2Wrapper::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0) { delete this; }
    return uCount;
}

// OK
HRESULT IMF2DBuffer2Wrapper::GetCurrentLength(DWORD* pcbCurrentLength)
{
    *pcbCurrentLength = size;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::GetMaxLength(DWORD* pcbMaxLength)
{
    *pcbMaxLength = size;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Lock(BYTE** ppbBuffer, DWORD* pcbMaxLength, DWORD* pcbCurrentLength)
{
    *ppbBuffer = data;
    *pcbMaxLength = size;
    *pcbCurrentLength = size;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::SetCurrentLength(DWORD cbCurrentLength)
{
    // NOP
    (void)cbCurrentLength;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Unlock()
{
    // NOP
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::ContiguousCopyFrom(const BYTE* pbSrcBuffer, DWORD cbSrcBuffer)
{
    // NOP
    (void)pbSrcBuffer;
    (void)cbSrcBuffer;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::ContiguousCopyTo(BYTE* pbDestBuffer, DWORD cbDestBuffer)
{
    memcpy(pbDestBuffer, data, cbDestBuffer);
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::GetContiguousLength(DWORD* pcbLength)
{
    *pcbLength = size;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::GetScanline0AndPitch(BYTE** pbScanline0, LONG* plPitch)
{
    *pbScanline0 = data;
    *plPitch = pitch;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::IsContiguousFormat(BOOL* pfIsContiguous)
{
    *pfIsContiguous = TRUE;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Lock2D(BYTE** ppbScanline0, LONG* plPitch)
{
    *ppbScanline0 = data;
    *plPitch = pitch;
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Unlock2D()
{
    // NOP
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Copy2DTo(IMF2DBuffer2* pDestBuffer)
{
    pDestBuffer->ContiguousCopyFrom(data, size);
    return S_OK;
}

// OK
HRESULT IMF2DBuffer2Wrapper::Lock2DSize(MF2DBuffer_LockFlags lockFlags, BYTE** ppbScanline0, LONG* plPitch, BYTE** ppbBufferStart, DWORD* pcbBufferLength)
{
    (void)lockFlags;
    *ppbScanline0 = data;
    *plPitch = pitch;
    *ppbBufferStart = data;
    *pcbBufferLength = size;
    return S_OK;
}

// OK
bitmap_container::bitmap_container(SoftwareBitmap const& sb) : bmp(sb)
{
    buf = bmp.LockBuffer(BitmapBufferAccessMode::Read);
    ref = buf.CreateReference();
}

// OK
bitmap_container::~bitmap_container()
{
    ref.Close();
    buf.Close();
    bmp.Close();
}

// OK
void Converter_Initialize()
{
    CoCreateInstance(CLSID_SoftwareBitmapNativeFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_sbnf));
}

// OK
void Converter_Cleanup()
{
    g_sbnf->Release();
}

// OK
void Converter_ZHTInvalidate(uint16_t const* depth_in, uint16_t* depth_out)
{
    Neon_ZHTInvalidate(depth_in, depth_out);
}

// OK
void Converter_ZLTInvalidate(uint8_t const* sigma_in, uint16_t const* depth_in, uint16_t* depth_out)
{
    Neon_ZLTInvalidate(sigma_in, depth_in, depth_out);
}

// OK
void Converter_YUV2RGB(uint8_t* image, uint32_t stride, uint32_t height, uint32_t format_in, uint32_t format_out, void** fc)
{
    uint32_t const pixels = stride * height;

    GUID subtype_in;
    uint32_t pitch;
    uint32_t size;
    BitmapPixelFormat subtype_out;
    IMF2DBuffer2Wrapper* buf; // Release
    ABI::Windows::Graphics::Imaging::ISoftwareBitmap* sb;

    switch (format_in)
    {
    case 57:  subtype_in = MFVideoFormat_L16;  pitch = stride * 2; size = (pixels * 2);     break;
    case 62:  subtype_in = MFVideoFormat_L8;   pitch = stride * 1; size = (pixels * 1);     break;
    case 103: subtype_in = MFVideoFormat_NV12; pitch = stride * 1; size = (pixels * 3) / 2; break;
    case 107: subtype_in = MFVideoFormat_YUY2; pitch = stride * 2; size = (pixels * 2);     break;
    default:  return;
    }

    switch (format_out)
    {
    case 30: subtype_out = BitmapPixelFormat::Rgba8; break;
    case 87: subtype_out = BitmapPixelFormat::Bgra8; break;
    default: return;
    }

    IMF2DBuffer2Wrapper::Create(image, stride, height, pitch, size, &buf);
    g_sbnf->CreateFromMF2DBuffer2(buf, subtype_in, stride, height, FALSE, NULL, IID_PPV_ARGS(&sb));
    *fc = new (std::nothrow) bitmap_container(SoftwareBitmap::Convert({ sb, winrt::take_ownership_from_abi }, subtype_out));

    buf->Release();
}

// OK
void Converter_Extract(void* fc, void const** buffer, int32_t* length)
{
    bitmap_container* f = (bitmap_container*)fc;

    *buffer = f->ref.data();
    *length = (int32_t)f->ref.Capacity();
}

// OK
void Converter_Release(void* fc)
{
    delete (bitmap_container*)fc;
}

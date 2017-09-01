#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

const uint32_t SCREEN_WIDTH = 960;
const uint32_t SCREEN_HEIGHT = 480;

// global declarations
IDXGISwapChain          *g_pSwapchain = nullptr;  // the pointer to the swap chain interface
ID3D11Device            *g_pDev = nullptr;  // the pointer to our Direct3D device interface
ID3D11DeviceContext     *g_pDevcon = nullptr;  // the pointer to our Direct3D device context

ID3D11RenderTargetView  *g_pRTView = nullptr;

ID3D11InputLayout       *g_pLayout = nullptr;  // the pointer to the input layout
ID3D11VertexShader      *g_pVS = nullptr;  // the pointer to the vertex shader
ID3D11PixelShader       *g_pPS = nullptr;        // the pointer to the pixel shader

ID3D11Buffer            *g_pVBuffer = nullptr;  // Vertex Buffer

struct VERTEX {
    XMFLOAT3 Position;
    XMFLOAT4 Color;
};

template<class T>
inline void SafeRelease(T **ppInterfaceToRelease) {
    if (*ppInterfaceToRelease != nullptr) {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = nullptr;
    }
}

void CreateRenderTarget() {
    HRESULT hr;
    ID3D11Texture2D *pBackBuffer;

    // Get a pointer to the back buffer
    g_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        (LPVOID*)&pBackBuffer);

    // Create a render-target view
    g_pDev->CreateRenderTargetView(pBackBuffer, NULL, &g_pRTView);
    pBackBuffer->Release();

    // Bind the view
    g_pDevcon->OMSetRenderTargets(1, &g_pRTView, NULL);

}

void SetViewPort() {
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    g_pDevcon->RSSetViewports(1, &viewport);
}

// this is the function that loads and prepares the shaders
void InitPipeline() {
    // load and compile the two shaders
    ID3DBlob *VS, *PS;

    D3DReadFileToBlob(L"copy.vso", &VS);
    D3DReadFileToBlob(L"copy.pso", &PS);

    // encapsulate both shaders into shader objects
    g_pDev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVS);
    g_pDev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPS);

    // set the shader objects
    g_pDevcon->VSSetShader(g_pVS, 0, 0);
    g_pDevcon->PSSetShader(g_pPS, 0, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    g_pDev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pLayout);
    g_pDevcon->IASetInputLayout(g_pLayout);

    VS->Release();
    PS->Release();
}
// this is the function that creates the shape to render
void InitGraphics() {
    VERTEX OurVertices[] = {
        { XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
    };

    
    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX)* 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    g_pDev->CreateBuffer(&bd, NULL, &g_pVBuffer);

    D3D11_MAPPED_SUBRESOURCE ms;
    g_pDevcon->Map(g_pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, OurVertices, sizeof(VERTEX)* 3);
    g_pDevcon->Unmap(g_pVBuffer, NULL);
}




HRESULT CreateGraphicsResources(HWND hWnd) {
    HRESULT hr = S_OK;

    if (g_pSwapchain == nullptr) {
        // create a struct to hold information about the swap chain
        DXGI_SWAP_CHAIN_DESC scd;

        // clear out the struct for use
        ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

        // fill the swap chain description struct
        scd.BufferCount = 1;                                    // one back buffer
        scd.BufferDesc.Width = SCREEN_WIDTH;
        scd.BufferDesc.Height = SCREEN_HEIGHT;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
        scd.OutputWindow = hWnd;                                // the window to be used
        scd.SampleDesc.Count = 4;                               // how many multi samples
        scd.Windowed = TRUE;                                    // windowed/full-screen mode
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching
        

    }

    return hr;
}

void DiscardGraphicsResources() {
    SafeRelease(&pRenderTarget);
    SafeRelease(&pLightSlateGrayBrush);
    SafeRelease(&pCornflowerBlueBrush);
}


LRESULT CALLBACK WindowProc(HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow) {
    HWND hWnd;
    WNDCLASSEX wc;

    // initialize COM
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return -1;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(0,
        _T("WindowClass1"), // name of the window class
        _T("Hello, Engine![Direct 2D]"),  // title of the window
        WS_OVERLAPPEDWINDOW,  // window style
        100,
        100,
        960,
        540,
        NULL,
        NULL,
        hInstance,
        NULL
        );
    ShowWindow(hWnd, nCmdShow);

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // uninitialize COM
    CoUninitialize();

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    bool wasHandled = false;
    switch (message) {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
            result = -1;
        }

        wasHandled = true;
        result = 1;
        break;

    case WM_PAINT:{
                      HRESULT hr = CreateGraphicsResources(hWnd);
                      if (SUCCEEDED(hr)) {
                          PAINTSTRUCT ps;
                          BeginPaint(hWnd, &ps);
                          pRenderTarget->BeginDraw();
                          pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

                          D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

                          int width = static_cast<int>(rtSize.width);
                          int height = static_cast<int>(rtSize.height);

                          // draw a grid background.
                          for (int x = 0; x < width; x += 10) {
                              pRenderTarget->DrawLine(
                                  D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                                  D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                                  pLightSlateGrayBrush,
                                  0.5f
                                  );
                          }

                          for (int y = 0; y < height; y += 10) {
                              pRenderTarget->DrawLine(
                                  D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                                  D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                                  pLightSlateGrayBrush,
                                  0.5f
                                  );
                          }


                          // draw two rectangles
                          D2D1_RECT_F rectangle1 = D2D1::RectF(
                              rtSize.width / 2 - 50.0f,
                              rtSize.height / 2 - 50.0f,
                              rtSize.width / 2 + 50.0f,
                              rtSize.height / 2 + 50.0f
                              );
                          D2D1_RECT_F rectangle2 = D2D1::RectF(
                              rtSize.width / 2 - 100.0f,
                              rtSize.height / 2 - 100.0f,
                              rtSize.width / 2 + 100.0f,
                              rtSize.height / 2 + 100.0f
                              );

                          pRenderTarget->FillRectangle(&rectangle1, pLightSlateGrayBrush);
                          pRenderTarget->DrawRectangle(rectangle2, pCornflowerBlueBrush);

                          hr = pRenderTarget->EndDraw();
                          if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {
                              DiscardGraphicsResources();
                          }
                          EndPaint(hWnd, &ps);
                      }
    }
        wasHandled = true;
        break;

    case WM_SIZE:
        if (pRenderTarget != nullptr) {
            RECT rc;
            GetClientRect(hWnd, &rc);

            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            pRenderTarget->Resize(size);
        }
        break;

    case WM_DESTROY:
        DiscardGraphicsResources();
        if (pFactory) {
            pFactory->Release();
            pFactory = nullptr;
        }

        PostQuitMessage(0);

        result = 1;
        wasHandled = true;
        break;

    case WM_DISPLAYCHANGE:
        InvalidateRect(hWnd, nullptr, false);
        wasHandled = true;

        break;
    }
    if (!wasHandled) { return DefWindowProc(hWnd, message, wParam, lParam); }
    return result;
}

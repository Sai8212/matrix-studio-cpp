#include <windows.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include "Matrix.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")

using namespace std;
using namespace Gdiplus;

// ================= Page & Control IDs =================

enum {
    PAGE_HOME = 0,
    PAGE_BASICS,
    PAGE_TYPES,
    PAGE_OPERATIONS,
    PAGE_CALCULATOR,
    PAGE_SETTINGS,
    PAGE_ABOUT,
    PAGE_COUNT
};

#define IDC_NAV_BASE          100   // 100..106 for the 7 nav buttons

#define IDC_CALC_EDIT_A       301
#define IDC_CALC_EDIT_B       302
#define IDC_CALC_EDIT_RESULT  303

#define IDC_CALC_BTN_ADD      401
#define IDC_CALC_BTN_MUL      402
#define IDC_CALC_BTN_TRANS    403
#define IDC_CALC_BTN_USE_A    404

#define IDC_A_PRESET_IDENT    410
#define IDC_A_PRESET_ZERO     411
#define IDC_A_PRESET_RANDOM   412

#define IDC_B_PRESET_IDENT    413
#define IDC_B_PRESET_ZERO     414
#define IDC_B_PRESET_RANDOM   415

#define IDC_HOME_GO_ADD       420
#define IDC_HOME_GO_MUL       421
#define IDC_HOME_GO_TRANS     422

#define IDC_SET_BTN_DARK      430
#define IDC_SET_BTN_LIGHT     431

#define IDC_INFO_EDIT         500

// ================= Educational Texts =================

static const char* HOME_TEXT =
    "======================================================================\r\n"
    "                 WELCOME TO MATRIX STUDIO (DESKTOP)                   \r\n"
    "======================================================================\r\n\r\n"
    "WHAT IS A MATRIX?\r\n"
    "  In simple terms, a matrix is numbers arranged neatly in rows and columns,\r\n"
    "  like a grid or table. Each number inside is called an 'element'.\r\n\r\n"
    "WHY DO MATRICES MATTER?\r\n"
    "  Matrices let us organize and compute with large sets of numbers at once,\r\n"
    "  instead of handling each value separately. This makes them indispensable\r\n"
    "  for modern high-performance computational systems.\r\n\r\n"
    "REAL-WORLD APPLICATIONS:\r\n"
    "  [+] Computer Graphics : Rotating, scaling, and translating 3D models & game worlds.\r\n"
    "  [+] Engineering       : Solving complex systems of circuit & structural equations.\r\n"
    "  [+] Physics           : Describing coordinate transformations & quantum states.\r\n"
    "  [+] Economics         : Modeling input-output relationships, supply, and demand.\r\n"
    "  [+] Machine Learning  : Representing neural network weights, embeddings, & data.\r\n\r\n"
    "GETTING STARTED:\r\n"
    "  Click '5. Matrix Calculator' in the sidebar on the left to begin calculations,\r\n"
    "  or browse '2. Matrix Basics' and '3. Types of Matrices' to explore theory.\r\n";

static const char* BASICS_TEXT =
    "======================================================================\r\n"
    "                         MATRIX BASICS & ANATOMY                      \r\n"
    "======================================================================\r\n\r\n"
    "Think of a matrix as a table with rows (going across) and columns (going down).\r\n\r\n"
    "CORE TERMINOLOGY:\r\n"
    "  * Row      : A horizontal line of elements.\r\n"
    "  * Column   : A vertical line of elements.\r\n"
    "  * Element  : Each individual number inside the matrix.\r\n"
    "  * Order    : Written as 'm x n', meaning m rows and n columns.\r\n"
    "               Example: A 2x3 matrix has 2 rows and 3 columns (6 elements total).\r\n"
    "  * Notation : Matrices are named with capital letters, like A or B.\r\n"
    "               A single element is written A[i][j], denoting the value in\r\n"
    "               row i, column j.\r\n\r\n"
    "VISUAL REPRESENTATION:\r\n"
    "             Column 1    Column 2    Column 3\r\n"
    "  Row 1: [   A[1][1]     A[1][2]     A[1][3]   ]\r\n"
    "  Row 2: [   A[2][1]     A[2][2]     A[2][3]   ]\r\n\r\n"
    "INDEXING RULES:\r\n"
    "  - In mathematics, indices typically start at 1 (Row 1, Column 1).\r\n"
    "  - In C++ programming, array indices start at 0 (Row 0, Column 0).\r\n"
    "  - The application automatically bridges between mathematical inputs and code.\r\n";

static const char* TYPES_TEXT =
    "======================================================================\r\n"
    "                    TAXONOMY & CLASSIFICATION OF MATRICES             \r\n"
    "======================================================================\r\n\r\n"
    "1. BASIC TYPES:\r\n"
    "   * Row Matrix         : Has only 1 row (1 x n). Example: [ 3  5  8 ]\r\n"
    "   * Column Matrix      : Has only 1 column (m x 1). Example: [ 2; 4; 9 ]\r\n"
    "   * Rectangular Matrix : Rows and columns are different (m != n).\r\n"
    "   * Square Matrix      : Rows and columns are equal (n x n).\r\n"
    "   * Zero (Null) Matrix : Every element inside is 0.\r\n\r\n"
    "2. SPECIAL SQUARE MATRICES:\r\n"
    "   * Diagonal Matrix    : Square matrix where all non-diagonal elements are 0.\r\n"
    "   * Scalar Matrix      : Diagonal matrix where all diagonal entries are equal.\r\n"
    "   * Identity Matrix (I): Diagonal matrix where all diagonal entries are 1.\r\n"
    "   * Upper Triangular   : Every element below the main diagonal is 0.\r\n"
    "   * Lower Triangular   : Every element above the main diagonal is 0.\r\n\r\n"
    "3. PROPERTY-BASED TYPES:\r\n"
    "   * Symmetric Matrix      : Matrix equals its own transpose (A = A^T).\r\n"
    "   * Skew-Symmetric Matrix : Matrix equals the negative of its transpose (A = -A^T).\r\n"
    "                             Diagonal elements must always be 0.\r\n\r\n"
    "4. DETERMINANT & INVERSE TYPES:\r\n"
    "   * Singular Matrix     : Determinant equals 0 (no multiplicative inverse exists).\r\n"
    "   * Non-Singular Matrix : Determinant is not 0 (a unique inverse A^-1 exists).\r\n\r\n"
    "5. ADVANCED TYPES:\r\n"
    "   * Idempotent Matrix : A^2 = A.\r\n"
    "   * Nilpotent Matrix  : A^k = 0 for some positive integer power k.\r\n"
    "   * Involutory Matrix : A^2 = Identity Matrix (A is its own inverse).\r\n"
    "   * Orthogonal Matrix : A * A^T = Identity Matrix (A^T = A^-1).\r\n";

static const char* OPERATIONS_TEXT =
    "======================================================================\r\n"
    "                  MATHEMATICAL OPERATIONS SPECIFICATION               \r\n"
    "======================================================================\r\n\r\n"
    "1. MATRIX ADDITION (A + B):\r\n"
    "   * Rule      : Both matrices must have the EXACT SAME order.\r\n"
    "   * Condition : order(A) == order(B); otherwise addition is undefined.\r\n"
    "   * Formula   : C[i][j] = A[i][j] + B[i][j]\r\n"
    "   * Result    : Same order (m x n) as operand matrices.\r\n\r\n"
    "2. MATRIX MULTIPLICATION (A x B):\r\n"
    "   * Rule      : Columns of Matrix A must EQUAL rows of Matrix B.\r\n"
    "   * Condition : cols(A) == rows(B); otherwise multiplication is undefined.\r\n"
    "   * Formula   : C[i][j] = sum(A[i][k] * B[k][j]) for k=0..(cols(A)-1)\r\n"
    "   * Result    : Order becomes rows(A) x cols(B).\r\n"
    "   * Note      : Matrix multiplication is non-commutative in general (A*B != B*A).\r\n\r\n"
    "3. MATRIX TRANSPOSE (A^T):\r\n"
    "   * Rule      : Works on ANY matrix; no dimensional restrictions.\r\n"
    "   * Action    : Swaps row and column indices.\r\n"
    "   * Formula   : C[j][i] = A[i][j]\r\n"
    "   * Result    : Order changes from (m x n) to (n x m).\r\n";

static const char* ABOUT_TEXT =
    "======================================================================\r\n"
    "                  MATRIX STUDIO - ARCHITECTURE & ABOUT                \r\n"
    "======================================================================\r\n\r\n"
    "Application  : Matrix Operations System (C++ Desktop Application)\r\n"
    "Architecture : Standalone Native Windows (Win32 + GDI+ Anti-Aliased Engine)\r\n"
    "Design Style : Modern Liquid Glass / VisionOS Frosted Aesthetics\r\n"
    "Language     : Modern C++ (C++11 Standard)\r\n\r\n"
    "OBJECT-ORIENTED PROGRAMMING (OOP) PRINCIPLES APPLIED:\r\n\r\n"
    "  1. Encapsulation:\r\n"
    "     The internal 2D storage grid (rows, cols, data vector) is strictly private.\r\n"
    "     External callers interact solely via public getters, setters, and operator methods.\r\n\r\n"
    "  2. Operator Overloading:\r\n"
    "     Matrix objects support natural mathematical operations:\r\n"
    "       - Matrix operator+(const Matrix& other) const;\r\n"
    "       - Matrix operator*(const Matrix& other) const;\r\n"
    "     This allows clean expressions like 'Matrix result = a + b;' in code.\r\n\r\n"
    "  3. Separation of Concerns:\r\n"
    "     The Matrix class is 100% decoupled from the UI rendering layer.\r\n"
    "     Mathematical logic can be tested, reused, or benchmarked completely\r\n"
    "     independently of Windows display code.\r\n";

// ================= Dark Mode Hook Types =================

enum PreferredAppMode { AppModeDefault, AllowDark, ForceDark, ForceLight, MaxMode };
typedef PreferredAppMode (WINAPI *fnSetPreferredAppMode)(PreferredAppMode);
typedef BOOL (WINAPI *fnAllowDarkModeForWindow)(HWND, BOOL);

static void EnableDarkModeSupport(HWND hwnd = NULL) {
    HMODULE hUxTheme = LoadLibraryA("uxtheme.dll");
    if (hUxTheme) {
        fnSetPreferredAppMode pSetPreferredAppMode = (fnSetPreferredAppMode)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135));
        if (pSetPreferredAppMode) {
            pSetPreferredAppMode(ForceDark);
        }
        if (hwnd) {
            fnAllowDarkModeForWindow pAllowDarkMode = (fnAllowDarkModeForWindow)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133));
            if (pAllowDarkMode) {
                pAllowDarkMode(hwnd, TRUE);
            }
        }
    }
}

// ================= Global State =================

static HINSTANCE   g_hInst;
static ULONG_PTR   g_gdiplusToken;
static HFONT       g_hFontRegular = NULL;
static HFONT       g_hFontBold    = NULL;
static HFONT       g_hFontMono    = NULL;

static int         g_currentPage = PAGE_HOME;
static bool        g_isDark      = true;

static COLORREF    g_clrBg;
static COLORREF    g_clrPanel;
static COLORREF    g_clrBorder;
static COLORREF    g_clrText;
static COLORREF    g_clrTextMuted;
static COLORREF    g_clrEditBg;
static COLORREF    g_clrEditText;
static COLORREF    g_clrAccent;
static HBRUSH      g_hBrushBg     = NULL;
static HBRUSH      g_hBrushEdit   = NULL;

// Window handles
static HWND        g_hwndMain     = NULL;
static HWND        hNav[PAGE_COUNT];
static HWND        hCalcEditA, hCalcEditB, hCalcEditResult;
static HWND        hCalcBtnAdd, hCalcBtnMul, hCalcBtnTrans, hCalcBtnUseA;
static HWND        hAIdent, hAZero, hARandom;
static HWND        hBIdent, hBZero, hBRandom;
static HWND        hHomeBtnAdd, hHomeBtnMul, hHomeBtnTrans;
static HWND        hSetBtnDark, hSetBtnLight;
static HWND        hInfoEdit;

static string      g_lastSuccessResult = "";

// ================= Forward Declarations =================

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitTheme(bool dark, HWND hwnd);
void CreateAllControls(HWND hwnd);
void ShowActivePage(int page);
void UpdateControlLayout(int clientW, int clientH);
void AddRoundedRect(GraphicsPath& path, RectF rect, float radius);

void HandleAdd();
void HandleMultiply();
void HandleTranspose();
void HandleUseAsA();
void FillMatrixPreset(HWND hEdit, const string& type);

string GetEditText(HWND hEdit);
void SetEditText(HWND hEdit, const string& text);

// ================= WinMain =================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;

    g_hInst = hInstance;
    srand((unsigned int)time(NULL));

    // Force system to dark mode for scrollbars and controls at startup
    EnableDarkModeSupport(NULL);

    // Initialize GDI+ for anti-aliased rendering
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;   // Handled in WM_ERASEBKGND / WM_PAINT
    wc.lpszClassName = "LiquidGlassMatrixApp";
    RegisterClassEx(&wc);

    // Center window on screen
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 1060;
    int winH = 740;
    int posX = (screenW - winW) / 2;
    int posY = (screenH - winH) / 2;

    HWND hwnd = CreateWindowExA(
        WS_EX_APPWINDOW, "LiquidGlassMatrixApp", "Matrix Studio - Mathematical Workspace",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        posX, posY, winW, winH,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;
    g_hwndMain = hwnd;

    EnableDarkModeSupport(hwnd);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplusToken);
    return (int)msg.wParam;
}

// ================= Theme & Brushes =================

void InitTheme(bool dark, HWND hwnd) {
    g_isDark = dark;

    if (dark) {
        g_clrBg        = RGB(12, 14, 20);
        g_clrPanel     = RGB(20, 24, 34);
        g_clrBorder    = RGB(42, 50, 68);
        g_clrText      = RGB(242, 245, 252);
        g_clrTextMuted = RGB(140, 148, 165);
        g_clrEditBg    = RGB(16, 20, 28);
        g_clrEditText  = RGB(235, 240, 255);
        g_clrAccent    = RGB(45, 115, 235);
    } else {
        g_clrBg        = RGB(242, 244, 248);
        g_clrPanel     = RGB(255, 255, 255);
        g_clrBorder    = RGB(215, 222, 235);
        g_clrText      = RGB(18, 24, 38);
        g_clrTextMuted = RGB(100, 110, 125);
        g_clrEditBg    = RGB(255, 255, 255);
        g_clrEditText  = RGB(20, 25, 40);
        g_clrAccent    = RGB(35, 100, 225);
    }

    if (g_hBrushBg) DeleteObject(g_hBrushBg);
    g_hBrushBg = CreateSolidBrush(g_clrBg);

    if (g_hBrushEdit) DeleteObject(g_hBrushEdit);
    g_hBrushEdit = CreateSolidBrush(g_clrEditBg);

    // Apply Windows 10/11 immersive dark mode to window frame
    if (hwnd) {
        BOOL useDarkMode = dark ? TRUE : FALSE;
        DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &useDarkMode, sizeof(useDarkMode));
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// ================= Helper: Rounded Rect =================

void AddRoundedRect(GraphicsPath& path, RectF rect, float radius) {
    float diameter = radius * 2.0f;
    if (diameter > rect.Width) diameter = rect.Width;
    if (diameter > rect.Height) diameter = rect.Height;

    path.AddArc(rect.X, rect.Y, diameter, diameter, 180.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y, diameter, diameter, 270.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y + rect.Height - diameter, diameter, diameter, 0.0f, 90.0f);
    path.AddArc(rect.X, rect.Y + rect.Height - diameter, diameter, diameter, 90.0f, 90.0f);
    path.CloseFigure();
}

// ================= Custom Owner-Draw Button =================

void DrawCustomButton(LPDRAWITEMSTRUCT dis) {
    Graphics g(dis->hDC);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RectF rect((float)dis->rcItem.left, (float)dis->rcItem.top,
               (float)(dis->rcItem.right - dis->rcItem.left),
               (float)(dis->rcItem.bottom - dis->rcItem.top));

    bool isSelected = (dis->itemState & ODS_SELECTED);
    int id = dis->CtlID;

    bool isNav = (id >= IDC_NAV_BASE && id < IDC_NAV_BASE + PAGE_COUNT);
    bool isNavActive = isNav && ((id - IDC_NAV_BASE) == g_currentPage);
    bool isAction = (id == IDC_CALC_BTN_ADD || id == IDC_CALC_BTN_MUL || id == IDC_CALC_BTN_TRANS);

    // 1. Fill entire button rect with the parent panel color
    // This prevents ANY default white button face color from showing around corners!
    SolidBrush baseBg(g_isDark ? Color(255, 20, 24, 34) : Color(255, 255, 255));
    g.FillRectangle(&baseBg, rect);

    // 2. Button pill shape
    GraphicsPath path;
    RectF btnRect = rect;
    btnRect.Inflate(-1.0f, -1.0f);
    AddRoundedRect(path, btnRect, isNav ? 8.0f : 6.0f);

    Color fillClr;
    Color borderClr;
    Color textClr;

    if (isNavActive) {
        // Active Sidebar Button: Radiant Blue
        fillClr = Color(255, 37, 99, 235);
        borderClr = Color(255, 96, 165, 250);
        textClr = Color(255, 255, 255, 255);
    } else if (isNav) {
        // Inactive Sidebar Button: Sleek Dark Glass Tint (Syncs with UI, NEVER white!)
        if (isSelected) {
            fillClr = g_isDark ? Color(255, 38, 46, 64) : Color(255, 220, 226, 238);
            borderClr = g_isDark ? Color(255, 70, 84, 115) : Color(255, 185, 195, 212);
            textClr = Color(255, 255, 255, 255);
        } else {
            fillClr = g_isDark ? Color(255, 26, 31, 44) : Color(255, 242, 245, 250);
            borderClr = g_isDark ? Color(255, 46, 54, 76) : Color(255, 215, 222, 232);
            textClr = g_isDark ? Color(255, 215, 225, 240) : Color(255, 35, 42, 58);
        }
    } else if (isAction) {
        // Calculator Primary Action Button
        fillClr = isSelected ? Color(255, 29, 78, 216) : Color(255, 37, 99, 235);
        borderClr = Color(255, 96, 165, 250);
        textClr = Color(255, 255, 255, 255);
    } else {
        // Preset / Utility Button
        if (isSelected) {
            fillClr = g_isDark ? Color(255, 45, 54, 75) : Color(255, 215, 222, 235);
            borderClr = g_isDark ? Color(255, 75, 90, 122) : Color(255, 180, 190, 210);
            textClr = Color(255, 255, 255, 255);
        } else {
            fillClr = g_isDark ? Color(255, 28, 34, 48) : Color(255, 242, 245, 250);
            borderClr = g_isDark ? Color(255, 48, 58, 80) : Color(255, 210, 218, 230);
            textClr = g_isDark ? Color(255, 205, 215, 235) : Color(255, 30, 40, 60);
        }
    }

    SolidBrush brush(fillClr);
    g.FillPath(&brush, &path);

    Pen pen(borderClr, 1.0f);
    g.DrawPath(&pen, &path);

    // Get Button Text
    char text[128];
    GetWindowTextA(dis->hwndItem, text, sizeof(text));

    FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font font(&fontFamily, isNavActive ? 10.5f : 10.0f,
                       (isNavActive || isAction) ? FontStyleBold : FontStyleRegular, UnitPoint);

    StringFormat format;
    format.SetAlignment(isNav ? StringAlignmentNear : StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    RectF textRect = btnRect;
    if (isNav) {
        textRect.X += 14.0f;
        textRect.Width -= 14.0f;
    }

    // Convert char* to wchar_t*
    int wlen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    vector<wchar_t> wtext(wlen);
    MultiByteToWideChar(CP_UTF8, 0, text, -1, &wtext[0], wlen);

    SolidBrush textBrush(textClr);
    g.DrawString(&wtext[0], -1, &font, textRect, &format, &textBrush);
}

// ================= Window Procedure =================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_CREATE: {
            g_hFontRegular = CreateFontA(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

            g_hFontBold = CreateFontA(-15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

            g_hFontMono = CreateFontA(-15, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");

            InitTheme(true, hwnd);
            CreateAllControls(hwnd);
            ShowActivePage(PAGE_HOME);
            return 0;
        }

        case WM_SIZE: {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            UpdateControlLayout(w, h);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1; // Handled in WM_PAINT

        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
            DrawCustomButton(dis);
            return TRUE;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            HWND hCtl = (HWND)lParam;
            if (hCtl == hCalcEditA || hCtl == hCalcEditB || hCtl == hCalcEditResult || hCtl == hInfoEdit) {
                SetTextColor(hdc, g_clrEditText);
                SetBkColor(hdc, g_clrEditBg);
                return (LRESULT)g_hBrushEdit;
            }
            SetTextColor(hdc, g_clrText);
            SetBkColor(hdc, g_clrBg);
            return (LRESULT)g_hBrushBg;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdcScreen = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

            // Double buffering to eliminate flicker
            HDC hdcMem = CreateCompatibleDC(hdcScreen);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, width, height);
            HGDIOBJ hbmOld = SelectObject(hdcMem, hbmMem);

            Graphics g(hdcMem);
            g.SetSmoothingMode(SmoothingModeAntiAlias);

            // 1. Paint Background with Deep Obsidian Ambient Lighting
            SolidBrush bgBrush(g_isDark ? Color(255, 12, 14, 20) : Color(255, 242, 244, 248));
            g.FillRectangle(&bgBrush, 0, 0, width, height);

            if (g_isDark) {
                // Ambient light orbs
                GraphicsPath orbPath1, orbPath2;
                orbPath1.AddEllipse(width - 320, -100, 450, 450);
                PathGradientBrush pgb1(&orbPath1);
                pgb1.SetCenterColor(Color(45, 59, 130, 246));
                Color surround1[] = { Color(0, 59, 130, 246) };
                int count1 = 1;
                pgb1.SetSurroundColors(surround1, &count1);
                g.FillPath(&pgb1, &orbPath1);

                orbPath2.AddEllipse(100, height - 300, 500, 500);
                PathGradientBrush pgb2(&orbPath2);
                pgb2.SetCenterColor(Color(35, 139, 92, 246));
                Color surround2[] = { Color(0, 139, 92, 246) };
                int count2 = 1;
                pgb2.SetSurroundColors(surround2, &count2);
                g.FillPath(&pgb2, &orbPath2);
            }

            // 2. Paint Sidebar Glass Panel
            float sideW = 230.0f;
            RectF sideRect(12.0f, 12.0f, sideW, (float)(height - 24));
            GraphicsPath sidePath;
            AddRoundedRect(sidePath, sideRect, 14.0f);

            SolidBrush sideBrush(g_isDark ? Color(255, 20, 24, 34) : Color(255, 255, 255));
            g.FillPath(&sideBrush, &sidePath);

            Pen sidePen(g_isDark ? Color(255, 42, 50, 70) : Color(255, 215, 222, 235), 1.0f);
            g.DrawPath(&sidePen, &sidePath);

            // Brand Logo & Title in Sidebar
            RectF logoRect(26.0f, 24.0f, 34.0f, 34.0f);
            GraphicsPath logoPath;
            AddRoundedRect(logoPath, logoRect, 8.0f);
            LinearGradientBrush logoBrush(logoRect, Color(255, 45, 115, 235), Color(255, 139, 92, 246), 45.0f);
            g.FillPath(&logoBrush, &logoPath);

            FontFamily segoe(L"Segoe UI");
            Gdiplus::Font logoM(&segoe, 13.0f, FontStyleBold, UnitPoint);
            StringFormat centerFmt;
            centerFmt.SetAlignment(StringAlignmentCenter);
            centerFmt.SetLineAlignment(StringAlignmentCenter);
            SolidBrush whiteBrush(Color(255, 255, 255, 255));
            g.DrawString(L"M", -1, &logoM, logoRect, &centerFmt, &whiteBrush);

            Gdiplus::Font brandTitle(&segoe, 12.0f, FontStyleBold, UnitPoint);
            Gdiplus::Font brandSub(&segoe, 8.5f, FontStyleRegular, UnitPoint);
            SolidBrush brandTitleBrush(g_isDark ? Color(255, 245, 248, 255) : Color(255, 20, 25, 40));
            SolidBrush brandSubBrush(g_isDark ? Color(255, 140, 150, 170) : Color(255, 110, 120, 135));

            RectF brandTextRect(68.0f, 22.0f, 160.0f, 20.0f);
            RectF brandSubRect(68.0f, 40.0f, 160.0f, 16.0f);
            StringFormat leftFmt;
            leftFmt.SetAlignment(StringAlignmentNear);
            g.DrawString(L"Matrix Studio", -1, &brandTitle, brandTextRect, &leftFmt, &brandTitleBrush);
            g.DrawString(L"LINEAR ALGEBRA OS", -1, &brandSub, brandSubRect, &leftFmt, &brandSubBrush);

            // 3. Paint Header Glass Bar
            float mainX = sideW + 24.0f;
            float mainW = (float)(width - mainX - 12.0f);
            float headerH = 58.0f;

            RectF headerRect(mainX, 12.0f, mainW, headerH);
            GraphicsPath headerPath;
            AddRoundedRect(headerPath, headerRect, 14.0f);

            SolidBrush headerBrush(g_isDark ? Color(255, 20, 24, 34) : Color(255, 255, 255));
            g.FillPath(&headerBrush, &headerPath);
            Pen headerPen(g_isDark ? Color(255, 42, 50, 70) : Color(255, 215, 222, 235), 1.0f);
            g.DrawPath(&headerPen, &headerPath);

            const wchar_t* pageTitles[PAGE_COUNT] = {
                L"Matrix Studio — Dashboard & Overview",
                L"Matrix Basics — Core Definitions & Structure",
                L"Types of Matrices — Complete Taxonomy",
                L"Matrix Operations — Mathematical Rules & Formulas",
                L"Matrix Calculator — Interactive Workspace",
                L"Settings — Visual Themes & Configuration",
                L"About Application — C++ & OOP Architecture"
            };

            Gdiplus::Font pageFont(&segoe, 12.5f, FontStyleBold, UnitPoint);
            RectF headerTitleRect(mainX + 20.0f, 18.0f, mainW - 40.0f, 30.0f);
            g.DrawString(pageTitles[g_currentPage], -1, &pageFont, headerTitleRect, &leftFmt, &brandTitleBrush);

            // 4. Paint Main Content Container Card
            float contentY = 12.0f + headerH + 12.0f;
            float contentH = (float)(height - contentY - 12.0f);

            RectF contentRect(mainX, contentY, mainW, contentH);
            GraphicsPath contentPath;
            AddRoundedRect(contentPath, contentRect, 14.0f);

            SolidBrush contentBrush(g_isDark ? Color(255, 16, 20, 28) : Color(255, 255, 255));
            g.FillPath(&contentBrush, &contentPath);
            Pen contentPen(g_isDark ? Color(255, 42, 50, 72) : Color(255, 215, 222, 235), 1.0f);
            g.DrawPath(&contentPen, &contentPath);

            // Transfer memory bitmap to screen
            BitBlt(hdcScreen, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);

            if (id >= IDC_NAV_BASE && id < IDC_NAV_BASE + PAGE_COUNT) {
                ShowActivePage(id - IDC_NAV_BASE);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (id == IDC_CALC_BTN_ADD) {
                HandleAdd();
            }
            else if (id == IDC_CALC_BTN_MUL) {
                HandleMultiply();
            }
            else if (id == IDC_CALC_BTN_TRANS) {
                HandleTranspose();
            }
            else if (id == IDC_CALC_BTN_USE_A) {
                HandleUseAsA();
            }
            else if (id == IDC_A_PRESET_IDENT) {
                FillMatrixPreset(hCalcEditA, "identity");
            }
            else if (id == IDC_A_PRESET_ZERO) {
                FillMatrixPreset(hCalcEditA, "zero");
            }
            else if (id == IDC_A_PRESET_RANDOM) {
                FillMatrixPreset(hCalcEditA, "random");
            }
            else if (id == IDC_B_PRESET_IDENT) {
                FillMatrixPreset(hCalcEditB, "identity");
            }
            else if (id == IDC_B_PRESET_ZERO) {
                FillMatrixPreset(hCalcEditB, "zero");
            }
            else if (id == IDC_B_PRESET_RANDOM) {
                FillMatrixPreset(hCalcEditB, "random");
            }
            else if (id == IDC_HOME_GO_ADD || id == IDC_HOME_GO_MUL || id == IDC_HOME_GO_TRANS) {
                ShowActivePage(PAGE_CALCULATOR);
                InvalidateRect(hwnd, NULL, TRUE);
                if (id == IDC_HOME_GO_ADD) HandleAdd();
                else if (id == IDC_HOME_GO_MUL) HandleMultiply();
                else if (id == IDC_HOME_GO_TRANS) HandleTranspose();
            }
            else if (id == IDC_SET_BTN_DARK) {
                InitTheme(true, hwnd);
            }
            else if (id == IDC_SET_BTN_LIGHT) {
                InitTheme(false, hwnd);
            }
            return 0;
        }

        case WM_DESTROY: {
            if (g_hBrushBg) DeleteObject(g_hBrushBg);
            if (g_hBrushEdit) DeleteObject(g_hBrushEdit);
            if (g_hFontRegular) DeleteObject(g_hFontRegular);
            if (g_hFontBold) DeleteObject(g_hFontBold);
            if (g_hFontMono) DeleteObject(g_hFontMono);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ================= Control Creation =================

void CreateAllControls(HWND hwnd) {
    const char* navLabels[PAGE_COUNT] = {
        "1.  Home",
        "2.  Matrix Basics",
        "3.  Types of Matrices",
        "4.  Matrix Operations",
        "5.  Matrix Calculator",
        "6.  Settings",
        "7.  About"
    };

    // Sidebar navigation buttons (BS_OWNERDRAW with dark background)
    int navY = 74;
    for (int i = 0; i < PAGE_COUNT; i++) {
        hNav[i] = CreateWindowA(
            "BUTTON", navLabels[i],
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            24, navY, 206, 40,
            hwnd, (HMENU)(INT_PTR)(IDC_NAV_BASE + i), g_hInst, NULL
        );
        navY += 48;
    }

    // Calculator inputs & controls (NO WS_BORDER to remove ugly white border)
    DWORD editStyle = WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;

    hCalcEditA = CreateWindowA("EDIT", "2 2\r\n1 2\r\n3 4", editStyle,
        0, 0, 100, 100, hwnd, (HMENU)(INT_PTR)IDC_CALC_EDIT_A, g_hInst, NULL);

    hCalcEditB = CreateWindowA("EDIT", "2 2\r\n5 6\r\n7 8", editStyle,
        0, 0, 100, 100, hwnd, (HMENU)(INT_PTR)IDC_CALC_EDIT_B, g_hInst, NULL);

    hCalcEditResult = CreateWindowA("EDIT", "", editStyle | ES_READONLY,
        0, 0, 100, 100, hwnd, (HMENU)(INT_PTR)IDC_CALC_EDIT_RESULT, g_hInst, NULL);

    // Matrix A Presets
    hAIdent  = CreateWindowA("BUTTON", "Identity", WS_CHILD | BS_OWNERDRAW, 0, 0, 70, 26, hwnd, (HMENU)(INT_PTR)IDC_A_PRESET_IDENT, g_hInst, NULL);
    hAZero   = CreateWindowA("BUTTON", "Zero",     WS_CHILD | BS_OWNERDRAW, 0, 0, 60, 26, hwnd, (HMENU)(INT_PTR)IDC_A_PRESET_ZERO, g_hInst, NULL);
    hARandom = CreateWindowA("BUTTON", "Random",   WS_CHILD | BS_OWNERDRAW, 0, 0, 70, 26, hwnd, (HMENU)(INT_PTR)IDC_A_PRESET_RANDOM, g_hInst, NULL);

    // Matrix B Presets
    hBIdent  = CreateWindowA("BUTTON", "Identity", WS_CHILD | BS_OWNERDRAW, 0, 0, 70, 26, hwnd, (HMENU)(INT_PTR)IDC_B_PRESET_IDENT, g_hInst, NULL);
    hBZero   = CreateWindowA("BUTTON", "Zero",     WS_CHILD | BS_OWNERDRAW, 0, 0, 60, 26, hwnd, (HMENU)(INT_PTR)IDC_B_PRESET_ZERO, g_hInst, NULL);
    hBRandom = CreateWindowA("BUTTON", "Random",   WS_CHILD | BS_OWNERDRAW, 0, 0, 70, 26, hwnd, (HMENU)(INT_PTR)IDC_B_PRESET_RANDOM, g_hInst, NULL);

    // Operations Command Buttons
    hCalcBtnAdd   = CreateWindowA("BUTTON", "Add (A + B)",       WS_CHILD | BS_OWNERDRAW, 0, 0, 140, 38, hwnd, (HMENU)(INT_PTR)IDC_CALC_BTN_ADD, g_hInst, NULL);
    hCalcBtnMul   = CreateWindowA("BUTTON", "Multiply (A x B)",  WS_CHILD | BS_OWNERDRAW, 0, 0, 150, 38, hwnd, (HMENU)(INT_PTR)IDC_CALC_BTN_MUL, g_hInst, NULL);
    hCalcBtnTrans = CreateWindowA("BUTTON", "Transpose (A)",     WS_CHILD | BS_OWNERDRAW, 0, 0, 140, 38, hwnd, (HMENU)(INT_PTR)IDC_CALC_BTN_TRANS, g_hInst, NULL);
    hCalcBtnUseA  = CreateWindowA("BUTTON", "Use as Matrix A",   WS_CHILD | BS_OWNERDRAW, 0, 0, 140, 38, hwnd, (HMENU)(INT_PTR)IDC_CALC_BTN_USE_A, g_hInst, NULL);

    // Educational & Info Multi-Line Viewer (NO WS_BORDER)
    hInfoEdit = CreateWindowA("EDIT", "", editStyle | ES_READONLY,
        0, 0, 100, 100, hwnd, (HMENU)(INT_PTR)IDC_INFO_EDIT, g_hInst, NULL);

    // Home Page Quick Launchers
    hHomeBtnAdd   = CreateWindowA("BUTTON", "Launch Addition",       WS_CHILD | BS_OWNERDRAW, 0, 0, 140, 36, hwnd, (HMENU)(INT_PTR)IDC_HOME_GO_ADD, g_hInst, NULL);
    hHomeBtnMul   = CreateWindowA("BUTTON", "Launch Multiplication", WS_CHILD | BS_OWNERDRAW, 0, 0, 160, 36, hwnd, (HMENU)(INT_PTR)IDC_HOME_GO_MUL, g_hInst, NULL);
    hHomeBtnTrans = CreateWindowA("BUTTON", "Launch Transpose",      WS_CHILD | BS_OWNERDRAW, 0, 0, 140, 36, hwnd, (HMENU)(INT_PTR)IDC_HOME_GO_TRANS, g_hInst, NULL);

    // Settings Theme Buttons
    hSetBtnDark  = CreateWindowA("BUTTON", "Dark Obsidian Theme", WS_CHILD | BS_OWNERDRAW, 0, 0, 160, 38, hwnd, (HMENU)(INT_PTR)IDC_SET_BTN_DARK, g_hInst, NULL);
    hSetBtnLight = CreateWindowA("BUTTON", "Light Silver Theme",   WS_CHILD | BS_OWNERDRAW, 0, 0, 160, 38, hwnd, (HMENU)(INT_PTR)IDC_SET_BTN_LIGHT, g_hInst, NULL);

    // Set Monospace Font on code / matrix edit boxes
    SendMessage(hCalcEditA, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);
    SendMessage(hCalcEditB, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);
    SendMessage(hCalcEditResult, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);
    SendMessage(hInfoEdit, WM_SETFONT, (WPARAM)g_hFontMono, TRUE);

    // Apply DarkMode_Explorer to controls to turn scrollbars dark!
    SetWindowTheme(hwnd, L"DarkMode_Explorer", NULL);
    SetWindowTheme(hInfoEdit, L"DarkMode_Explorer", NULL);
    SetWindowTheme(hCalcEditA, L"DarkMode_Explorer", NULL);
    SetWindowTheme(hCalcEditB, L"DarkMode_Explorer", NULL);
    SetWindowTheme(hCalcEditResult, L"DarkMode_Explorer", NULL);

    // Default Result Text
    SetEditText(hCalcEditResult, "Ready. Click Add (A + B), Multiply (A x B), or Transpose (A) to compute.");
}

// ================= Layout Management =================

void UpdateControlLayout(int clientW, int clientH) {
    float sideW = 230.0f;
    float mainX = sideW + 24.0f;
    float mainW = (float)(clientW - mainX - 12.0f);
    float headerH = 58.0f;
    float contentY = 12.0f + headerH + 12.0f; // 82.0f
    float contentH = (float)(clientH - contentY - 12.0f);

    if (mainW < 200.0f) mainW = 200.0f;
    if (contentH < 200.0f) contentH = 200.0f;

    // Position info edit cleanly inside the content card (8px inset from border)
    int pad = 10;
    MoveWindow(hInfoEdit, (int)mainX + pad, (int)contentY + pad, (int)mainW - 2 * pad, (int)contentH - 2 * pad, TRUE);

    // Position Calculator Controls
    int cardW = ((int)mainW - 36) / 2;
    int editH = ((int)contentH - 190) / 2;
    if (editH < 70) editH = 70;

    // Matrix A Box & Presets
    MoveWindow(hAIdent,  (int)mainX + 14, (int)contentY + 12, 68, 24, TRUE);
    MoveWindow(hAZero,   (int)mainX + 88, (int)contentY + 12, 56, 24, TRUE);
    MoveWindow(hARandom, (int)mainX + 150, (int)contentY + 12, 68, 24, TRUE);
    MoveWindow(hCalcEditA, (int)mainX + 14, (int)contentY + 40, cardW, editH, TRUE);

    // Matrix B Box & Presets
    int col2X = (int)mainX + 22 + cardW;
    MoveWindow(hBIdent,  col2X, (int)contentY + 12, 68, 24, TRUE);
    MoveWindow(hBZero,   col2X + 74, (int)contentY + 12, 56, 24, TRUE);
    MoveWindow(hBRandom, col2X + 136, (int)contentY + 12, 68, 24, TRUE);
    MoveWindow(hCalcEditB, col2X, (int)contentY + 40, cardW, editH, TRUE);

    // Command Buttons Bar
    int barY = (int)contentY + 40 + editH + 12;
    int btnW = 135;
    int btnGap = 10;
    MoveWindow(hCalcBtnAdd,   (int)mainX + 14, barY, btnW, 36, TRUE);
    MoveWindow(hCalcBtnMul,   (int)mainX + 14 + btnW + btnGap, barY, 150, 36, TRUE);
    MoveWindow(hCalcBtnTrans, (int)mainX + 14 + 2 * btnW + 15 + btnGap, barY, btnW, 36, TRUE);
    MoveWindow(hCalcBtnUseA,  (int)mainX + 14 + 3 * btnW + 15 + 2 * btnGap, barY, 140, 36, TRUE);

    // Result Output Box
    int resY = barY + 36 + 12;
    int resH = (int)contentH - (resY - (int)contentY) - 12;
    if (resH < 70) resH = 70;
    MoveWindow(hCalcEditResult, (int)mainX + 14, resY, (int)mainW - 28, resH, TRUE);

    // Settings Theme Buttons
    MoveWindow(hSetBtnDark,  (int)mainX + 24, (int)contentY + 40, 190, 42, TRUE);
    MoveWindow(hSetBtnLight, (int)mainX + 230, (int)contentY + 40, 190, 42, TRUE);
}

// ================= Page Switching =================

void ShowActivePage(int page) {
    g_currentPage = page;

    int calcShow = (page == PAGE_CALCULATOR) ? SW_SHOW : SW_HIDE;
    ShowWindow(hCalcEditA, calcShow);
    ShowWindow(hCalcEditB, calcShow);
    ShowWindow(hCalcEditResult, calcShow);
    ShowWindow(hCalcBtnAdd, calcShow);
    ShowWindow(hCalcBtnMul, calcShow);
    ShowWindow(hCalcBtnTrans, calcShow);
    ShowWindow(hCalcBtnUseA, calcShow);
    ShowWindow(hAIdent, calcShow);
    ShowWindow(hAZero, calcShow);
    ShowWindow(hARandom, calcShow);
    ShowWindow(hBIdent, calcShow);
    ShowWindow(hBZero, calcShow);
    ShowWindow(hBRandom, calcShow);

    int setShow = (page == PAGE_SETTINGS) ? SW_SHOW : SW_HIDE;
    ShowWindow(hSetBtnDark, setShow);
    ShowWindow(hSetBtnLight, setShow);

    int infoShow = (page != PAGE_CALCULATOR && page != PAGE_SETTINGS) ? SW_SHOW : SW_HIDE;
    ShowWindow(hInfoEdit, infoShow);

    // Update text content for info pages
    if (page == PAGE_HOME) SetWindowTextA(hInfoEdit, HOME_TEXT);
    else if (page == PAGE_BASICS) SetWindowTextA(hInfoEdit, BASICS_TEXT);
    else if (page == PAGE_TYPES) SetWindowTextA(hInfoEdit, TYPES_TEXT);
    else if (page == PAGE_OPERATIONS) SetWindowTextA(hInfoEdit, OPERATIONS_TEXT);
    else if (page == PAGE_ABOUT) SetWindowTextA(hInfoEdit, ABOUT_TEXT);
}

// ================= Matrix Calculation Logic =================

string GetEditText(HWND hEdit) {
    int len = GetWindowTextLengthA(hEdit);
    string result(len + 1, '\0');
    GetWindowTextA(hEdit, &result[0], len + 1);
    result.resize(len);
    return result;
}

void SetEditText(HWND hEdit, const string& text) {
    SetWindowTextA(hEdit, text.c_str());
}

void HandleAdd() {
    try {
        ::Matrix a = parseMatrixFromText(GetEditText(hCalcEditA), "A");
        ::Matrix b = parseMatrixFromText(GetEditText(hCalcEditB), "B");
        ::Matrix result = a + b;

        ostringstream oss;
        oss << "========================================================\r\n";
        oss << "        CALCULATION SUCCESS: MATRIX ADDITION (A + B)    \r\n";
        oss << "========================================================\r\n\r\n";
        oss << "Result Dimension: " << result.getRows() << " x " << result.getCols() << "\r\n\r\n";
        oss << "Matrix Result:\r\n" << result.toString() << "\r\n";
        oss << "Step-by-step element sums:\r\n";
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < a.getCols(); ++j) {
                oss << "  C[" << (i + 1) << "][" << (j + 1) << "] = "
                    << "(" << a.get(i, j) << ") + (" << b.get(i, j) << ") = "
                    << result.get(i, j) << "\r\n";
            }
        }

        g_lastSuccessResult = to_string(result.getRows()) + " " + to_string(result.getCols()) + "\r\n";
        for (int i = 0; i < result.getRows(); ++i) {
            for (int j = 0; j < result.getCols(); ++j) {
                g_lastSuccessResult += to_string(result.get(i, j));
                if (j != result.getCols() - 1) g_lastSuccessResult += " ";
            }
            g_lastSuccessResult += "\r\n";
        }

        SetEditText(hCalcEditResult, oss.str());
    } catch (const invalid_argument& e) {
        ostringstream err;
        err << "========================================================\r\n";
        err << "                 [!] CALCULATION ERROR                  \r\n";
        err << "========================================================\r\n\r\n";
        err << e.what() << "\r\n";
        SetEditText(hCalcEditResult, err.str());
    }
}

void HandleMultiply() {
    try {
        ::Matrix a = parseMatrixFromText(GetEditText(hCalcEditA), "A");
        ::Matrix b = parseMatrixFromText(GetEditText(hCalcEditB), "B");
        ::Matrix result = a * b;

        ostringstream oss;
        oss << "========================================================\r\n";
        oss << "     CALCULATION SUCCESS: MATRIX MULTIPLICATION (A x B) \r\n";
        oss << "========================================================\r\n\r\n";
        oss << "Result Dimension: " << result.getRows() << " x " << result.getCols() << "\r\n\r\n";
        oss << "Matrix Result:\r\n" << result.toString() << "\r\n";
        oss << "Step-by-step dot products:\r\n";
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < b.getCols(); ++j) {
                oss << "  C[" << (i + 1) << "][" << (j + 1) << "] = ";
                for (int k = 0; k < a.getCols(); ++k) {
                    oss << "(" << a.get(i, k) << "*" << b.get(k, j) << ")";
                    if (k != a.getCols() - 1) oss << " + ";
                }
                oss << " = " << result.get(i, j) << "\r\n";
            }
        }

        g_lastSuccessResult = to_string(result.getRows()) + " " + to_string(result.getCols()) + "\r\n";
        for (int i = 0; i < result.getRows(); ++i) {
            for (int j = 0; j < result.getCols(); ++j) {
                g_lastSuccessResult += to_string(result.get(i, j));
                if (j != result.getCols() - 1) g_lastSuccessResult += " ";
            }
            g_lastSuccessResult += "\r\n";
        }

        SetEditText(hCalcEditResult, oss.str());
    } catch (const invalid_argument& e) {
        ostringstream err;
        err << "========================================================\r\n";
        err << "                 [!] CALCULATION ERROR                  \r\n";
        err << "========================================================\r\n\r\n";
        err << e.what() << "\r\n";
        SetEditText(hCalcEditResult, err.str());
    }
}

void HandleTranspose() {
    try {
        ::Matrix a = parseMatrixFromText(GetEditText(hCalcEditA), "A");
        ::Matrix result = a.transpose();

        ostringstream oss;
        oss << "========================================================\r\n";
        oss << "       CALCULATION SUCCESS: MATRIX TRANSPOSE (A^T)      \r\n";
        oss << "========================================================\r\n\r\n";
        oss << "Original: " << a.getRows() << "x" << a.getCols()
            << "  -->  Transposed: " << result.getRows() << "x" << result.getCols() << "\r\n\r\n";
        oss << "Matrix Result:\r\n" << result.toString() << "\r\n";
        oss << "Coordinate index inversions:\r\n";
        for (int i = 0; i < a.getRows(); ++i) {
            for (int j = 0; j < a.getCols(); ++j) {
                oss << "  A[" << (i + 1) << "][" << (j + 1) << "] ("
                    << a.get(i, j) << ") --> C[" << (j + 1) << "][" << (i + 1) << "]\r\n";
            }
        }

        g_lastSuccessResult = to_string(result.getRows()) + " " + to_string(result.getCols()) + "\r\n";
        for (int i = 0; i < result.getRows(); ++i) {
            for (int j = 0; j < result.getCols(); ++j) {
                g_lastSuccessResult += to_string(result.get(i, j));
                if (j != result.getCols() - 1) g_lastSuccessResult += " ";
            }
            g_lastSuccessResult += "\r\n";
        }

        SetEditText(hCalcEditResult, oss.str());
    } catch (const invalid_argument& e) {
        ostringstream err;
        err << "========================================================\r\n";
        err << "                 [!] CALCULATION ERROR                  \r\n";
        err << "========================================================\r\n\r\n";
        err << e.what() << "\r\n";
        SetEditText(hCalcEditResult, err.str());
    }
}

void HandleUseAsA() {
    if (!g_lastSuccessResult.empty()) {
        SetEditText(hCalcEditA, g_lastSuccessResult);
    }
}

void FillMatrixPreset(HWND hEdit, const string& type) {
    string current = GetEditText(hEdit);
    istringstream ss(current);
    int r = 2, c = 2;
    ss >> r >> c;
    if (r <= 0 || c <= 0 || r > 10 || c > 10) { r = 2; c = 2; }

    ostringstream out;
    out << r << " " << c << "\r\n";
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            int val = 0;
            if (type == "identity") val = (i == j) ? 1 : 0;
            else if (type == "zero") val = 0;
            else if (type == "random") val = (rand() % 19) - 9; // -9 to 9
            out << val;
            if (j != c - 1) out << " ";
        }
        out << "\r\n";
    }
    SetEditText(hEdit, out.str());
}

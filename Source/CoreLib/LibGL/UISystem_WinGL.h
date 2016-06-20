#ifndef GX_GLTEXT_H
#define GX_GLTEXT_H

#include "../Basic.h"
#include "../Graphics/LibUI.h"
#include "../Imaging/Bitmap.h"
#include "../WinForm/WinTimer.h"
#include "../MemoryPool.h"
#include "OpenGLHardwareRenderer.h"

namespace GraphicsUI
{
	class Font;

	class DIBImage;

	struct TextSize
	{
		int x, y;
	};

	class TextRasterizationResult
	{
	public:
		TextSize Size;
		int BufferSize;
		unsigned char * ImageData;
	};

	class WinGLSystemInterface;

	class TextRasterizer
	{
	private:
		unsigned int TexID;
		DIBImage *Bit;
	public:
		TextRasterizer();
		~TextRasterizer();
		bool MultiLine = false;
		void SetFont(const Font & Font);
		TextRasterizationResult RasterizeText(WinGLSystemInterface * system, const CoreLib::String & text);
		TextSize GetTextSize(const CoreLib::String & text);
	};


	class BakedText : public IBakedText
	{
	public:
		WinGLSystemInterface* system;
		unsigned char * textBuffer;
		int BufferSize;
		int Width, Height;
		virtual int GetWidth() override
		{
			return Width;
		}
		virtual int GetHeight() override
		{
			return Height;
		}
		~BakedText();
	};


	class WinGLFont : public IFont
	{
	private:
		TextRasterizer rasterizer;
		WinGLSystemInterface * system;
	public:
		WinGLFont(WinGLSystemInterface * ctx, const GraphicsUI::Font & font)
		{
			system = ctx;
			rasterizer.SetFont(font);
		}
		virtual Rect MeasureString(const CoreLib::String & text) override;
		virtual IBakedText * BakeString(const CoreLib::String & text) override;

	};

	class GLUIRenderer;

	class Font
	{
	public:
		CoreLib::String FontName;
		int Size;
		bool Bold, Underline, Italic, StrikeOut;
		Font()
		{
			NONCLIENTMETRICS NonClientMetrics;
			NonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(NonClientMetrics.iPaddedBorderWidth);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &NonClientMetrics, 0);
			FontName = NonClientMetrics.lfMessageFont.lfFaceName;
			Size = 9;
			Bold = false;
			Underline = false;
			Italic = false;
			StrikeOut = false;
		}
		Font(const CoreLib::String& sname, int ssize)
		{
			FontName = sname;
			Size = ssize;
			Bold = false;
			Underline = false;
			Italic = false;
			StrikeOut = false;
		}
		Font(const CoreLib::String & sname, int ssize, bool sBold, bool sItalic, bool sUnderline)
		{
			FontName = sname;
			Size = ssize;
			Bold = sBold;
			Underline = sUnderline;
			Italic = sItalic;
			StrikeOut = false;
		}
		CoreLib::String ToString() const
		{
			CoreLib::StringBuilder sb;
			sb << FontName << Size << Bold << Underline << Italic << StrikeOut;
			return sb.ProduceString();
		}
	};

	class WinGLSystemInterface : public ISystemInterface
	{
	private:
		unsigned char * textBuffer = nullptr;
		CoreLib::Dictionary<CoreLib::String, CoreLib::RefPtr<WinGLFont>> fonts;
		GL::BufferObject textBufferObj;
		CoreLib::MemoryPool textBufferPool;
		VectorMath::Vec4 ColorToVec(GraphicsUI::Color c);
		CoreLib::RefPtr<WinGLFont> defaultFont, titleFont, symbolFont;
		CoreLib::WinForm::Timer tmrHover, tmrTick;
		UIEntry * entry = nullptr;
		void TickTimerTick(CoreLib::Object *, CoreLib::WinForm::EventArgs e);
		void HoverTimerTick(CoreLib::Object *, CoreLib::WinForm::EventArgs e);
	public:
		GLUIRenderer * uiRenderer;
		GL::HardwareRenderer * glContext = nullptr;
		virtual void SetClipboardText(const CoreLib::String & text) override;
		virtual CoreLib::String GetClipboardText() override;
		virtual IFont * LoadDefaultFont(DefaultFontType dt = DefaultFontType::Content) override;
		virtual void SwitchCursor(CursorType c) override;
		void UpdateCompositionWindowPos(HIMC hIMC, int x, int y);
	public:
		WinGLSystemInterface(GL::HardwareRenderer * ctx);
		~WinGLSystemInterface();
		unsigned char * AllocTextBuffer(int size)
		{
			return textBufferPool.Alloc(size);
		}
		void FreeTextBuffer(unsigned char * buffer, int size)
		{
			textBufferPool.Free(buffer, size);
		}
		int GetTextBufferRelativeAddress(unsigned char * buffer)
		{
			return (int)(buffer - textBuffer);
		}
		GL::BufferObject GetTextBufferObject()
		{
			return textBufferObj;
		}
		IFont * LoadFont(const Font & f);
		IImage * CreateImageObject(const CoreLib::Imaging::Bitmap & bmp);
		void SetResolution(int w, int h);
		void ExecuteDrawCommands(CoreLib::List<DrawCommand> & commands);
		void SetEntry(UIEntry * pentry);
		int HandleSystemMessage(HWND hWnd, UINT message, WPARAM &wParam, LPARAM &lParam);
	};
}

#endif
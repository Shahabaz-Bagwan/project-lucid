// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#ifndef NATIVEUI_GFX_FONT_H_
#define NATIVEUI_GFX_FONT_H_

#include <memory>
#include <string>

#include "base/memory/ref_counted.h"
#include "nativeui/nativeui_export.h"
#include "nativeui/types.h"

#if defined(OS_WIN)
#include "base/win/scoped_gdi_object.h"
#endif

namespace base {
class FilePath;
}

#if defined(OS_WIN)
namespace Gdiplus {
class PrivateFontCollection;
}
#endif

namespace nu {

class NATIVEUI_EXPORT Font : public base::RefCounted<Font> {
 public:
  // Get the cached default font.
  static Font* Default();

  // Standard font weights as used in Pango and Windows. The values must match
  // https://msdn.microsoft.com/en-us/library/system.windows.fontweights(v=vs.110).aspx
  enum class Weight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Normal = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900,
  };

  // The following constants indicate the font style.
  enum class Style {
    Normal = 0,
    Italic = 1,
  };

  // Create a Font implementation with the specified |name|
  // (encoded in UTF-8), DIP |size|, |weight| and |style|.
  Font(const std::string& name, float size, Weight weight, Style style);

  // Create from from file path.
  Font(const base::FilePath& path, float size);

  // Returns a new Font derived from the existing font.
  // It is caller's responsibility to manage the lifetime of returned font.
  Font* Derive(float size_delta, Weight weight, Style style) const;

  // Return the specified font name in UTF-8.
  std::string GetName() const;

  // Return the font size in pixels.
  float GetSize() const;

  // Return the font weight.
  Weight GetWeight() const;

  // Return the font style.
  Style GetStyle() const;

  // Return the native font handle.
  NativeFont GetNative() const;

#if defined(OS_WIN)
  // Private: Return font name in UTF-16.
  const std::wstring& GetName16() const;

  // Private: Get or create the HFONT.
  HFONT GetHFONT(HWND hwnd) const;
#endif

 protected:
  // Create default system UI font.
  Font();

  virtual ~Font();

 private:
  friend class base::RefCounted<Font>;

  NativeFont font_;

#if defined(OS_WIN)
  // Cached PrivateFontCollection, used by fonts created from paths.
  std::unique_ptr<Gdiplus::PrivateFontCollection> font_collection_;

  // Cached font family, which is requested by DirectWrite a lot.
  mutable std::wstring font_family_;

  // Cached HFont.
  mutable base::win::ScopedHFONT hfont_;
#endif
};

}  // namespace nu

#endif  // NATIVEUI_GFX_FONT_H_

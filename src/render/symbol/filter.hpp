//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) Twan van Laarhoven and the other MSE developers          |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#pragma once

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <util/reflect.hpp>
#include <gfx/color.hpp>

DECLARE_POINTER_TYPE(Symbol);
class SymbolFilter;

// ----------------------------------------------------------------------------- : Symbol filtering

/// Filter a symbol-image.
/** Filtering means that each pixel will be determined by the specified function.
 *  The result is stored in the symbol parameter.
 */
void filter_symbol(Image& symbol, const SymbolFilter& filter);

/// Render a Symbol to an Image and filter it
Image render_symbol(const SymbolP& symbol, const SymbolFilter& filter, double border_radius = 0.05, int width = 100, int height = 100, bool edit_hints = false, bool allow_smaller = false);

/// Is a point inside a symbol?
enum SymbolSet
{  SYMBOL_INSIDE
,  SYMBOL_BORDER
,  SYMBOL_OUTSIDE
};

// ----------------------------------------------------------------------------- : SymbolFilter

/// Base class for symbol filters
class SymbolFilter : public IntrusivePtrVirtualBase {
public:
  virtual ~SymbolFilter() {}
  /// What color should the symbol have at location (x, y)?
  /** x,y are in the range [0...1) */
  virtual Color color(double x, double y, SymbolSet point) const = 0;
  /// Name of this fill type
  virtual String fillType() const = 0;
  /// Comparision
  virtual bool operator == (const SymbolFilter& that) const = 0;
  
  DECLARE_REFLECTION_VIRTUAL();
};

template <>
intrusive_ptr<SymbolFilter> read_new<SymbolFilter>(Reader& reader);

// ----------------------------------------------------------------------------- : SymbolFilter types

/// Symbol filter that returns solid colors
class SolidFillSymbolFilter : public SymbolFilter {
public:
  inline SolidFillSymbolFilter() {}
  inline SolidFillSymbolFilter(const Color& fill_color, const Color& border_color)
    : fill_color(fill_color), border_color(border_color)
  {}
  Color color(double x, double y, SymbolSet point) const override;
  String fillType() const override;
  bool operator == (const SymbolFilter& that) const override;
private:
  Color fill_color, border_color;
  DECLARE_REFLECTION_OVERRIDE();
};

/// Symbol filter that returns some gradient
class GradientSymbolFilter : public SymbolFilter {
public:
  inline GradientSymbolFilter() {}
  inline GradientSymbolFilter(const Color& fill_color_1, const Color& border_color_1, const Color& fill_color_2, const Color& border_color_2)
    : fill_color_1(fill_color_1), border_color_1(border_color_1)
    , fill_color_2(fill_color_2), border_color_2(border_color_2)
  {}
protected:
  Color fill_color_1, border_color_1;
  Color fill_color_2, border_color_2;
  template <typename T>
  Color color(double x, double y, SymbolSet point, const T* t) const;
  bool equal(const GradientSymbolFilter& that) const;
  
  DECLARE_REFLECTION_OVERRIDE();
};

/// Symbol filter that returns a linear gradient
class LinearGradientSymbolFilter : public GradientSymbolFilter {
public:
  LinearGradientSymbolFilter();
  LinearGradientSymbolFilter(const Color& fill_color_1, const Color& border_color_1, const Color& fill_color_2, const Color& border_color_2
                            ,double center_x, double center_y, double end_x, double end_y);
  
  Color color(double x, double y, SymbolSet point) const override;
  String fillType() const override;
  bool operator == (const SymbolFilter& that) const override;
  
  /// return time on the gradient, used by GradientSymbolFilter::color
  inline double t(double x, double y) const;
  
private:
  double center_x, center_y;
  double end_x,    end_y;
  mutable double len;
  DECLARE_REFLECTION_OVERRIDE();
};

/// Symbol filter that returns a radial gradient
class RadialGradientSymbolFilter : public GradientSymbolFilter {
public:
  inline RadialGradientSymbolFilter() {}
  inline RadialGradientSymbolFilter(const Color& fill_color_1, const Color& border_color_1, const Color& fill_color_2, const Color& border_color_2)
    : GradientSymbolFilter(fill_color_1, border_color_1, fill_color_2, border_color_2)
  {}
  
  Color color(double x, double y, SymbolSet point) const override;
  String fillType() const override;
  bool operator == (const SymbolFilter& that) const override;
  
  /// return time on the gradient, used by GradientSymbolFilter::color
  inline double t(double x, double y) const;
};


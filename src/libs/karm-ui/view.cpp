#include <karm-text/loader.h>

#include "box.h"
#include "view.h"

namespace marK::Ui {

// MARK: Text ------------------------------------------------------------------

static Opt<Rc<Text::Fontface>> _regularFontface = NONE;

Rc<Text::Fontface> regularFontface() {
    if (not _regularFontface) {
        _regularFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
}

static Opt<Rc<Text::Fontface>> _mediumFontface = NONE;

Rc<Text::Fontface> mediumFontface() {
    if (not _mediumFontface) {
        _mediumFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Medium.ttf"_url).unwrap();
    }
    return *_mediumFontface;
}

static Opt<Rc<Text::Fontface>> _boldFontface = NONE;

Rc<Text::Fontface> boldFontface() {
    if (not _boldFontface) {
        _boldFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Bold.ttf"_url).unwrap();
    }
    return *_boldFontface;
}

static Opt<Rc<Text::Fontface>> _italicFontface = NONE;

Rc<Text::Fontface> italicFontface() {
    if (not _italicFontface) {
        _italicFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Italic.ttf"_url).unwrap();
    }
    return *_italicFontface;
}

static Opt<Rc<Text::Fontface>> _codeFontface = NONE;

Rc<Text::Fontface> codeFontface() {
    if (not _codeFontface) {
        _codeFontface = Text::loadFontfaceOrFallback("bundle://fonts-fira-code/fonts/FiraCode-Regular.ttf"_url).unwrap();
    }
    return *_codeFontface;
}

Text::ProseStyle TextStyles::displayLarge() {
    return {
        .font = Text::Font{
            regularFontface(),
            57,
        },
    };
}

Text::ProseStyle TextStyles::displayMedium() {
    return {
        .font = Text::Font{
            regularFontface(),
            45,
        },
    };
}

Text::ProseStyle TextStyles::displaySmall() {
    return {
        .font = Text::Font{
            regularFontface(),
            36,
        },
    };
}

Text::ProseStyle TextStyles::headlineLarge() {
    return {
        .font = Text::Font{
            regularFontface(),
            32,
        },
    };
}

Text::ProseStyle TextStyles::headlineMedium() {
    return {
        .font = Text::Font{
            regularFontface(),
            28,
        },
    };
}

Text::ProseStyle TextStyles::headlineSmall() {
    return {
        .font = Text::Font{
            regularFontface(),
            24,
        },
    };
}

Text::ProseStyle TextStyles::titleLarge() {
    return {
        .font = Text::Font{
            regularFontface(),
            22,
        },
    };
}

Text::ProseStyle TextStyles::titleMedium() {
    return {
        .font = Text::Font{
            mediumFontface(),
            16,
        },
    };
}

Text::ProseStyle TextStyles::titleSmall() {
    return {
        .font = Text::Font{
            mediumFontface(),
            14,
        },
    };
}

Text::ProseStyle TextStyles::labelLarge() {
    return {
        .font = Text::Font{
            mediumFontface(),
            14,
        },
    };
}

Text::ProseStyle TextStyles::labelMedium() {
    return {
        .font = Text::Font{
            mediumFontface(),
            12,
        },
    };
}

Text::ProseStyle TextStyles::labelSmall() {
    return {
        .font = Text::Font{
            mediumFontface(),
            11,
        },
    };
}

Text::ProseStyle TextStyles::bodyLarge() {
    return {
        .font = Text::Font{
            regularFontface(),
            16,
        },
        .multiline = true,
    };
}

Text::ProseStyle TextStyles::bodyMedium() {
    return {
        .font = Text::Font{
            regularFontface(),
            14,
        },
        .multiline = true,
    };
}

Text::ProseStyle TextStyles::bodySmall() {
    return {
        .font = Text::Font{
            regularFontface(),
            12,
        },
        .multiline = true,
    };
}

Text::ProseStyle TextStyles::codeLarge() {
    return {
        .font = Text::Font{
            codeFontface(),
            16,
        },
        .multiline = true,
    };
}

Text::ProseStyle TextStyles::codeMedium() {
    return {
        .font = Text::Font{
            codeFontface(),
            14,
        },
        .multiline = true,
    };
}

Text::ProseStyle TextStyles::codeSmall() {
    return {
        .font = Text::Font{
            codeFontface(),
            12,
        },
        .multiline = true,
    };
}

struct Text : public View<Text> {
    Rc<marK::Text::Prose> _prose;

    Text(Rc<marK::Text::Prose> prose)
        : _prose(std::move(prose)) {}

    Text(::Text::ProseStyle style, Str text)
        : _prose(makeRc<marK::Text::Prose>(style, text)) {}

    void reconcile(Text& o) override {
        _prose = std::move(o._prose);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.origin(bound().xy.cast<f64>());
        g.fill(*_prose);
        g.pop();
    }

    void layout(Math::Recti bound) override {
        _prose->layout(Au{bound.width});
        View<Text>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _prose->layout(Au{s.width});
        return size.ceil().cast<isize>();
    }
};

Child text(marK::Text::ProseStyle style, Str text) {
    return makeRc<Text>(style, text);
}

Child text(Str text) {
    return makeRc<Text>(TextStyles::labelMedium(), text);
}

Child text(Rc<marK::Text::Prose> prose) {
    return makeRc<Text>(prose);
}

// MARK: Icon ------------------------------------------------------------------

struct Icon : public View<Icon> {
    Gfx::Icon _icon;
    Opt<Gfx::Color> _color;

    Icon(Gfx::Icon icon, Opt<Gfx::Color> color)
        : _icon(icon), _color(color) {}

    void reconcile(Icon& o) override {
        _icon = o._icon;
        _color = o._color;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        if (_color)
            g.fillStyle(_color.unwrap());
        _icon.fill(g, bound().topStart());
        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _icon.bound().size().cast<isize>();
    }
};

Child icon(Gfx::Icon icon, Opt<Gfx::Color> color) {
    return makeRc<Icon>(icon, color);
}

Child icon(Mdi::Icon i, f64 size, Opt<Gfx::Color> color) {
    return icon(Gfx::Icon{i, size}, color);
}

// MARK: Image -----------------------------------------------------------------

struct Image : public View<Image> {
    marK::Image::Picture _image;
    Opt<Math::Radiif> _radii;

    Image(marK::Image::Picture image, Opt<Math::Radiif> radii = NONE)
        : _image(image), _radii(radii) {}

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();

        if (_radii) {
            g.fillStyle(_image.pixels());
            g.fill(bound(), *_radii);
        } else {
            g.blit(bound(), _image);
        }

        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _image.bound().size().cast<isize>();
    }
};

Child image(marK::Image::Picture image) {
    return makeRc<Image>(image);
}

Child image(marK::Image::Picture image, Math::Radiif radii) {
    return makeRc<Image>(image, radii);
}

// MARK: Canvas ----------------------------------------------------------------

struct Canvas : public View<Canvas> {
    OnPaint _onPaint;

    Canvas(OnPaint onPaint)
        : _onPaint(std::move(onPaint)) {}

    void reconcile(Canvas& o) override {
        _onPaint = std::move(o._onPaint);
        View<Canvas>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(_bound);
        g.origin(_bound.xy.cast<f64>());
        _onPaint(g, _bound.wh);
        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint hint) override {
        if (hint == Hint::MIN)
            return 0;
        return _bound.wh;
    }
};

Child canvas(OnPaint onPaint) {
    return makeRc<Canvas>(std::move(onPaint));
}

struct SceneCanvas : public View<SceneCanvas> {
    Rc<Scene::Node> _scene;
    Scene::PaintOptions _options;

    SceneCanvas(Rc<Scene::Node> scene, Scene::PaintOptions options)
        : _scene(std::move(scene)), _options(options) {}

    void reconcile(SceneCanvas& o) override {
        _scene = o._scene;
        _options = o._options;
        View<SceneCanvas>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti rect) override {
        g.push();
        g.clip(_bound);
        g.origin(_bound.xy.cast<f64>());
        g.scale(_bound.size().cast<f64>() / _scene->bound().size().cast<f64>());

        Math::Trans2f trans;
        trans = trans.translated(-_bound.xy.cast<f64>());
        trans = trans.scaled(_bound.size().cast<f64>() / _scene->bound().size().cast<f64>());

        auto rectInScene = trans.inverse().apply(rect.cast<f64>()).bound();

        _scene->paint(g, Math::Rectf::MAX, _options);

        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint hint) override {
        if (hint == Hint::MIN) {
            return 0;
        }

        return _scene->bound().size().ceil().cast<isize>();
    }
};

Child canvas(Rc<Scene::Node> child, Scene::PaintOptions options) {
    return makeRc<SceneCanvas>(std::move(child), options);
}

// MARK: Filter ----------------------------------------------------------------

struct BackgroundFilter : public ProxyNode<BackgroundFilter> {
    Gfx::Filter _filter;

    BackgroundFilter(Gfx::Filter filter, Child child)
        : ProxyNode<BackgroundFilter>(std::move(child)),
          _filter(filter) {}

    void reconcile(BackgroundFilter& o) override {
        _filter = o._filter;
        ProxyNode<BackgroundFilter>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        g.apply(_filter);
        g.pop();
        ProxyNode<BackgroundFilter>::paint(g, r);
    }
};

Child backgroundFilter(Gfx::Filter f, Child child) {
    return makeRc<BackgroundFilter>(f, std::move(child));
}

struct ForegroundFilter : public ProxyNode<ForegroundFilter> {
    Gfx::Filter _filter;

    ForegroundFilter(Gfx::Filter filter, Child child)
        : ProxyNode<ForegroundFilter>(std::move(child)),
          _filter(filter) {}

    void reconcile(ForegroundFilter& o) override {
        _filter = o._filter;
        ProxyNode<ForegroundFilter>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        ProxyNode<ForegroundFilter>::paint(g, r);
        g.push();
        g.clip(bound());
        g.apply(_filter);
        g.pop();
    }
};

Child foregroundFilter(Gfx::Filter f, Child child) {
    return makeRc<ForegroundFilter>(f, std::move(child));
}

} // namespace marK::Ui

#pragma once

#include <karm-ui/node.h>
#include <vaev-markup/dom.h>

namespace PaperMuncher::Inspector {

Ui::Child app(Mime::Url url, Res<Strong<Vaev::Markup::Document>> dom);

} // namespace PaperMuncher::Inspector

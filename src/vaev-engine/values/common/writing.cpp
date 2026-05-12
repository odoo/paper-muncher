export module Vaev.Engine:values.common.writing;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

// https://www.w3.org/TR/css-writing-modes-3/#propdef-writing-mode
export enum struct WritingMode : u8 {
    HORIZONTAL_TB,
    VERTICAL_RL,
    VERTICAL_LR,
};

// https://www.w3.org/TR/css-writing-modes-3/#propdef-direction
export enum struct Direction : u8 {
    LTR,
    RTL,
};

} // namespace Vaev::Experimental

export module Html5LibTest:testResult;

import Karm.Core;

namespace Html5LibTest {

export struct Result {
    Karm::Serde::Value reference;
    Karm::Serde::Array actual;
};

} // namespace Html5LibTest

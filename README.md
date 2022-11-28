# NcCommon
[![Build](https://github.com/NcStudios/NcCommon/actions/workflows/build.yml/badge.svg)](https://github.com/NcStudios/NcCommon/actions?query=workflow%3ABuild)
[![License](https://img.shields.io/github/license/McCallisterRomer/NCEngine.svg)](https://github.com/McCallisterRomer/NcCommon/blob/vnext/LICENSE)

NcCommon consists of several libraries providing functionality that needs to be shared across NcStudios repos. These include `NcMath` and `NcUtility`, as well as two third-party libraries: [DirectXMath](https://github.com/microsoft/DirectXMath) and [{fmt}](https://github.com/fmtlib/fmt).

NcStudios repos should consume NcCommon via `FetchContent()` targeting the `release` branch.

### Build Options
    NC_COMMON_BUILD_TESTS (default OFF)

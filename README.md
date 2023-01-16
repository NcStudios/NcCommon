# NcCommon
[![Build](https://github.com/NcStudios/NcCommon/actions/workflows/build.yml/badge.svg)](https://github.com/NcStudios/NcCommon/actions?query=workflow%3ABuild)
[![License](https://img.shields.io/github/license/McCallisterRomer/NCEngine.svg)](https://github.com/McCallisterRomer/NcCommon/blob/vnext/LICENSE)

NcCommon consists of several libraries providing functionality that needs to be shared across NcStudios repos. These include `NcMath` and `NcUtility`, as well as the following third-party libraries: [DirectXMath](https://github.com/microsoft/DirectXMath), [{fmt}](https://github.com/fmtlib/fmt), and [nlohmann json](https://github.com/nlohmann/json).

NcStudios repos should consume NcCommon via `FetchContent()` targeting the `release` branch.

### Build Options
    NC_COMMON_BUILD_TESTS (default OFF)

    NC_COMMON_STATIC_ANALYSIS (default OFF)
        Note: MSVC Only
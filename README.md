# NcCommon
[![Build](https://github.com/NcStudios/NcCommon/actions/workflows/build.yml/badge.svg)](https://github.com/NcStudios/NcCommon/actions?query=workflow%3ABuild)
[![License](https://img.shields.io/github/license/McCallisterRomer/NCEngine.svg)](https://github.com/McCallisterRomer/NcCommon/blob/vnext/LICENSE)

NcCommon is a library containing general functionality which is shared across NcStudios repos. The following third-party libraries are also included:
- [DirectXMath](https://github.com/microsoft/DirectXMath)
- [{fmt}](https://github.com/fmtlib/fmt)

NcStudios repos should consume NcCommon via `FetchContent()` targeting the `release` branch.

### Build Options
    NC_COMMON_BUILD_TESTS (default OFF)

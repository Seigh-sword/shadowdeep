#pragma once
#include <cstdint>
#include <string_view>

namespace shadowdeep {

constexpr std::string_view kGameVersion = "Zv1";
constexpr std::string_view kVersionHuman = "Zero v1";
constexpr std::string_view kVersionEra = "Zero";
constexpr int kVersionEraNumber = 1;

constexpr uint16_t kSaveSchemaVersion = 1;
constexpr uint16_t kConfigSchemaVersion = 4;
constexpr uint16_t kContainerFormatVersion = 1;

constexpr uint32_t kBuildRevision = 1;

constexpr std::string_view kProjectName = "SHADOWDEEP";
constexpr std::string_view kAuthor = "github.com/Seigh-sword";
constexpr std::string_view kRepositoryUrl = "https://github.com/Seigh-sword/shadowdeep";
constexpr std::string_view kLicenseId = "ISC";

}

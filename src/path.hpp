#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> retrievePath();
std::optional<std::string> findExecutable(
    std::string_view command, const std::vector<std::string>& paths);

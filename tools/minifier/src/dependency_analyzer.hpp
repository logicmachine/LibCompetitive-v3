#pragma once
#include "clang/Tooling/Tooling.h"
#include "removal_table.hpp"

RemovalTable analyze_dependency(clang::tooling::ClangTool &tool);


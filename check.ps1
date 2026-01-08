# ============================================
# check_msvc_sdk.ps1
# Windows MSVC + Windows SDK + kernel32.lib 环境检测
# ============================================

# 设置输出编码为 UTF-8，避免中文乱码
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

function Check-VS {
    Write-Host "=== 检查 Visual Studio 安装 ==="
    $vswherePath = "C:\Tools\vswhere.exe"
    if (-Not (Test-Path $vswherePath)) {
        Write-Host "vswhere.exe 未找到，请安装 Visual Studio Installer" -ForegroundColor Red
        return $null
    }

    $vsInstalls = & $vswherePath -all -products * `
        -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
        -property installationPath

    if ($vsInstalls.Count -eq 0) {
        Write-Host "未找到带有 C++ 工具链的 Visual Studio 实例" -ForegroundColor Red
        return $null
    }

    Write-Host "找到 Visual Studio 实例路径：" -ForegroundColor Green
    $vsInstalls | ForEach-Object { Write-Host "  $_" }
    return $vsInstalls[0]
}

function Check-MSVC($vsPath) {
    Write-Host "`n=== 检查 MSVC 编译器 ==="
    $msvcDir = Join-Path $vsPath "VC\Tools\MSVC"
    if (-Not (Test-Path $msvcDir)) {
        Write-Host "MSVC 工具链目录未找到: $msvcDir" -ForegroundColor Red
        return $null
    }

    $msvcVersion = Get-ChildItem $msvcDir | Sort-Object Name -Descending | Select-Object -First 1
    $clPath = Join-Path $msvcVersion.FullName "bin\Hostx64\x64\cl.exe"
    $linkPath = Join-Path $msvcVersion.FullName "bin\Hostx64\x64\link.exe"

    if (Test-Path $clPath) { Write-Host "cl.exe 存在: $clPath" -ForegroundColor Green }
    else { Write-Host "cl.exe 未找到" -ForegroundColor Red }

    if (Test-Path $linkPath) { Write-Host "link.exe 存在: $linkPath" -ForegroundColor Green }
    else { Write-Host "link.exe 未找到" -ForegroundColor Red }

    return @{ "MSVCVersion" = $msvcVersion.Name; "cl" = $clPath; "link" = $linkPath }
}

function Check-WindowsSDK {
    Write-Host "`n=== 检查 Windows SDK ==="
    $sdkIncludeRoot = "C:\Program Files (x86)\Windows Kits\10\Include"
    $sdkLibRoot = "C:\Program Files (x86)\Windows Kits\10\Lib"

    if (-Not (Test-Path $sdkIncludeRoot)) { Write-Host "Include 目录未找到: $sdkIncludeRoot" -ForegroundColor Red }
    else { Write-Host "Include 目录存在: $sdkIncludeRoot" -ForegroundColor Green }

    if (-Not (Test-Path $sdkLibRoot)) { Write-Host "Lib 目录未找到: $sdkLibRoot" -ForegroundColor Red }
    else { Write-Host "Lib 目录存在: $sdkLibRoot" -ForegroundColor Green }

    # 尝试找到最新版本的 SDK
    $sdkVersions = Get-ChildItem $sdkIncludeRoot | Sort-Object Name -Descending
    if ($sdkVersions.Count -eq 0) {
        Write-Host "未找到任何 Windows SDK 版本" -ForegroundColor Red
        return $null
    }

    $latestVersion = $sdkVersions[0].Name
    Write-Host "检测到最新 Windows SDK 版本: $latestVersion" -ForegroundColor Green

    $kernel32Lib = Join-Path $sdkLibRoot "$latestVersion\um\x64\kernel32.lib"
    if (Test-Path $kernel32Lib) { Write-Host "找到 kernel32.lib: $kernel32Lib" -ForegroundColor Green }
    else { Write-Host "未找到 kernel32.lib，请检查 SDK 安装" -ForegroundColor Red }

    return @{ "SDKVersion" = $latestVersion; "Kernel32" = $kernel32Lib }
}

# ========== 执行检测 ==========
$vsPath = Check-VS
if ($vsPath -ne $null) {
    $msvcInfo = Check-MSVC $vsPath
}

$sdkInfo = Check-WindowsSDK

Write-Host "`n=== 检测完成 ==="

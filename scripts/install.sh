#!/bin/bash
# unipm installer for Linux/macOS
# Download from: curl -fsSL https://raw.githubusercontent.com/ieee-cs-bmsit/unipm/main/scripts/install.sh | bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
REPO="ieee-cs-bmsit/unipm"
INSTALL_DIR="${UNIPM_INSTALL_DIR:-$HOME/.local/bin}"
VERSION="${UNIPM_VERSION:-latest}"
BINARY_NAME="unipm"

# Functions
log_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

log_success() {
    echo -e "${GREEN}✓${NC} $1"
}

log_error() {
    echo -e "${RED}✗${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# Detect OS and architecture
detect_platform() {
    log_info "Detecting platform..."
    
    OS="$(uname -s)"
    ARCH="$(uname -m)"
    
    case "$OS" in
        Linux*)
            OS="linux"
            log_info "OS: Linux"
            ;;
        Darwin*)
            OS="macos"
            log_info "OS: macOS"
            ;;
        *)
            log_error "Unsupported operating system: $OS"
            log_info "unipm supports Linux and macOS. For Windows, use install.ps1"
            exit 1
            ;;
    esac
    
    case "$ARCH" in
        x86_64)
            ARCH="x64"
            log_info "Architecture: x64"
            ;;
        aarch64|arm64)
            ARCH="arm64"
            log_info "Architecture: ARM64"
            ;;
        *)
            log_error "Unsupported architecture: $ARCH"
            log_info "unipm supports x64 and ARM64 architectures"
            exit 1
            ;;
    esac
}

# Check dependencies
check_dependencies() {
    if ! command -v curl &> /dev/null; then
        log_error "curl is required but not installed"
        log_info "Install curl and try again"
        exit 1
    fi
    
    if ! command -v tar &> /dev/null && ! command -v unzip &> /dev/null; then
        log_warning "Neither tar nor unzip found, trying direct binary download"
    fi
}

# Get latest release version from GitHub
get_latest_version() {
    if [ "$VERSION" = "latest" ]; then
        log_info "Fetching latest version..."
        VERSION=$(curl -fsSL "https://api.github.com/repos/$REPO/releases/latest" | grep '"tag_name"' | sed -E 's/.*"v?([^"]+)".*/\1/')
        
        if [ -z "$VERSION" ]; then
            log_error "Failed to fetch latest version"
            log_info "You can manually specify a version: UNIPM_VERSION=1.0.0 curl ... | bash"
            exit 1
        fi
        
        log_success "Latest version: v$VERSION"
    fi
}

# Download and install binary
download_and_install() {
    log_info "Preparing installation to $INSTALL_DIR..."
    
    # Create installation directory
    mkdir -p "$INSTALL_DIR"
    
    # Construct download URL
    BINARY_FILE="unipm-$OS-$ARCH"
    DOWNLOAD_URL="https://github.com/$REPO/releases/download/v$VERSION/$BINARY_FILE"
    CHECKSUM_URL="https://github.com/$REPO/releases/download/v$VERSION/$BINARY_FILE.sha256"
    
    log_info "Downloading from $DOWNLOAD_URL..."
    
    # Download binary
    TEMP_FILE=$(mktemp)
    if ! curl -fsSL "$DOWNLOAD_URL" -o "$TEMP_FILE"; then
        log_error "Failed to download unipm binary"
        log_info "Check if version v$VERSION exists at https://github.com/$REPO/releases"
        rm -f "$TEMP_FILE"
        exit 1
    fi
    
    log_success "Downloaded successfully"
    
    # Download and verify checksum if available
    if curl -fsSL "$CHECKSUM_URL" -o "$TEMP_FILE.sha256" 2>/dev/null; then
        log_info "Verifying checksum..."
        
        cd "$(dirname "$TEMP_FILE")"
        EXPECTED_CHECKSUM=$(cat "$TEMP_FILE.sha256" | awk '{print $1}')
        
        if command -v sha256sum &> /dev/null; then
            ACTUAL_CHECKSUM=$(sha256sum "$(basename "$TEMP_FILE")" | awk '{print $1}')
        elif command -v shasum &> /dev/null; then
            ACTUAL_CHECKSUM=$(shasum -a 256 "$(basename "$TEMP_FILE")" | awk '{print $1}')
        else
            log_warning "No SHA256 tool found, skipping checksum verification"
            ACTUAL_CHECKSUM="$EXPECTED_CHECKSUM"
        fi
        
        if [ "$EXPECTED_CHECKSUM" = "$ACTUAL_CHECKSUM" ]; then
            log_success "Checksum verified"
        else
            log_error "Checksum verification failed!"
            log_error "Expected: $EXPECTED_CHECKSUM"
            log_error "Actual: $ACTUAL_CHECKSUM"
            rm -f "$TEMP_FILE" "$TEMP_FILE.sha256"
            exit 1
        fi
        
        rm -f "$TEMP_FILE.sha256"
    else
        log_warning "Checksum not available, skipping verification"
    fi
    
    # Install binary
    mv "$TEMP_FILE" "$INSTALL_DIR/$BINARY_NAME"
    chmod +x "$INSTALL_DIR/$BINARY_NAME"
    
    log_success "Installed to $INSTALL_DIR/$BINARY_NAME"
}

# Update PATH in shell configuration
update_path() {
    log_info "Configuring PATH..."
    
    # Check if already in PATH
    if echo "$PATH" | grep -q "$INSTALL_DIR"; then
        log_success "PATH already configured"
        return
    fi
    
    PATH_EXPORT="export PATH=\"$INSTALL_DIR:\$PATH\""
    UPDATED_FILES=""
    
    # Update shell configuration files
    for RC_FILE in "$HOME/.bashrc" "$HOME/.bash_profile" "$HOME/.zshrc" "$HOME/.profile"; do
        if [ -f "$RC_FILE" ]; then
            if ! grep -q "$INSTALL_DIR" "$RC_FILE" 2>/dev/null; then
                echo "" >> "$RC_FILE"
                echo "# Added by unipm installer" >> "$RC_FILE"
                echo "$PATH_EXPORT" >> "$RC_FILE"
                UPDATED_FILES="$UPDATED_FILES $RC_FILE"
                log_success "Updated $(basename "$RC_FILE")"
            fi
        fi
    done
    
    if [ -z "$UPDATED_FILES" ]; then
        log_warning "No shell configuration files found"
        log_info "Add this to your shell config manually:"
        echo "    $PATH_EXPORT"
    fi
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Add to current PATH temporarily
    export PATH="$INSTALL_DIR:$PATH"
    
    if ! command -v unipm &> /dev/null; then
        log_error "Installation verification failed"
        log_info "unipm binary not found in PATH"
        exit 1
    fi
    
    INSTALLED_VERSION=$("$INSTALL_DIR/$BINARY_NAME" --version 2>&1 | head -n1)
    log_success "Installation verified: $INSTALLED_VERSION"
}

# Print success message
print_success() {
    echo ""
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}  ✓ unipm installed successfully!${NC}"
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "📦 Version: ${BLUE}v$VERSION${NC}"
    echo -e "📁 Location: ${BLUE}$INSTALL_DIR/$BINARY_NAME${NC}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo "  1. Restart your terminal or run:"
    echo "     source ~/.bashrc  # or ~/.zshrc"
    echo ""
    echo "  2. Verify installation:"
    echo "     unipm --version"
    echo ""
    echo "  3. Get started:"
    echo "     unipm install docker"
    echo "     unipm help"
    echo ""
    echo -e "📚 Documentation: ${BLUE}https://github.com/$REPO${NC}"
    echo -e "💬 Issues: ${BLUE}https://github.com/$REPO/issues${NC}"
    echo ""
}

# Cleanup on error
cleanup() {
    if [ $? -ne 0 ]; then
        log_error "Installation failed"
        log_info "Check the error messages above"
        log_info "For help, visit: https://github.com/$REPO/issues"
    fi
}

trap cleanup EXIT

# Main installation flow
main() {
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  unipm installer"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    
    check_dependencies
    detect_platform
    get_latest_version
    download_and_install
    update_path
    verify_installation
    print_success
}

main

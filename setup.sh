#!/usr/bin/env bash

set -eou pipefail

readonly c_REPOSITORY_URL="https://github.com/tyleruploads/mdez.git"
readonly c_APPIMAGE_URL="https://github.com/tyleruploads/mdez/releases/latest/download/MDEZ++-x86_64.AppImage"
readonly c_APPIMAGE_NAME=$(basename "$c_APPIMAGE_URL")
readonly c_ICON_URL="https://raw.githubusercontent.com/tyleruploads/mdez/refs/heads/main/data/io.github.tyleruploads.mdez.png"
readonly c_ICON_NAME=$(basename "$c_ICON_URL")

readonly COLOR_RESET="\033[0m"
readonly COLOR_BOLD="\033[1m"
readonly COLOR_RED="\033[31m"
readonly COLOR_GREEN="\033[32m"
readonly COLOR_BLUE="\033[34m"
readonly COLOR_CYAN="\033[36m"
readonly COLOR_YELLOW="\033[33m"
readonly COLOR_ORANGE="\033[38;2;255;165;0m"

readonly VERSION=$(curl -s https://api.github.com/repos/tyleruploads/mdez/releases/latest | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')

case "${1:-}" in
    --version|-v)
        echo "Target Version: MDEZ++ ${VERSION}+dev"
        exit 0
        ;;
    --help|-h)
        echo "Usage: ./setup.sh [OPTION]"
        echo ""
        echo "Options:"
        echo "    -v, --version     Show version information"
        echo "    -h, --version     Show this help message"
        echo ""
        echo "Running without options starts interactive installer menu"
        exit 0
        ;;
    -*)
        echo -e "${COLOR_RED}Error: Unknown option '$1'${COLOR_RESET}"
        echo -e "${COLOR_RED}Run './setup.sh --help' for usage.${COLOR_RESET}"
        exit 1
        ;;
esac

print_div() {
    local char="${1:-=}"

    printf '%*s\n' "$(tput cols)" '' | tr ' ' "$char"
}

trap 'echo -e "\n${COLOR_RED}Installation aborted.${COLOR_RESET}"; exit 1' INT TERM

clear

print_div
echo -e "${COLOR_BLUE}${COLOR_BOLD}MDEZ++ Installer${COLOR_RESET}"
echo -e "${COLOR_ORANGE}MDEZ++ (mdez) is an open-source, lightweight, cross-platform C++20 and Qt GUI Markdown editor.\nInstead of a WYSIWYG editor, MDEZ++ renders a real-time live preview of your text in a split-screen pane using a fast, native, and minimal GUI.${COLOR_RESET}"
print_div
echo -e "${COLOR_GREEN}Maintainer     : Tyler N. (tyleruploads)${COLOR_RESET}"
echo -e "${COLOR_GREEN}Repository     : https://github.com/tyleruploads/mdez${COLOR_RESET}"
echo -e "${COLOR_GREEN}Target Version : ${VERSION}+dev${COLOR_RESET}"
echo -e "${COLOR_GREEN}Script Source  : https://github.com/tyleruploads/mdez/blob/main/setup.sh${COLOR_RESET}"
echo -e "${COLOR_GREEN}License        : Open Source (MIT) ${COLOR_RESET}"
print_div

if [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "${COLOR_RED}Error: Installer currently not supported on macOS${COLOR_RESET}" >&2
fi

if ! sudo -n true 2>/dev/null; then
    echo -e "${COLOR_YELLOW}This script is not currently running with administrative rights. You will be asked to authorize administrative privileges if necessary.${COLOR_RESET}"
    echo -e "${COLOR_YELLOW}If the script fails with any sort of permission error, rerun it with sudo, even if you already provided the administrator password before the failure.${COLOR_RESET}"
    sleep 2
fi

options=("Install Remote AppImage" "Compile Locally and Install" "Uninstall MDEZ++" "Exit")
PS3="Please enter your choice (1-4): "

echo -e "\n"

select opt in "${options[@]}"; do
    case $opt in
        "Install Remote AppImage")
            echo -e "${COLOR_CYAN}[1/4] Downloading AppImage from GitHub Releases${COLOR_RESET}"

            if ! curl -fsSL --retry 5 --retry-delay 2 -o "$c_APPIMAGE_NAME" "$c_APPIMAGE_URL"; then
                echo -e "${COLOR_RED}Error: Download failed entirely (network, 404, or GitHub error)${COLOR_RESET}" >&2
                exit 1
            fi

            if [ ! -s "$c_APPIMAGE_NAME" ]; then
                echo -e "${COLOR_RED}Error: Download resulted in an empty file.${COLOR_RESET}" >&2
                rm -f "$c_APPIMAGE_NAME"
                exit 1
            fi

            chmod +x "$c_APPIMAGE_NAME"

            echo -e "${COLOR_CYAN}[2/4] Installing Icon${COLOR_RESET}"

            if ! curl -fsSL --retry 5 --retry-delay 2 -o "$c_ICON_NAME" "$c_ICON_URL"; then
                echo -e "${COLOR_RED}Error: Download failed entirely (network, 404, or GitHub error)${COLOR_RESET}" >&2
                exit 1
            fi

            if [ ! -s "$c_ICON_NAME" ]; then
                echo -e "${COLOR_RED}Error: Download resulted in an empty file.${COLOR_RESET}" >&2
                rm -f "$c_ICON_NAME"
                exit 1
            fi

            echo -e "${COLOR_CYAN}[3/4] Installing AppImage to /usr/local/bin${COLOR_RESET}"

            sudo mkdir -p /usr/local/bin
            sudo mv "$c_APPIMAGE_NAME" /usr/local/bin/mdez

            LINE='export PATH="/usr/local/bin:$PATH"'

            TARGET_BASHRC="${SUDO_USER:+$(getent passwd "$SUDO_USER" | cut -d: -f6)}/.bashrc"
            [ -z "${SUDO_USER:-}" ] && TARGET_BASHRC="$HOME/.bashrc"

            if ! grep -qxF "$LINE" "$TARGET_BASHRC"; then
                echo "$LINE" >> "$TARGET_BASHRC"
                echo "${COLOR_GREEN}/usr/local/bin has been added to ~/.bashrc${COLOR_RESET}"
            fi

            echo -e "${COLOR_CYAN}[4/4] Installing desktop entry point to /usr/local/share/applications${COLOR_RESET}"

            sudo mkdir -p /usr/local/share/icons/hicolor/16x16/apps/
            sudo cp "$c_ICON_NAME" /usr/local/share/icons/hicolor/16x16/apps/mdez.png
            rm -f "$c_ICON_NAME"

            sudo mkdir -p /usr/local/share/applications
            sudo tee /usr/local/share/applications/mdez.desktop > /dev/null << 'EOF'
[Desktop Entry]
Type=Application
Name=MDEZ++
Comment=A lightweight C++20 and Qt GUI Markdown editor
Exec=/usr/local/bin/mdez
Icon=mdez
Terminal=false
Categories=Utility;TextEditor;
EOF


            sudo chmod +x /usr/local/share/applications/mdez.desktop

            sudo update-desktop-database /usr/local/share/applications
            sudo gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor/ || true

            echo -e "${COLOR_GREEN}MDEZ++ AppImage has been successfully installed. You can run it at any time with the mdez command.${COLOR_RESET}"
            echo -e "${COLOR_GREEN}To update your shell, run: source ~/.bashrc${COLOR_RESET}"

            ;;
        "Compile Locally and Install")
            echo -e "${COLOR_CYAN}[1/4] Installing dependencies${COLOR_RESET}\n"

            if command -v apt-get &>/dev/null; then
                echo -e "${COLOR_BOLD}Using apt-get...${COLOR_RESET}"
                sudo apt-get update -y
                sudo apt-get install -y build-essential git qt6-base-dev qt6-base-dev-tools cmake
            elif command -v dnf &>/dev/null; then
                echo -e "${COLOR_BOLD}Using dnf...${COLOR_RESET}"
                sudo dnf groupinstall -y "Development Tools"
                sudo dnf install -y git cmake qt6-qtbase-devel
            elif command -v pacman &>/dev/null; then
                echo -e "${COLOR_BOLD}Using pacman...${COLOR_RESET}"
                sudo pacman -S --noconfirm --needed base-devel git cmake qt6-base xcb-util-cursor
            elif command -v zypper &>/dev/null; then
                echo -e "${COLOR_BOLD}Using zypper...${COLOR_RESET}"
                sudo zypper refresh
                sudo zypper install -y -t pattern devel_basis
                sudo zypper install -y git cmake qt6-base-devel
            else
                echo -e "${COLOR_RED}Error: No supported package manager (apt-get, dnf, pacman, zypper) found.${COLOR_RESET}"
                exit 1
            fi

            echo -e "${COLOR_CYAN}[2/4] Cloning GitHub Repository${COLOR_RESET}\n"

            if [ -d ".git" ] && [[ "$(git config --get remote.origin.url)" == "$c_REPOSITORY_URL" ]]; then
                echo -e "${COLOR_BOLD}Already inside mdez repository. Updating...${COLOR_RESET}"
                git pull
            else
                if [ -d "mdez" ]; then
                    echo -e "${COLOR_BOLD}Directory mdez already exists. Updating...${COLOR_RESET}"
                    cd "mdez"
                    git pull
                else
                    echo -e "${COLOR_BOLD}Cloning mdez repository${COLOR_RESET}"
                    git clone "$c_REPOSITORY_URL"
                    cd "mdez"
                fi
            fi

            echo -e "${COLOR_CYAN}[3/4] Compiling MDEZ++${COLOR_RESET}\n"

            rm -rf build # Removes build directory in case it already exists to prevent errors
            mkdir -p build
            cd build

            cmake ..
            cmake --build . --parallel

            echo -e "${COLOR_CYAN}[4/4] Installing MDEZ++${COLOR_RESET}\n"
            sudo cmake --install .

            echo -e "${COLOR_GREEN}Compilation successful! MDEZ++ can be ran with the 'mdez' command in the terminal. A desktop entry point has also been made.${COLOR_RESET}"
            mdez

            ;;
        "Uninstall MDEZ++")
            c_MDEZ_PATH=$(command -v mdez || true)

            if [ -n "$c_MDEZ_PATH" ]; then
                echo -e "${COLOR_CYAN}MDEZ found at: $c_MDEZ_PATH. Uninstalling...${COLOR_RESET}"
                sudo rm -f "$c_MDEZ_PATH"
                echo -e "${COLOR_GREEN}MDEZ successfully uninstalled.${COLOR_RESET}"
            else
                if [ -f "/usr/local/bin/mdez" ]; then
                    echo -e "${COLOR_CYAN}MDEZ found at: /usr/local/bin/mdez. Uninstalling...${COLOR_RESET}"
                    sudo rm -f "/usr/local/bin/mdez"
                    echo -e "${COLOR_CYAN}MDEZ successfully uninstalled.${COLOR_RESET}"
                else
                    echo -e "${COLOR_RED}MDEZ not found. Not uninstalling.${COLOR_RESET}" >&2
                fi
            fi

            sudo rm -f /usr/local/share/icons/hicolor/16x16/apps/mdez.png
            sudo rm -f /usr/local/share/applications/mdez.desktop

            echo -e "${COLOR_BOLD}${COLOR_GREEN}To update your shell, run: source ~/.bashrc${COLOR_RESET}"

            ;;
        "Exit")
            break
            ;;
        *)
            echo -e "${COLOR_RED}Invalid option.${COLOR_RESET}"
            ;;
    esac
done



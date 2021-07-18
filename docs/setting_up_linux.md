# Setting up development environment on Linux

## Install needed system commands and dependencies:

### Ubuntu
```sh
sudo apt install git python3 python3-pip python3-setuptools python3-wheel ninja-build libglfw3-dev cmake libfmt-dev libtbb-dev clang
```

### Fedora
```sh
sudo dnf install git python3 python3-pip python3-setuptools python3-wheel ninja-build glfw-devel cmake fmt-devel tbb-devel clang
```

## Clone repository
```sh
git clone --recursive https://github.com/enoxum/UbiBelPMF_2021.git
```

## Lets grab ourselves the latest version of meson:

```sh
sudo pip3 install meson
```

## Grab an IDE:

### Ubuntu
```sh
sudo snap install --classic code
```

### Fedora
```sh
sudo rpm --import https://packages.microsoft.com/keys/microsoft.asc
sudo sh -c 'echo -e "[code]\nname=Visual Studio Code\nbaseurl=https://packages.microsoft.com/yumrepos/vscode\nenabled=1\ngpgcheck=1\ngpgkey=https://packages.microsoft.com/keys/microsoft.asc" > /etc/yum.repos.d/vscode.repo'
sudo dnf install code
```

## Install C++ and Meson extensions for our IDE:

```sh
code --install-extension ms-vscode.cpptools
code --install-extension asabil.meson
```

## Open dagger folder in VS Code, press F5, and profit!

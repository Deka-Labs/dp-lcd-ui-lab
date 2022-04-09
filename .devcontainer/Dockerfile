# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.217.4/containers/ubuntu/.devcontainer/base.Dockerfile

# Download STM32 package
ARG VARIANT="hirsute"
ARG CUBE_MCU_NAME="F3"
ARG CUBE_PACKAGE_VERSION="1.11.3"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT} as stm32cubedownloader
ARG CUBE_MCU_NAME
ARG CUBE_PACKAGE_VERSION

RUN wget https://github.com/STMicroelectronics/STM32Cube${CUBE_MCU_NAME}/archive/refs/tags/v${CUBE_PACKAGE_VERSION}.tar.gz \
    && tar -xf ./v${CUBE_PACKAGE_VERSION}.tar.gz -C /opt


# [Choice] Ubuntu version (use hirsuite or bionic on local arm64/Apple Silicon): hirsute, focal, bionic

FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}
ARG CUBE_MCU_NAME
ARG CUBE_PACKAGE_VERSION

# Install packages
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends gdb-multiarch binutils-arm-none-eabi gcc-arm-none-eabi libnewlib-arm-none-eabi cmake build-essential wget openocd stlink-tools 

COPY --from=stm32cubedownloader /opt/STM32Cube${CUBE_MCU_NAME}-${CUBE_PACKAGE_VERSION} /opt/STM32Cube${CUBE_MCU_NAME}

# Set location of Cube
ENV STM32_CUBE_${CUBE_MCU_NAME}_PATH=/opt/STM32Cube${CUBE_MCU_NAME}



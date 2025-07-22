#FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11
FROM mcr.microsoft.com/devcontainers/cpp:1.2.3-ubuntu24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libboost-all-dev python3-pip  python3.12-venv vim
RUN python3 -m venv /opt/conan
RUN /opt/conan/bin/pip install conan
RUN su vscode -c "/opt/conan/bin/conan profile detect"
RUN su vscode  -c "echo \"set -o vi\" >> ~/.bashrc"

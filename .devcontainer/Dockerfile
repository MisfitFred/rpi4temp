FROM fred78/raspberry-pi-pico-sdk


ARG USERNAME=dev
ARG USER_UID=1010
ARG USER_GID=$USER_UID

run apk add bash bash-completion 

# Create the user
RUN addgroup --gid "$USER_GID" "$USERNAME"  &&\
     adduser \
    --disabled-password \
    --gecos "" \
    --home /home/dev \
    --shell /bin/bash \
    --ingroup "$USERNAME" \
    --no-create-home \
    --uid "$USER_UID" \
    "$USERNAME"

# ********************************************************
# * Anything else you want to do like clean up goes here *
# ***************************** ***************************

# [Optional] Set the default user. Omit if you want to keep the default as root.
USER $USERNAME
# [Optional] Uncomment this section to install additional vcpkg ports.
#RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

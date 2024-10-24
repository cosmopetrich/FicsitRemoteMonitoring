#! /usr/bin/env bash

set -eux -o pipefail

# Install the generator and run 'version' so that it downloads the appropriate Java (not JS) cruft
# Currently disabled and unused
# Add "features": { "ghcr.io/devcontainers/features/java:1": {} } to devcontainer.json if required.
# npm install -g @openapitools/openapi-generator-cli
# openapi-generator-cli version 

# Put YQ in the $PATH so we can use it for YAML validation and converting to JSON
mkdir -p ${HOME}/.local/bin
wget https://github.com/mikefarah/yq/releases/latest/download/yq_linux_amd64 -O ${HOME}/.local/bin/yq
chmod +x ${HOME}/.local/bin/yq 
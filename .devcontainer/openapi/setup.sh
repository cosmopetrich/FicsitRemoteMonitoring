#! /usr/bin/env bash

set -eu -o pipefail

# Install the generator and run 'version' so that it downloads the appropriate Java (not JS) cruft
npm install -g @openapitools/openapi-generator-cli
openapi-generator-cli version 

# Put YQ in the $PATH so we can use it for YAML validation and converting to JSON
mkdir -p ${HOME}/.local/bin
wget https://github.com/mikefarah/yq/releases/latest/download/yq_linux_amd64 -O ${HOME}/.local/bin/yq
chmod +x ${HOME}/.local/bin/yq 

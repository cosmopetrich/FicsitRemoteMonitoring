This folder contains FRM's HTTP API definitions, written using [OpenAPI](https://www.openapis.org/).

## Structure

The [openapi.yaml](openapi.yaml) file provides the base API definition and references the other `openapi-*.yaml` files using [the $ref keyword](https://swagger.io/docs/specification/v3_0/using-ref/). Breaking them up in this way is not strictly necessary, but does keep the files from being too unweildy.

- `openapi-schemas-common.yaml` - Common data types which are used across multiple API paths.
- `openapi-paths-*.yaml` - API path definitions. Each file approximately matches the functions defined in [Source/FicsitRemoteMonitoring/Private/*.cpp](Source/FicsitRemoteMonitoring/Private/).

The main openapi.yaml must adhere to the OpenAPI specification. While the other files can be a little more free-form and are required to have specific top-level keys, please attempt to keep the contents of these files under [the components key](https://swagger.io/specification/#components-object) so that we can merge them easily if required.

Note that FRM uses OpenAPI 3.1.0 - the latest version as of October 2024. Some public documentation may still reference 3.0.x and the [notes on what has changed between the two versions may be of use](https://www.openapis.org/blog/2021/02/16/migrating-from-openapi-3-0-to-3-1-0).

## Building manually

The API should ideally be built using Github Actions. However, the steps below are provided should you need to build it locally.

### Installing dependencies

It is recommended to use the "FRM-OpenAPI" devcontainer attached to this repository which can be used with [Github Codespaces](https://docs.github.com/en/codespaces/setting-up-your-project-for-codespaces/adding-a-dev-container-configuration/introduction-to-dev-containers) or [VSCode](https://code.visualstudio.com/docs/devcontainers/containers) to obtain a prebuilt Linux environment with the dependencies ready to go.

Should you wish to build the documentation on Windows you will need at least the following.

 - [YQ](https://github.com/mikefarah/yq?tab=readme-ov-file#windows)
 - [NodeJS](https://nodejs.org/en/download/package-manager)
 - [Java](https://www.microsoft.com/openjdk)
 - [Openapi-generator](https://github.com/OpenAPITools/openapi-generator-cli?tab=readme-ov-file#installation).
 - [ReDoc](https://github.com/Redocly/redocly-cli?tab=readme-ov-file#usage)

The commands below were written with bash in mind. On Windows you will either need to use bash instead of cmd/powershell (such as the "git bash" which comes with the Windows git installer) or adjust the commands accordingly.

### Validation

 1. Validate the YAML.

    ```bash
    yq openapi-*.yaml > /dev/null
    ```

    If it exits with exit code 0 and no output then the YAML files are valid. 

    While this step is not strictly necessary, the `yq` command gives much more useful feedback than the OpenAPI commands if there are errors in the input files.

 2. Validate against the OpenAPI specification.

    ```bash
    openapi-generator-cli validate -i openapi.yaml
    ```

    In typical Java fashion the command will usually print one or more stack traces if anything is wrong, so you may need to scroll up to find the real error message.

 3. Run the linter.

    ```bash
    redocly lint --config redocly.yaml openapi.yaml \
        --skip-rule info-license \
        --skip-rule operation-4xx-response \
        --skip-rule security-defined 
    ```

    This will catch some problems which `openapi-generator-cli` does not.
    Address any errors or warnings that are raised.

    Although ReDoc supports a config file for linting, it [does not make it easy to disable rules globally](https://github.com/Redocly/redocly-cli/issues/1224). The following rules are disabled.

    - info-license - FRM is currently unlicensed.
    - operation-4xx-response - Most endpoints do not ever return 401/403/404/etc.
    - security-defined - FRM has no authn or authz.

### Updating documentation

 1. Generate the documentation.

    ```bash
    redocly build-docs --config redocly.yaml openapi.yaml
    ```

    The resulting `redoc-static.html` file can be opened in your browser or deployed to just about any hosting platform.

### Publish API definition

 1. Convert to JSON.

    JSON is a little more widely-supported than YAML, and we'll also take this opportunity to squish the definitions down into a single file so that they are more portable.

    ```bash
    API_OUTPUT="$(mktemp -d)"
    openapi-generator-cli generate -g openapi -i openapi.yaml -o "${API_OUTPUT}
    echo "Wrote API to ${API_OUTPUT}/openapi.json
    ```

    Writing it out ot a temporary folder prevents the generator from dumping junk files in your CWD.

## Style

 - Descriptions should be full sentences beginning with an uppercase letter and ending with a period.
 - Prefer `example` to `examples` for schemas to save typing since we don't currently use multiple examples.
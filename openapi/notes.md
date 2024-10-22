There is a devcontainer config in the root of this branch which can be setup to make running these tools easier if you want to try them out.

Once the container is running you may need to run the shell script in .devcontainer manually to install the final dependencies.

## Generating asciidoc

1. Ensure that the YAML is valid.

    The `yq` command gives much more useful error messages about invalid YAML than the OpenAPI CLI does.

    ```bash
    yq api.yaml > /dev/null
    ```

    If it prints out nothing then the validation was successful.

 2. Ensure that the OpenAPI spec is valid.

    ```bash
    openapi-generator-cli validate -i api.yaml
    ```

  3. Generate the documentation.

    ```bash
    openapi-generator-cli generate -g asciidoc -i api.yaml
    ```

It's possible to tweak the output a little by creating a config.yaml and passing it to the command with `-c`. There aren't many options, but here are some.

```yaml
---
additionalProperties:
  useIntroduction: true
  useMethodAndPath: true
```

## Generating asciidoc (openapi-to-asciidoc)

 1. This tool only takes JSON as input. Convert using YQ.

```bash
yq -o json api.yaml > api.json
```

```bash
openapi-to-asciidoc -j api.json -o api.adoc
```

## Generating markdown/HTML

See the `openapi-generator-cli` commands above, but replace `-g asciidoc` with `-g markdown`/`-g html`/`-g html2`.

## Generating Redoc

```bash
redocly build-docs api.yaml
```

## Generating swagger

As noted in the main readme this is a bit of a pain. As noted in [the swagger-ui readme](https://github.com/swagger-api/swagger-ui?tab=readme-ov-file#general):

 1. Download swagger-ui
 2. Grab the contents of the `dist` folder.
 3. Open its index.html in your browser.
 4. Paste the URL of the JSON (not yaml) file you want to use into the bar at the top.
    - It seems to need an http/https scheme. I couldn't get file:// to work, so I just uploaded the FRM spec to a gist.
    - You can also edit one of the files in dist to set the default, grep my example for 'gist' if needed.


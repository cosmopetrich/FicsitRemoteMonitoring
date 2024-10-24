This folder contains FRM's HTTP API definitions, written using [OpenAPI](https://www.openapis.org/).

## Structure

It's implemented as a NodeJS project, primarily because most OpenAPI-related tooling is written in JS. However, you don't need to know any JS to work on the API definitions.

The definitions themselves are written in raw YAML and are stored in the [resources/openapi](resources/openapi) directory. The [frm-openapi.yaml](resources/openapi/frm-openapi.yaml) file provides the base API definition and references the other [components/*.yaml](resources/openapi/components/) files using [the $ref keyword](https://swagger.io/docs/specification/v3_0/using-ref/). Breaking them up in this way is not strictly necessary, but does keep the files from being too unweildy.

Under the [tests](tests) directory are some simple integration-style tests, written in Typescript. They're very simple, and merely attempt to test that FRM's endpoints and the API definitions are mostly in alignment.

## Building manually

It is recommended to use the "FRM-OpenAPI" devcontainer attached to this repository which can be used with [Github Codespaces](https://docs.github.com/en/codespaces/setting-up-your-project-for-codespaces/adding-a-dev-container-configuration/introduction-to-dev-containers) or [VSCode](https://code.visualstudio.com/docs/devcontainers/containers) to obtain a prebuilt Linux environment with the dependencies ready to go.

Building directly on Windows should be possible if you [install NodeJS](https://nodejs.org/en/download/prebuilt-installer) and [yq](https://github.com/mikefarah/yq).

#### Install NodeJS packages

```bash
npm install
```

#### Validate the YAML

```bash
npm run openapi-yaml-check
```

If it prints a bunch of YAML then everything is okay. If there's an error then the output will be short and succinct. While this step can be skipped, it tends to give more directly actionable output if there's an error in the YAML syntax when compared to the OpenAPI-related tools.

#### Combine the YAML files into a single JSON file

Note that this will be be done automatically when needed by another command.

```bash
npm run openapi-bundle
```

This use Redoc to generate a `dist/frm-openapi.json` from all the YAML files under `resources/openapi` and set the API version number to something (hopefully) sane. A single JSON file is more portable and widely-supported than multiple YAML files. It uses Redoc since that is already being used to generate documentation and because the official openapi-generator does not support OpenAPI 3.1.0 fully (omits `type`, strange behaviour with `$ref` siblings, etc).


#### Build the documentation

```bash
npm run openapi-docs
```

This will use Redoc to convert the `dist/frm-openapi.json` file into a `dist/redoc-static.html` file. That file can be opened in your browser or deployed to just about any hosting platform.


#### Generate clients

```bash
npm run openapi-clients
```

This will generate the following under `clients/`.

 - Typescript
   - This requires [HeyAPI](https://heyapi.dev) which requires NodeJS.
   - The generated client uses [Fetch](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API) and suitable for use in all web browsers released since 2017 or with NodeJS >=18. In testing, HeyAPI seems to provide slightly better output than the default OpenAPI generator and doesn't spit out a bunch of unnecessary cruft to go along with it.

#### Running tests

```bash
npm run tests
```

## Style




 - Descriptions should be full sentences beginning with an uppercase letter and ending with a period.
 - Prefer `example` to `examples` for schemas to save typing since we don't currently use multiple examples.
 - Avoid using headings etc in endpoint descriptions (gets put in generated code docstrings).
 - Give schemas that are list items a `title` so that generated code won't call them `foo_inner`.

Possibly use https://github.com/IBM/openapi-validator or https://stoplight.io/open-source/spectral 

ClassName:
                        allOf:
                          - $ref: "openapi-schemas-common.yaml#/components/schemas/ClassName"
                          - example: FGTrainStationIdentifier

https://github.com/hey-api/openapi-ts/issues/1183 when using allof

## Misc notes

- When building clients may need to use jq or the like to set the version in the JSON config due to https://github.com/OpenAPITools/openapi-generator-cli/issues/371
- In general the default generated clients seem a little jank compared to e.g.
  - https://github.com/hey-api/openapi-ts
  - https://github.com/openapi-generators/openapi-python-client


Using `$ref` siblings to set specific examples may make the generated code slightly worse.

Given an API definition containing the following schemas:

```
components:
  schemas:
    ExampleNumber:
      type: number
    ExampleRange:
      $ref: "#/components/schemas/ExampleNumber"
      example: 10
```

Most codegen tools appear to generate types along the lines of the following.

```typescript
export type ExampleNumber = number;

export type ExampleRange = ExampleNumber;
```

Far from the end of the world but slightly annoying.

The main openapi.yaml must adhere to the OpenAPI specification. While the other files can be a little more free-form and are required to have specific top-level keys, please attempt to keep the contents of these files under [the components key](https://swagger.io/specification/#components-object) so that we can merge them easily if required.

Note that FRM uses OpenAPI 3.1.0 - the latest version as of October 2024. Some public documentation may still reference 3.0.x and the [notes on what has changed between the two versions may be of use](https://www.openapis.org/blog/2021/02/16/migrating-from-openapi-3-0-to-3-1-0).
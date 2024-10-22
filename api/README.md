This folder contains FRM's HTTP API definitions, written using [OpenAPI](https://www.openapis.org/).

## Structure

It's implemented as a NodeJS project, primarily because most OpenAPI-related tooling is written in JS. However, you don't need to know any JS to work on the API definitions. Everything should work on Windows and you can use Visual Studio.

- API definitions themselves are written in raw YAML and are stored in the [resources/openapi](resources/openapi) directory.
- The [frm-openapi.yaml](resources/openapi/frm-openapi.yaml) provides the base API definition and references the other [components/\*.yaml](resources/openapi/components/) files using [the $ref keyword](https://swagger.io/docs/specification/v3_0/using-ref/).
- Breaking them up in this way is not strictly necessary, but does keep the files from being too unweildy.

Most other files can be ignored, though descriptions are available at the bottom of this file.

## Dependenices

These instructions are for Windows. Linux or MacOS requirements should be easily sourced from the package manager or homebrew.

1.  Install NodeJS.

    The simplest way is to [download the Windows Installer from the official site](https://nodejs.org/en/download/prebuilt-installer) and run it, selecting the defaults when asked.

2.  Install [yq](https://github.com/mikefarah/yq).

    While it's possible to download it directly from Github, by far the easiest way is to use [winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/) which should be preinstalled on Windows 10 and 11.

    ```bash
    winget install --id MikeFarah.yq
    ```

    Instructions for other Windows package managers such as Chocolately and Scoop are available in yq's readme.

3.  Install NodeJS packages.

    Open a terminal (git bash or powershell) and navigate to the directory containing this file (FicsitRemoteMonitoring/api). Then, run the command below.

    ```bash
    npm install
    ```

    As of 2024-10 you may see a warning along the lines of "10 severe security vulnerabilities". This is actually a single vulnerability, CVE-2024-4067/4068, and is [probably a spurious report](https://github.com/micromatch/braces/pull/37#issuecomment-21216496140).
    NPM does not have the ability to mark specific warnings as ignored without [third-party tooling](https://www.npmjs.com/package/better-npm-audit).

If you are using Visual Studio then you _may_ need install Typescript support, though it may also be preinstalled.

- On the top menu select "Tools" then :Get Tools and Features".
- Navigate to the "Individual components" tab.
- Enter "TypeScript" in the search box near the top of the window.
- Check "JavaSCript and TypeScript langauge support" and "TypeScript Server".
- Press "Install while downloading" in the bottom right.

## Usage

### Format everything

After making any changes to YAML, JSON, or JS/TS files in this directory you shoudl run the formatter.

```bash
npm run format
```

This uses [prettier](https://prettier.io/) to enforce a consistent, opinionated style.

### Validate YAML files

After editing one of the OpenAPI YAML files it can be helpful to check if the YAML is valid.

```bash
npm run openapi-yaml-check
```

If it prints a bunch of YAML then everything is okay. If there's an error then the output will be short and succinct.

### Build the documentation

This will generate a `dist/redoc-static.html` file which can be opened in your browser or deployed to just about any hosting platform.

```bash
npm run openapi-docs
```

### Running the tests

```bash
npm run tests
```

This will perform some prerequisite steps like building the client on each invocation, since NPM has no way to track whether those need to be updated.
Those steps can be skipped by invoking `npm run just-tests`. However, there may be unexpected failures if any of the preqreuisites are out of date.

### Other less-commonly-used steps

These are documented for completeness but should not need to be run manually.

#### Combine the YAML files into a single JSON file

Generate a `dist/frm-openapi.json` from all the YAML files under `resources/openapi` and set the API version number to something (hopefully) sane. A single JSON file is more portable and widely-supported than multiple YAML files.

This will be done automatically when needed by one of the `npm run` commands described above.

```bash
npm run openapi-bundle
```

It uses Redoc since that is already being used to generate documentation, the official openapi-generator requires Java, and since the official generator does not support OpenAPI 3.1.0 fully (omits `type`, strange behaviour with `$ref` siblings, etc).

#### Generate clients

Generate client libraries under `dist/clients`.

This will be done automatically when needed by one of the `npm run` commands described above.

```bash
npm run openapi-clients
```

Specifically it will create the following.

- `clients/typescript`
  - Uses [HeyAPI](https://heyapi.dev) which is written in JS and seems to provide better output than the default OpenAPI generator and doesn't spit out a bunch of unnecessary cruft to go along with it.
  - The generated client uses [Fetch](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API) and suitable for use in all web browsers released since 2017 or with NodeJS >=18.

## Writing tests

Under the [test](test) directory are some simple integration-style tests, written in Typescript.
They're very simple, and merely attempt to test that FRM's endpoints and the API definitions are mostly in alignment.
You don't really need to know much Javascript to write them.
There's no need to worry about asynchronous nonsense or callbacks. It pretty much looks like regular code.

Here's a stripped-down and heavily commented example of `tests/getTrainStation.ts` which provides a test suite for the "getTrainStation" endpoint consisting of two tests.

```typescript
// Import the generic Node functions we need for testing
import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

// Import common test definitions
import "./common""

// Import the OpenAPI-generated client
import * as frm from "../dist/clients/typescript";
// Import the OpenAPI-generated types
import type { GetTrainStationResponse, TrainStation } from "../dist/clients/typescript";

// A test suite
describe("getTrainStation", () => {
    // Common variables accessible to all tests that will have their values set in `before`
    // Take care not to *change* these values inside a test or other tests may break
    let resp: GetTrainStationResponse;
    let genericStation: TrainStation;

    // This will run once before the rest of the tests to set things up
    // The `aysnc` keyword lets it handle the asynchronous HTTP request transparently
    before(async () => {
        // Store the response from FRM's HTTP endpoint
        // The `await` keyword lets us treat it like a synchronous call
        // The parentheses around it are part of a 'destructuring assignment'
        ({ data: resp } = await frm.getTrainStation(frm));
        // Find the building that most tests will work on so we don't have to locate it every test
        genericStation = resp.filter(s => s.Name === "Test Station")[0];
    });

    // A test case
    it("has the correct name", () => {
        assert.strictEqual(genericStation.Name, "Test Station");
    });

    // Another test case
    it("has the correct location", () => {
        const expected = { x: 123, y: 456, z:789.012, rotation: 90 };
        assert.deepStrictEqual(genericStation.location, expected);
    });
});
```

Or, since JSON is valid JS/TS, you can check an entire document at once. If the test fails then the output will show which properties are changed or missing.

```typescript
it("has the correct data", () => {
  const expected = {
    Name: "Test Station",
    ClassName: "FGTrainStationIdentifier",
    location: {
      x: 123,
      y: 456,
      z: 789.012,
      rotation: 90,
    },
  };
  assert.deepStrictEqual(genericStation, expected);
});
```

Tests are written for [NodeJS inbuilt test runner](https://nodejs.org/api/test.html) an use [the inbuilt assert function](https://nodejs.org/api/assert.html) so there's no need to worry about learning whatever testing library is the new hotness this week.

The `describe` function defines a [test suite](https://nodejs.org/api/test.html#describe-and-it-aliases) and the `it` functions the test within that suite. Each test should make one or more assertions. If all the assertions come back `true` then the test passes. If any of them come back `false` then the test fails.

Some quick tips are below.

- Use [assert.strictEqual](https://nodejs.org/api/assert.html#assertstrictequalactual-expected-message) to compare basic types like strings, numbers, or booleans.
- When comparing objects or arrays use [assert.deepStrictEqual](https://nodejs.org/api/assert.html#assertdeepstrictequalactual-expected-message) instead.
- If it doesn't matter what order the items in an array is in then you'll need to [sort()](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/sort) both arrays comparing them.
- There's no need to try to sort objects. The `deepStrictEqual` function doesn't take key/property order into account.

Editors such as Visual Studio should run the linter automatically and produce a warning if the linter detects any code that could be improved. The linter can also be run manually.

```bash
npm run tests-lint
```

### Other files in this directory

Most of the other files and folders not detailed above are NodeJS-related cruft.

- The [package.json](https://docs.npmjs.com/cli/v10/configuring-npm/package-json) is the NodeJS project definition. Since this project doesn't shop any production code, only tests, this file is mostly useful for defining the NodeJS libraries that need to be installed for it to work and reusable commands.
- You should never need to look at the [package-lock.json](https://docs.npmjs.com/cli/v10/configuring-npm/package-lock-json), which just stores the specific versions of dependencies that are installed.
- Files under the [scripts](scripts/) directory are JS scripts which are used by NPM.
- The [tsconfig.json](https://www.typescriptlang.org/tsconfig/) configures the Typescript-to-Javascript compiler. Although compiled code is never shipped by this project this file is still used when running Typescript directly and by editors such as Visual Studio.
- Configuration for Redoc, the tool which generates the documentation, is defined in `.redocly.yaml`.
- Linting of the OpenAPI files for common mistakes is performed using Spectral, which is configured with `.spectral.yaml`.
- Formatting of most file types is handled by [prettier](https://prettier.io/) which is configured using `.prettierrc.yaml`.
- [ESLint](https://eslint.org/) handles basic quality checking for the tests (though formatting is left to prettier) and is configured using `eslint.config.mjs`.
- The .esproj (and .user.esproj, if it exists) are [some Visual Studio nonsense](https://learn.microsoft.com/en-us/visualstudio/javascript/javascript-project-system-msbuild-reference?view=vs-2022).
- Visual Studio and many other editors will read `.editorConfig`, which for this directory is configured to match prettier's formatting.

Automatically-generated output will be placed under `dist/` and ignored by git.

## Style

FIXME: Move to spectral.yaml

- Descriptions should be full sentences beginning with an uppercase letter and ending with a period.
- Prefer `example` to `examples` for schemas to save typing since we don't currently use multiple examples.
- Avoid using headings etc in endpoint descriptions (gets put in generated code docstrings).
- Give schemas that are list items a `title` so that generated code won't call them `foo_inner`.

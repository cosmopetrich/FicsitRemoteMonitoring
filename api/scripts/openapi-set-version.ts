// Given the path to an OpenAPI definition as an argument, set the version number of that API.
// The version number will be based on the current git tag or the most recent commit.
// This is written in TS rather than shell since it may be more portable for Windows users.

import { execSync } from "child_process";

// This will be run using `tsx` so drop the first few arguments
const args = process.argv.slice(2);
if (args.length != 1) {
  console.error("usage: openapi-bundle.ts FILENAME");
  process.exit(1);
}
const filename = args[0];

let version: string = "UNKNOWN";
try {
  version = execSync("git describe --always --tag").toString().trim();
} catch (e) {
  console.error("Failed to determine version using git");
  process.exit(1);
}

// Use yq to set the version since it handles preserving the formatting for us
// If we imported the JSON and wrote it out again then things would be re-ordered etc
let yqCmd = `yq e -i '.info.version = "${version}"' ${filename}`;
if (process.platform === "win32") {
  yqCmd = `yq e -i ".info.version = """${version}"""" ${filename}`;
}
try {
  execSync(yqCmd);
} catch (e) {
  console.error(`Failed to update ${filename}`);
  process.exit(1);
}

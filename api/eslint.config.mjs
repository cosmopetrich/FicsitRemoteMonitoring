// Configuration for the Javascript (and Typescript) linter
// Note that this config is in JS because TS support is experimental
// See https://eslint.org/docs/latest/use/configure/configuration-files
import eslint from "@eslint/js";
import tseslint from "typescript-eslint";
import eslintConfigPrettier from "eslint-config-prettier";

export default [
  eslint.configs.recommended,
  ...tseslint.configs.recommended,
  eslintConfigPrettier,
];

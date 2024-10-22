// Common cruft used by multiple tests

import * as frm from "../dist/clients/typescript";

frm.client.setConfig({
  baseUrl: process.env.FRM_HTTP_ADDR ?? "http://localhost:8080",
});

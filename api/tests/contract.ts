import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

import * as frm from "../dist/clients/typescript";
import type { GetTrainStationResponse } from "../dist/clients/typescript";

frm.client.setConfig({
    baseUrl: process.env.FRM_HTTP_ADDR ?? "http://localhost:8080",
});

describe("getTrainStation", () => {
    let resp: GetTrainStationResponse;

    before(async () => {
        const {data: resp} = await frm.getTrainStation(frm);
    });

    it("does stuff", () => {
        assert.strictEqual(resp[0].ClassName, "foo");
    });
});
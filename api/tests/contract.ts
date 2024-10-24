import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

import * as frm from "../clients/typescript";
import type { GetTrainStationResponse } from "../clients/typescript";

describe("getTrainStation", () => {
    let resp: GetTrainStationResponse;

    before(async () => {
        const {data: resp} = await frm.getTrainStation();
    });

    it("does stuff", () => {
        assert.strictEqual(resp[0].ClassName, "foo");
    });
});
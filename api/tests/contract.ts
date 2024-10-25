import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

import * as frm from "../dist/clients/typescript";
import type { GetTrainStationResponse, TrainStation } from "../dist/clients/typescript";

frm.client.setConfig({
    baseUrl: process.env.FRM_HTTP_ADDR ?? "http://localhost:8080",
    
});

describe("getTrainStation", () => {
    let resp: GetTrainStationResponse;
    let genericStation: TrainStation;

    before(async () => {
        ({ data: resp } = await frm.getTrainStation(frm));
        genericStation = resp.filter(s => s.Name === "Load Station")[0];
    });

    it("finds all stations", async () => {
        const actual = resp.map(s => s.Name).sort();
        const expected = [
            "Unload Station",
            "Load Station"
        ].sort();
        assert.deepStrictEqual(actual, expected);
    });

    it("has class name", async () => {
        assert.strictEqual(genericStation.ClassName, "FGTrainStationIdentifier");
    });

    it("has location", async () => {
        const expected = { x: -58000, y: 228800, z: 7400.013671875, rotation: 90 }
        assert.deepStrictEqual(genericStation.location, expected);
    });

    it("has power consumption", { todo: true }, async () => {
        // Not sure what this is currently expected to return
        assert.strictEqual(genericStation.PowerConsumption, 50);
    });

    it("has transfer rate", { todo: true }, async () => {
        // Not sure of a sane way to test this
        assert.strictEqual(genericStation.TransferRate, 123);
    });

    it("has transfer rate", { todo: true }, async () => {
        // Not sure of a sane way to test this
        assert.strictEqual(genericStation.TransferRate, 123);
    });

    it("has inflow rate", { todo: true }, async () => {
        // Not sure of a sane way to test this, possibly non-zero on a inflow-only station?
        // And a second test for zero on an outflow-only station?
        assert.strictEqual(genericStation.InflowRate, 123);
    });

    it("has outflow rate", { todo: true }, async () => {
        // Not sure of a sane way to test this, possibly non-zero on a outflow-only station?
        // And a second test for zero on an inflow-only station?
        assert.strictEqual(genericStation.OutflowRate, 123);
    });

    it("has circuit ID", async () => {
        // Not sure of a sane way to test this, possibly non-zero on a outflow-only station?
        // And a second test for zero on an inflow-only station?
        assert.strictEqual(genericStation.CircuitID, 0.10000000149011612);
    });

    it("has point geometry", async () => {
        let expected = { coordinates: Object.assign(genericStation.location), type: "Point" }
        delete expected.coordinates.rotation;
        assert.deepStrictEqual(genericStation.features.geometry, expected);
    });

    it("has common properties", async () => {
        const expected = {name: genericStation.Name, type: "Train Station"}
        assert.deepStrictEqual(genericStation.features.properties, expected);
    });
});
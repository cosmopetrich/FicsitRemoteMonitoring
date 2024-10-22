import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

import "./common";

import * as frm from "../dist/clients/typescript";
import type {
  GetTrainStationResponse,
  TrainStation,
} from "../dist/clients/typescript";

describe("getTrainStation", () => {
  let resp: GetTrainStationResponse;
  let genericStation: TrainStation;

  before(async () => {
    ({ data: resp } = await frm.getTrainStation(frm));
    genericStation = resp.filter((s) => s.Name === "Load Station")[0];
  });

  it("finds all stations", () => {
    const actual = resp.map((s) => s.Name).sort();
    const expected = ["Unload Station", "Load Station"].sort();
    assert.deepStrictEqual(actual, expected);
  });

  it("has class name", () => {
    assert.strictEqual(genericStation.ClassName, "FGTrainStationIdentifier");
  });

  it("has location", () => {
    const expected = { x: -58000, y: 228800, z: 7400.013671875, rotation: 90 };
    assert.deepStrictEqual(genericStation.location, expected);
  });

  it("has power consumption that includes all platforms but not the station itself", () => {
    // Note that this behaviour is likely wrong and it should be 50 higher for the station itself?
    const expected = genericStation.CargoInventory.reduce(
      (a, b) => a + b.PowerConsumption,
      0,
    );
    assert.strictEqual(genericStation.PowerConsumption, expected);
  });

  it("has transfer rate that is the sum of all connected platforms", () => {
    const expected = genericStation.CargoInventory.reduce(
      (a, b) => a + b.TransferRate,
      0,
    );
    assert.strictEqual(genericStation.TransferRate, expected);
  });

  it("has inflow rate that is the sum of all connected platforms", () => {
    const expected = genericStation.CargoInventory.reduce(
      (a, b) => a + b.InflowRate,
      0,
    );
    assert.strictEqual(genericStation.InflowRate, expected);
  });

  it("does not find connected platforms", () => {
    // This test will be fragile, see #101
    assert.strictEqual(genericStation.CargoInventory.length, 0);
  });

  it("has outflow rate that is the sum of all connected platforms", () => {
    const expected = genericStation.CargoInventory.reduce(
      (a, b) => a + b.OutflowRate,
      0,
    );
    assert.strictEqual(genericStation.OutflowRate, expected);
  });

  it("has circuit ID", () => {
    assert.strictEqual(genericStation.CircuitID, 0.10000000149011612);
  });

  it("has point geometry", () => {
    const expected = {
      coordinates: Object.assign(genericStation.location),
      type: "Point",
    };
    delete expected.coordinates.rotation;
    assert.deepStrictEqual(genericStation.features.geometry, expected);
  });

  it("has common properties", () => {
    const expected = { name: genericStation.Name, type: "Train Station" };
    assert.deepStrictEqual(genericStation.features.properties, expected);
  });
});

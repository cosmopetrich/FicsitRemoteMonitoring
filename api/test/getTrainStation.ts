import { strict as assert } from "node:assert";
import { before, describe, it } from "node:test";

import "./common";

import * as frm from "../dist/clients/typescript";
import type { GetTrainStationResponse } from "../dist/clients/typescript";

describe("getTrainStation", () => {
  let resp: GetTrainStationResponse;

  before(async () => {
    ({ data: resp } = await frm.getTrainStation(frm));
  });

  // White station "AlternatingEmptyPlat"
  it("looks past empty platforms", () => {
    const expected = [
      {
        Name: "Freight Platform",
        ClassName: "Build_TrainDockingStation_C",
        location: {
          x: -75700,
          y: 234800,
          z: 21100.01171875,
          rotation: 90.70710678118655,
        },
        PowerConsumption: 0.10000000149011612,
        TransferRate: 0,
        InflowRate: 0,
        OutflowRate: 0,
        LoadingMode: "Loading",
        LoadingStatus: "Loading",
        DockingStatus: "Waiting To Start",
        Inventory: [],
      },
    ];
    const station = resp.filter((s) => s.Name === "AlternatingEmptyPlat")[0];
    assert.deepStrictEqual(expected, station.CargoInventory);
  });

  // Red stations "2HeadFront2Plat"/"2HeadBack2Plat"
  it("handles double-headed stations", () => {
    const station1 = resp.filter((s) => s.Name === "2HeadFront2Plat")[0];
    const station2 = resp.filter((s) => s.Name === "2HeadBack2Plat")[0];
    assert.deepStrictEqual(
      station1.CargoInventory.sort(),
      station2.CargoInventory.sort(),
    );
  });

  // Green stations "Serial0Front0Plat"/"Serial0Back2Plat"
  it("handles serial stations", () => {
    const station1 = resp.filter((s) => s.Name === "Serial0Front0Plat")[0];
    const station2 = resp.filter((s) => s.Name === "Serial0Back2Plat")[0];
    assert.strictEqual(station1.CargoInventory.length, 0);
  });
});

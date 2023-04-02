const factorytimeout = setInterval(catchFactory, 1000);

var lastData = null;

async function catchFactory() {
  try {
    const response = await fetch("/getFactory");
    const data = await response.text();
    const getFactory = JSON.parse(data);
    displayFactory(getFactory);
  } catch {
    displayFactory(test_Factory);
    showAlert("Error while getting factory data! Using Testing Data.");
  }
}

var fTable = document.createElement("table");
fTable.style.width = "100%";
fTable.className = "table";
document.body.appendChild(fTable);

function displayFactory(getFactory) {
  if (lastData != getFactory) {
    lastData = getFactory;
  } else {
    return;
  }
  fTable.innerHTML = "";
  var thead = document.createElement("thead");
  fTable.append(thead);
  var tr = document.createElement("tr");
  thead.appendChild(tr);

  var lines = [
    "Name:",
    "Recipe:",
    "Efficiently:",
    "Recipe / Current Production:",
    "Ingredients / Current Consumption:",
    "Manufacturing Speed:",
    "Producing:",
    "Paused:",
  ];

  for (var i = 0; i < lines.length; i++) {
    var td = document.createElement("td");
    var b = document.createElement("b");
    b.innerText = lines[i];
    td.appendChild(b);

    var sortContainer = document.createElement("div");
    sortContainer.classList = "sortContainer";
    td.appendChild(sortContainer);

    var asc = document.createElement("button");
    asc.classList = "sortAsc";
    asc.innerText = "⬆️";
    asc.onclick = sort;

    var desc = document.createElement("button");
    desc.classList = "sortDesc";
    desc.innerText = "⬇️";
    desc.onclick = sort;

    sortContainer.appendChild(asc);
    sortContainer.appendChild(desc);

    tr.appendChild(td);
  }

  const tbody = document.createElement("tbody");
  fTable.appendChild(tbody);
  for (var i in getFactory) {
    prod = "";
    need = "";

    for (p in getFactory[i].production) {
      prod +=
        "Output: " +
        getFactory[i].production[p].Name +
        " / " +
        getFactory[i].production[p].CurrentProd.toFixed(2);
    }

    for (n in getFactory[i].ingredients) {
      need +=
        "Output: " +
        getFactory[i].ingredients[n].Name +
        " / " +
        getFactory[i].ingredients[n].CurrentConsumed.toFixed(2) + " ";
    }

    var tr = document.createElement("tr");
    tbody.appendChild(tr);

    var name = document.createElement("td");
    name.style.alignContent = "left";
    name.innerText = getFactory[i].Name;

    var recipe = document.createElement("td");
    recipe.style.alignContent = "center";
    recipe.innerText = getFactory[i].Recipe;

    eff = getFactory[i].production[p].ProdPercent * 1;
    var efficiently = document.createElement("td");
    efficiently.style.alignContent = "center";
    efficiently.innerText = Math.round(eff.toFixed(2)) + "%";

    var production = document.createElement("td");
    production.style.alignContent = "left";
    production.innerText = prod;

    var needs = document.createElement("td");
    needs.style.alignContent = "left";
    needs.innerText = need;

    var manuSpeed = document.createElement("td");
    manuSpeed.style.alignContent = "center";
    manuSpeed.innerText = getFactory[i].ManuSpeed + "%";

    var isProducing = document.createElement("td");
    isProducing.style.alignContent = "center";
    isProducing.innerText = getFactory[i].IsProducing;

    var isPaused = document.createElement("td");
    isPaused.style.alignContent = "center";
    isPaused.innerText = getFactory[i].IsPaused;

    var trLoop = [
      name,
      recipe,
      efficiently,
      production,
      needs,
      manuSpeed,
      isProducing,
      isPaused,
    ];

    for (var i in trLoop) {
      tr.appendChild(trLoop[i]);
    }
  }
}
